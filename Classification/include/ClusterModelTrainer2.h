#ifndef __ClusterModelTrainer2_h
#define __ClusterModelTrainer2_h

#include <vector>

#include "libcmaes/cmaes.h"

#include "MatrixOperations.h"
#include "ClusterModelTrainerParameters.h"
#include "ClusterModel.h"

template< typename TClusterer,
	  typename TLabeller >
class ClusterModelTrainer2
{
public:
  typedef ClusterModelTrainer2< TClusterer, TLabeller > Self;

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

    @param data   Matrix containing the instances in each row. 
    The next to last column is assumed to be the bag proportions.
    The last column is assumed to be the bag membership labels.

    @param featureSpaceDimension  Number of "top-level" features. If the 
    feature space is a set of objects, each represented  by a vector, 
    then the size of each vector is instances.size()/featureSpaceDimension

    @param trace       Flag controlling if a trace of the execution should
                       be stored.
   */
  ClusterModelTrainer2( const MatrixType& data,
		       const unsigned int& featureSpaceDimension,
		       bool trace=false )
    :
    m_Instances( data.leftCols( data.cols() - 2 ) ),
    m_P( data.col( data.cols() - 2 ) ),
    m_FeatureSpaceDimension( featureSpaceDimension ),
    m_BagIndices( data.rows() ),
    m_TraceEnabled( trace ),
    m_Trace( ),
    m_K( 0 ),
    m_Clusterer(),
    m_Labeller()
  {
    VectorType bagIndices = data.col( data.cols() - 1 );
    for ( std::size_t i = 0; i < bagIndices.size(); ++i ) {
      m_BagIndices[i] = static_cast<int>( bagIndices(i) );
    }    
  }
  
  /*
    Constructor

    @param p          Known label proportions for bags
    @param instances  Training instances
    @param featureSpaceDimension  Number of "top-level" features. If the 
                                  feature space is a set of objects, each
				  represented  by a vector, then the size 
				  of each vector is 
				  instances.size()/featureSpaceDimension
    @param bagIndices  Bag membership vector, mapping instances to bags
    @param trace       Flag controlling if a trace of the execution should
                       be stored.
   */
  ClusterModelTrainer2( const VectorType& p,
		       const MatrixType& instances,
		       const unsigned int& featureSpaceDimension,
		       const IndexVectorType& bagIndices,
		       bool trace=false )
    : m_P( p ),
      m_Instances( instances ),
      m_FeatureSpaceDimension( featureSpaceDimension ),
      m_BagIndices( bagIndices ),
      m_TraceEnabled( trace ),
      m_Trace( ),
      m_K( 0 ),
      m_Clusterer(),
      m_Labeller()
  {}

  void traceOn() { m_TraceEnabled = true; }
  void traceOff() { m_TraceEnabled = false; }
  bool traceStatus() { return m_TraceEnabled; }
  const Trace& getTrace() { return m_Trace; }

  std::size_t getMaxIterations();
  void setMaxIterations(int maxIterations);
  
  /*
    Train a cluster model.

    @param cm      The cluster model to train  [OUT]
    @param params  Parameters for the training [IN]

    @return  Training loss
   */
  double
  train( ModelType& cm,
	 const ClusterModelTrainerParameters& params);
  
private:
    
  typedef libcmaes::GenoPheno< libcmaes::pwqBoundStrategy > GenoPheno;
  typedef libcmaes::CMAParameters< GenoPheno > CMAParameters;
  typedef libcmaes::CMASolutions CMASolutions;

  /*
    Evaluate the cluster model objective function at a point in parameter
    space.

    @param weights  Array of weights
    @param N        Size of weights array
    
    @return         Objective value calculated at weights
   */
  double run( const double* weights, const int& N );

  VectorType m_P;
  MatrixType m_Instances;
  const unsigned int m_FeatureSpaceDimension;
  IndexVectorType m_BagIndices;
  bool m_TraceEnabled;
  Trace m_Trace;
  std::size_t m_K;
  ClustererType m_Clusterer;
  LabellerType m_Labeller;
};


template< typename TClusterer, typename TLabeller >
double
ClusterModelTrainer2< TClusterer, TLabeller >
::run( const double* weights, const int& N ) {
  DistanceFunctorType dist( weights, N );
  auto clustering = m_Clusterer.cluster( m_Instances, dist, m_K);

  // Find mapping from clusters to bags
  const std::size_t k = clustering.centers.rows();

  assert( m_BagIndices.size() == clustering.indices.size() );
  MatrixType C = MatrixType::Zero( m_P.size(), k );
  coOccurenceMatrix( m_BagIndices.begin(),
		     m_BagIndices.end(),
		     clustering.indices.begin(),
		     clustering.indices.end(),
		     C );
  C = rowNormalize(C);
  
  // Find labels 
  VectorType labels = VectorType::Zero( k );
  double loss = m_Labeller.label( m_P, C, labels );

  if ( m_TraceEnabled ) {
    m_Trace.clusterings.push_back( clustering );
    m_Trace.labellings.push_back( labels );
    m_Trace.losses.push_back( loss );
  }
  
  return loss;

}

template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainer2< TClusterer, TLabeller >
::train( ModelType& cm,
	 const ClusterModelTrainerParameters& params )
{
  m_K = params.k;
  m_Clusterer.setBranching( params.branching );
  VectorType weights = VectorType::Ones( m_FeatureSpaceDimension ) * 0.5;
  
  // We want to find a weighting of the feature space, such that the
  // weights are constrained to lie in [0,1].
  std::vector< double > lBounds( weights.size(), 0.0 );
  std::vector< double > uBounds( weights.size(), 1.0 );
  GenoPheno gp( &lBounds.front(), &uBounds.front(), weights.size() );
  
  CMAParameters cmaParams( weights.size(),
			   weights.data(),
			   params.sigma,
			   params.lambda,
			   params.seed,
			   gp );
  cmaParams.set_algo( aCMAES );

  if ( params.maxIterations > 0 ) {
    cmaParams.set_max_iter( params.maxIterations );
  }

  if ( !params.out.empty() ) {
    cmaParams.set_fplot( params.out );
  }
  
  // Wrap the objective for cmaes
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::function< double(const double*, const int&) >
    objective = std::bind( &Self::run, this, _1, _2 );

  // Run the optimization
  CMASolutions solutions = libcmaes::cmaes< GenoPheno >( objective,
							 cmaParams );

  // TODO: Handle the diferent ways that CMAES can terminate
  if ( solutions.run_status() < 0 ) {
    std::cerr << "Error occured while training model." << std::endl
	      << "CMA-ES error code: " << solutions.run_status() << std::endl;
    return std::numeric_limits<double>::infinity();
  }
  
  // Now we use the weights we found in the optimization to train a model
  DistanceFunctorType dist( weights.data(), weights.size() );
  auto clustering = m_Clusterer.cluster( m_Instances, dist, m_K );

  const std::size_t k = clustering.centers.rows();

  assert( m_BagIndices.size() == clustering.indices.size() );
  MatrixType C = MatrixType::Zero( m_P.size(), k );
  coOccurenceMatrix( m_BagIndices.begin(),
		     m_BagIndices.end(),
		     clustering.indices.begin(),
		     clustering.indices.end(),
		     C );
  C = rowNormalize(C);

  // Find labels 
  VectorType labels = VectorType::Zero( k );
  double loss = m_Labeller.label( m_P, C, labels );

  cm.setCenters( clustering.centers );
  cm.setLabels( labels );
  cm.weights() = weights;

  cm.build();
  
  return loss;
}
  
#endif
