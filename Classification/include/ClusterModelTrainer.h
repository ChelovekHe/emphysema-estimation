#ifndef __ClusterModelTrainer_h
#define __ClusterModelTrainer_h

#include <vector>

#include "libcmaes/cmaes.h"

#include "MatrixOperations.h"
#include "ClusterModelTrainerParameters.h"
#include "ClusterModel.h"

template< typename TClusterer,
	  typename TLabeller >
class ClusterModelTrainer
  : private TClusterer, private TLabeller
{
public:
  typedef ClusterModelTrainer< TClusterer, TLabeller > Self;
  
  typedef typename TClusterer::ClusteringType ClusteringType;
  typedef typename TClusterer::IndexVectorType IndexVectorType;
  typedef typename TClusterer::MatrixType MatrixType;
  typedef typename TClusterer::VectorType VectorType;
  typedef typename TClusterer::DistanceFunctorType DistanceFunctorType;
  typedef ClusterModel< DistanceFunctorType > ClusterModelType;

  using TClusterer::cluster;
  using TLabeller::label;

  /* 
     Struct that holds trace of execution
  */
  struct Trace {
    std::vector< ClusteringType > clusterings;
    std::vector< VectorType > labellings;
    std::vector< double > errors;
  };

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
  ClusterModelTrainer( const VectorType& p,
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
      m_K( 0 )
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

    @return  Status of training 
   */
  int
  train( ClusterModelType& cm,
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
};


template< typename TClusterer, typename TLabeller >
double
ClusterModelTrainer< TClusterer, TLabeller >
::run( const double* weights, const int& N ) {
  DistanceFunctorType dist( weights, N );
  auto clustering = cluster( m_Instances, dist, m_K);

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
  double error = label( m_P, C, labels );

  if ( m_TraceEnabled ) {
    m_Trace.clusterings.push_back( clustering );
    m_Trace.labellings.push_back( labels );
    m_Trace.errors.push_back( error );
  }
  
  return error;

}

template< typename TClusterer, typename TLabeller >
int 
ClusterModelTrainer< TClusterer, TLabeller >
::train( ClusterModelType& cm,
	 const ClusterModelTrainerParameters& params )
{
  m_K = params.k;
  VectorType weights( m_FeatureSpaceDimension );
  
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

  // Wrap the objective for cmaes
  using std::placeholders::_1;
  using std::placeholders::_2;
  std::function< double(const double*, const int&) >
    objective = std::bind( &Self::run, this, _1, _2 );

  // Run the optimization
  CMASolutions solutions = libcmaes::cmaes< GenoPheno >( objective,
							 cmaParams );

  
  // Now we use the weights we found in the optimization to train a model
  DistanceFunctorType dist( weights.data(), weights.size() );
  auto clustering = cluster( m_Instances, dist, m_K );

  MatrixType C = MatrixType::Zero( weights.size(), m_K );
  coOccurenceMatrix( m_BagIndices.begin(),
		     m_BagIndices.end(),
		     clustering.indices.begin(),
		     clustering.indices.end(),
		     C );
  C = rowNormalize(C);
  
  // Find labels 
  VectorType labels = VectorType::Zero( m_K );
  double error = label( m_P, C, labels );

  cm.setCenters( clustering.centers );
  cm.setLabels( labels );
  cm.weights() = weights;

  cm.build();
  
  return solutions.run_status();
}
  
#endif
