#ifndef __ClusterModelTrainer3_h
#define __ClusterModelTrainer3_h

#include <vector>

#include "libcmaes/cmaes.h"

#include "MatrixOperations.h"
#include "ClusterModelTrainerParameters.h"
#include "ClusterModel.h"

template< typename TClusterer,
	  typename TLabeller >
class ClusterModelTrainer3
{
public:
  typedef ClusterModelTrainer3< TClusterer, TLabeller > Self;

  typedef TClusterer ClustererType;
  typedef TLabeller LabellerType;
  
  typedef typename TClusterer::ClusteringType ClusteringType;
  typedef typename TClusterer::IndexVectorType IndexVectorType;
  typedef typename TClusterer::MatrixType MatrixType;
  typedef typename TClusterer::VectorType VectorType;
  typedef typename TClusterer::DistanceFunctorType DistanceFunctorType;
  typedef ClusterModel< DistanceFunctorType > ModelType;

  typedef ClusterModelTrainerParameters ParamsType;
  typedef double LossResultType;
  
  /* 
     Struct that holds trace of execution
  */
  struct Trace {
    std::vector< ClusteringType > clusterings;
    std::vector< VectorType > labellings;
    std::vector< LossResultType > losses;
  };

  /*
    Constructor

    @param clusterer  Object that can cluster instances 
    @param labeller   Object that can label a clustering 
    @param params     Parameters for the training 
   */
  ClusterModelTrainer3( ClustererType& clusterer,
			LabellerType& labeller,
			ClusterModelTrainerParameters& params )
    : m_Clusterer( clusterer ),
      m_Labeller( labeller ),
      m_Params( params ),
      m_Trace( )
  { }
  

  const Trace& getTrace() { return m_Trace; }

  /*
    Train a cluster model.

    @param p  Vector of bag proportions [IN]
    @param instances  Matrix of instances. One instance per row. [IN]
    @param bagLabels  Vector mapping instances to bags in p. [IN]
    @param cm      The cluster model to train  [OUT]

    @return  Training loss
   */
  double
  train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm);
  
private:
  typedef libcmaes::GenoPheno< libcmaes::pwqBoundStrategy > GenoPheno;
  typedef libcmaes::CMAParameters< GenoPheno > CMAParameters;
  typedef libcmaes::CMASolutions CMASolutions;

  ClustererType& m_Clusterer;
  LabellerType& m_Labeller;
  ClusterModelTrainerParameters& m_Params;
  Trace m_Trace;
};


template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainer3< TClusterer, TLabeller >
::train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm )
{
  VectorType weights =
    VectorType::Ones( m_Params.featureSpaceDimension ) * 0.5;
  
  // We want to find a weighting of the feature space, such that the
  // weights are constrained to lie in [0,1].
  std::vector< double > lBounds( weights.size(), 0.0 );
  std::vector< double > uBounds( weights.size(), 1.0 );
  GenoPheno gp( &lBounds.front(), &uBounds.front(), weights.size() );
  
  CMAParameters cmaParams( weights.size(),
			   weights.data(),
			   m_Params.sigma,
			   m_Params.lambda,
			   m_Params.seed,
			   gp );
  cmaParams.set_algo( aCMAES );

  if ( m_Params.maxIterations > 0 ) {
    cmaParams.set_max_iter( m_Params.maxIterations );
  }

  if ( !m_Params.out.empty() ) {
    cmaParams.set_fplot( m_Params.out );
  }
  
  // Define and wrap the objective for cmaes
  std::function< double(const double*, const int&) >
    objective =
    [&p, &instances, &bagLabels, this]
    ( const double* w, const int& N )
    {
      DistanceFunctorType d( w, N );
    
      // Find clustering
      auto clustering =
      this->m_Clusterer.cluster( instances, d, this->m_Params.k );
      
      // Get the actual number of clusters
      const std::size_t k = clustering.centers.rows();

      // Verify we got one cluster index for each instance
      assert( bagLabels.size() == clustering.indices.size() );
      
      // Calculate the mapping from clusters to bags
      MatrixType C = MatrixType::Zero( p.size(), k );
      coOccurenceMatrix( bagLabels.begin(),
			 bagLabels.end(),
			 clustering.indices.begin(),
			 clustering.indices.end(),
			 C );
      C = rowNormalize(C);
  
      // Find labels 
      VectorType labels = VectorType::Zero( k );
      double loss = this->m_Labeller.label( p, C, labels );

      // Store trace if requested
      if ( this->m_Params.trace ) {
	this->m_Trace.clusterings.push_back( clustering );
	this->m_Trace.labellings.push_back( labels );
	this->m_Trace.losses.push_back( loss );
      }
      return loss;
    }; // End of objective
  
  // Run the optimization
  CMASolutions solutions =
    libcmaes::cmaes< GenoPheno >( objective, cmaParams );

  // TODO: Handle the diferent ways that CMAES can terminate
  if ( solutions.run_status() < 0 ) {
    std::cerr << "Error occured while training model." << std::endl
	      << "CMA-ES error code: " << solutions.run_status() << std::endl;
    return std::numeric_limits<double>::infinity();
  }
  
  // Now we use the weights we found in the optimization to train a model
  DistanceFunctorType dist( weights.data(), weights.size() );
  auto clustering = m_Clusterer.cluster( instances, dist, m_Params.k );

  const std::size_t k = clustering.centers.rows();

  assert( bagLabels.size() == clustering.indices.size() );
  MatrixType C = MatrixType::Zero( p.size(), k );
  coOccurenceMatrix( bagLabels.begin(),
		     bagLabels.end(),
		     clustering.indices.begin(),
		     clustering.indices.end(),
		     C );
  C = rowNormalize(C);

  // Find labels 
  VectorType labels = VectorType::Zero( k );
  double loss = m_Labeller.label( p, C, labels );

  // Build the model
  cm.setCenters( clustering.centers );
  cm.setLabels( labels );
  cm.weights() = weights;

  cm.build();
  
  return loss;
}
  
#endif
