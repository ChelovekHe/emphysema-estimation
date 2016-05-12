#ifndef __CMSTrainer_h
#define __CMSTrainer_h

#include "libcmaes/cmaes.h"

#include "CMSModel.h"
#include "CMSTrainerParameters.h"
#include "BaggedDataset.h"

// TODO: Rewrite to return a model

/**

   TWeightedDistanceInstanceClusterer should define the types
     ParameterType
     DistanceType
     InstanceClusteringType
   and the methods
     TClusterer( ParameterType& )
     InstanceClusteringType Cluster( BaggedDataset&, const double*, int )
     
   TLabeler should define the types
     ParameterType
   and the methods
     TLabeler( ParameterType& )
     double Label( BaggedDataset&, BaggedDataset::MatrixType&, VectorType& )
 
   TTracer should define the types
     ParameterType
   and the methods
     TTracer( ParameterType& )
     void Trace( const InstanceClustering&, const VectorType&, double )
*/
template< typename TBaggedDataset,
	  typename TWeightedDistanceInstanceClusterer,
	  typename TClusterLabeler,
	  typename TTracer >
class CMSTrainer {
public:
  typedef TBaggedDataset BaggedDatasetType;
  typedef typename BaggedDatasetType::InstanceLabelVectorType ClusterLabelVectorType; 
  
  typedef TWeightedDistanceInstanceClusterer ClustererType;
  typedef TClusterLabeler                    LabelerType;
  typedef TTracer                            TracerType;
  
  typedef CMSTrainer< BaggedDatasetType, ClustererType, LabelerType, TracerType > Self;

  typedef typename ClustererType::InstanceClusteringType InstanceClusteringType;
  typedef CMSModel< typename ClustererType::DistanceType, BaggedDatasetType > ModelType;

  typedef CMSTrainerParameters                  CMSTrainerParameterType;
  typedef typename ClustererType::ParameterType ClustererParameterType;
  typedef typename LabelerType::ParameterType   LabelerParameterType;
  typedef typename TracerType::ParameterType    TracerParameterType;

  typedef libcmaes::GenoPheno< libcmaes::pwqBoundStrategy > GenoPheno;
  typedef libcmaes::CMAParameters< GenoPheno > CMAParameters;
  typedef libcmaes::CMASolutions CMASolutions;  

  CMSTrainer( CMSTrainerParameterType& trainerParams   = CMSTrainerParameterType(),
	      ClustererParameterType&  clustererParams = ClustererParameterType(),
	      LabelerParameterType&    labelerParams   = LabelerParameterType(),
	      TracerParameterType&     tracerParams    = TracerParameterType() )
    : m_TrainerParams( trainerParams )
    , m_ClustererParams( clustererParams )
    , m_LabelerParams( labelerParams )
    , m_TracerParams( tracerParams )
  {}

  ~CMSTrainer(){}
  
  /**
     \brief Train cluster model
     
     \param model  The model to train
     \patam bags   Training data as a collection of bags
  */
  void Train( ModelType& model, BaggedDatasetType& bags ) {
    // We are searhing for feature weights in [0,1] and we start at the center.
    // It is recomended that sigma is set so the optimal solution is within
    // [0.5 - sigma, 0.5 + sigma]. 
    const auto dim = model.Dimension();
    model.Weights() = std::vector< double >( dim, 0.5 );
    
    std::vector< double > lbounds( dim, 0.0 );
    std::vector< double > ubounds( dim, 1.0 );
    GenoPheno gp( &lbounds.front(), &ubounds.front(), dim );
    CMAParameters cmaParams( dim,
			     model.Weights().data(),
			     m_TrainerParams.sigma,
			     m_TrainerParams.lambda,
			     m_TrainerParams.seed,
			     gp );
    cmaParams.set_algo( aCMAES );

    ClustererType clusterer( m_ClustererParams );
    LabelerType   labeler( m_LabelerParams );
    TracerType    tracer( m_TracerParams );
    
    
    // Define and wrap the objective for cmaes
    std::function< double(const double*, const int&) > objective =
      [&bags, &clusterer, &labeler, &tracer]
      ( const double* w, const int& N )
      {
	InstanceClusteringType clustering = clusterer.Cluster( bags, w, N );
	
	// In some cases we have a clustering algorithm that is not guaranteed to
	// give us the requested number of clusters, so we need to check how many
	// we actually got
	ClusterLabelVectorType labels = ClusterLabelVectorType::Zero( clustering.K() );
	double risk = labeler.Label( bags, clustering.clusterBagMap, labels );
	
	tracer.Trace( clustering, labels, risk );
	return risk;
      };
  
    // Run the optimization
    CMASolutions solutions = libcmaes::cmaes< GenoPheno >( objective, cmaParams );

    // TODO: Handle the diferent ways that CMAES can terminate
    if ( solutions.run_status() < 0 ) {
      std::cerr << "Error occured while training model." << std::endl
		<< "CMA-ES error code: " << solutions.run_status() << std::endl;
      return std::numeric_limits<double>::infinity();
    }

    // Now we use the weights we found in the optimization to train a model and
    // iterate a couple of times to give an idea of how stable the clustering is
    model.Weights() = gp.pheno( solutions.best_candidate().get_x_dvec() );

    std::cout << "Used " << solutions.niter() << " iterations" << std::endl
	      << " == Weights ==" << std::endl << model.Weights() << std::endl;
 
    double bestRisk = std::numeric_limits<double>::infinity();  
    for ( int i = 0; i < m_TrainerParams.finalNumberOfClusterings; ++i ) {
      InstanceClusteringType clustering = clusterer.Cluster( bags, model.Weights().data(), model.Weights().size() );
      ClusterLabelVectorType labels = ClusterLabelVectorType::Zero( clustering.K() );
      double risk = labeler.Label( bags, clustering.clusterBagMap, labels );

      std::cout << risk << std::endl;

      if ( risk < bestRisk ) {
	bestRisk = risk;
	model.Centroids() = clustering.centroids;
	model.Labels( ) = labels;
      }
    }

    if ( !m_TrainerParams.out.empty() ) {
      std::string modelFile = m_TrainerParams.out + ".model";
      std::ofstream o( modelFile );
      if ( o.good() ) {
	o << model;
      }
      else {
	std::cerr << "Error writing model to " << modelFile << std::endl;
      }
    }
  
    model.Build();
  
    return bestRisk;
  }

  
 
protected:
  CMSTrainerParameterType m_TrainerParams;
  ClustererParameterType  m_ClustererParams;
  LabelerParameterType    m_LabelerParams;
  TracerParameterType     m_TracerParams;  
};


#endif
