#ifndef __NearestNeighbourModel_h
#define __NearestNeighbourModel_h

/*
  A nearest neighbour model
 */

#include <memory>

#include "flann/flann.hpp"

#include "Types.h"

template< typename TDistanceFunctor >
class NearestNeighbourModel {
public:
  typedef NearestNeighbourModel< TDistanceFunctor > Self;
  
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType VectorType;
  typedef std::vector< int > IndexVectorType;
  typedef double ElementType;
  typedef TDistanceFunctor DistanceFunctorType;

  NearestNeighbourModel( )
    : m_Instances(  ),
      m_Labels(  ),
      m_Weights(  ),
      m_Index( ),
      m_InternalInstances( ),
      m_K( 1 )
  {}

  NearestNeighbourModel( Self&& other )
    : m_Instances( std::move(other.m_Instances) ),
      m_Labels( std::move(other.m_Labels) ),
      m_Weights( std::move(other.m_Weights) ),
      m_Index( std::move(other.m_Index) ),
      m_InternalInstances( std::move(other.m_InternalInstances) ),
      m_K( other.m_K )
  {}
  
  NearestNeighbourModel( const MatrixType& instances,
	   const VectorType& labels,
	   const VectorType& weights )
    : m_Instances( instances ),
      m_Labels( labels ),
      m_Weights( weights ),
      m_Index( ),
      m_InternalInstances( ),
      m_K( 1 )
  {}

  ~NearestNeighbourModel() {}


  void setInstances( const MatrixType& instances ) {
    m_Instances = instances;
  }

  void setLabels( const VectorType& labels ) {
    m_Labels = labels;
  }

  VectorType& weights( ) {  return m_Weights;  }
  const VectorType& weights() const { return m_Weights;  }

  void setK( unsigned int k) {
    assert( k > 0 );
    m_K = k;
  }

  unsigned int getK() const { return m_K; }
  
  void build() {
    // This is so ugly
    m_InternalInstances =
      std::move(
        std::unique_ptr<InternalMatrixType>(
	  new InternalMatrixType( const_cast<ElementType*>(m_Instances.data()),
				  m_Instances.rows(),
				  m_Instances.cols() )));
    
    DistanceFunctorType dist( m_Weights.data(), m_Weights.size() );
    
    m_Index = std::move(
		std::unique_ptr< IndexType > (
		  new IndexType( *m_InternalInstances,
				 IndexParamsType(),
				 dist )));
    m_Index->buildIndex( );
  }

  
  VectorType
  predictInstances( const MatrixType& instances ) const {
    // We want to find exact matches
    SearchParamsType searchParams( flann::FLANN_CHECKS_UNLIMITED );
    searchParams.use_heap = flann::FLANN_False;
    searchParams.cores = 0; // Use as many as you like

    const unsigned int N = instances.rows();
    
    // Allocate matrices for the query
    IndexVectorType indices( N*m_K );
    InternalIndexVectorType _indices( indices.data(), N, m_K );
    MatrixType distances( N, m_K );
    InternalMatrixType _distances( distances.data(), distances.rows(), distances.cols() );

    InternalMatrixType
      _instances( const_cast<ElementType*>(instances.data()),
		  instances.rows(),
		  instances.cols() );

    m_Index->knnSearch( _instances, _indices, _distances, m_K, searchParams );
    VectorType predictions = VectorType::Zero( N );
    for ( std::size_t i = 0; i < N; ++i ) {
      for ( std::size_t j = 0; j < m_K; ++j ) {
	predictions(i) += m_Labels( _indices[i][j] );
      }
      predictions(i) /= m_K;
    }
    return std::move( predictions );
  }

  VectorType
  predictBags( const MatrixType& instances,
	       const IndexVectorType& bagLabels,
	       const std::size_t numberOfBags ) const {
    VectorType instancePrediction = predictInstances( instances );
    VectorType bagPrediction = VectorType::Zero( numberOfBags );
    VectorType bagCounts = VectorType::Zero( numberOfBags );
    for ( std::size_t i = 0; i < bagLabels.size(); ++i ) {
      bagPrediction( bagLabels[i] ) += instancePrediction(i);
      bagCounts( bagLabels[i] ) += 1; 
    }
    return std::move( bagPrediction.cwiseQuotient( bagCounts ) );
  }

private:
  typedef typename flann::LinearIndexParams IndexParamsType;
  typedef typename flann::SearchParams SearchParamsType;
  typedef typename flann::LinearIndex< DistanceFunctorType > IndexType;
  typedef flann::Matrix<ElementType> InternalMatrixType;
  typedef flann::Matrix<int> InternalIndexVectorType;
  
  MatrixType m_Instances;
  VectorType m_Labels;
  VectorType m_Weights;
  std::unique_ptr<IndexType> m_Index;
  std::unique_ptr<InternalMatrixType> m_InternalInstances;
  unsigned int m_K;
};

#endif
