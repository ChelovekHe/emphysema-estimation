#ifndef __ClusterModel_h
#define __ClusterModel_h

/*
  A cluster model should label instances according to 
  a basis given by labelled centers and a distance function

 */

#include <memory>
#include <limits>
#include <istream>
#include <ostream>
#include <ios>

#include "flann/flann.hpp"

#include "Types.h"

template< typename TDistanceFunctor >
class ClusterModel {
public:
  typedef ClusterModel< TDistanceFunctor > Self;
  
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType VectorType;
  typedef std::vector< int > IndexVectorType;
  typedef double ElementType;
  typedef TDistanceFunctor DistanceFunctorType;

  template< typename T >
  friend std::ostream& operator<<(std::ostream& os, const ClusterModel<T>& obj);
  
  ClusterModel( )
    : m_Centers(  ),
      m_Labels(  ),
      m_Weights(  ),
      m_Index( ),
      m_InternalCenters( )
  {}

  ClusterModel( Self&& other )
    : m_Centers( std::move(other.m_Centers) ),
      m_Labels( std::move(other.m_Labels) ),
      m_Weights( std::move(other.m_Weights) ),
      m_Index( std::move(other.m_Index) ),
      m_InternalCenters( std::move(other.m_InternalCenters) )
  {}
  
  ClusterModel( const MatrixType& centers,
		const VectorType& labels,
		const VectorType& weights )
    : m_Centers( centers ),
      m_Labels( labels ),
      m_Weights( weights ),
      m_Index( ),
      m_InternalCenters( )
  {}

  ~ClusterModel() {}


  void setCenters( const MatrixType& centers ) {
    m_Centers = centers;
  }

  void setLabels( const VectorType& labels ) {
    m_Labels = labels;
  }

  VectorType& weights( ) {  return m_Weights;  }
  const VectorType& weights() const { return m_Weights;  }
  
  void build() {
    // This is so ugly
    m_InternalCenters =
      std::move(
		std::unique_ptr<InternalMatrixType>(
      new InternalMatrixType( const_cast<ElementType*>(m_Centers.data()),
			      m_Centers.rows(),
			      m_Centers.cols()
			      )
						    ));
    
    DistanceFunctorType dist( m_Weights.data(),
			      m_Weights.size() );
    
    m_Index = std::move( std::unique_ptr< IndexType > (new IndexType( *m_InternalCenters,
			     IndexParamsType(),
								      dist )
						       ));
    m_Index->buildIndex( );
  }

  
  VectorType
  predictInstances( const MatrixType& instances ) const {
    // We want to find exact matches
    SearchParamsType searchParams( flann::FLANN_CHECKS_UNLIMITED );
    searchParams.use_heap = flann::FLANN_False;
    searchParams.cores = 0; // Use as many as you like
    
    // Allocate matrices for the query
    IndexVectorType indices( instances.rows() );
    InternalIndexVectorType _indices( indices.data(), indices.size(), 1 );
    MatrixType distances( instances.rows(), 1 );
    InternalMatrixType _distances( distances.data(), distances.rows(), distances.cols() );

    InternalMatrixType
      _instances( const_cast<ElementType*>(instances.data()),
		  instances.rows(),
		  instances.cols() );

    m_Index->knnSearch( _instances, _indices, _distances, 1, searchParams );
    VectorType predictions( indices.size() );
    for ( std::size_t i = 0; i < predictions.size(); ++i ) {
      predictions(i) = m_Labels( indices[i] );
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
  
  MatrixType m_Centers;
  VectorType m_Labels;
  VectorType m_Weights;
  std::unique_ptr<IndexType> m_Index;
  std::unique_ptr<InternalMatrixType> m_InternalCenters;    
};

template< typename TDistanceFunctor >
std::ostream& operator<<(std::ostream& os, const ClusterModel<TDistanceFunctor>& obj) {
  os << "# number of weights   number of clusters   dimension of feature space" << std::endl
     << obj.m_Weights.size() << "   " 
     << obj.m_Centers.rows() << "   "
     << obj.m_Centers.cols() << std::endl
     << obj.m_Weights << std::endl
     << obj.m_Labels << std::endl
     << obj.m_Centers;
  return os;
}

template< typename TDistanceFunctor >
std::istream& operator>>(std::istream& is, ClusterModel<TDistanceFunctor>& obj) {
  char c;
  is >> c;
  if ( c != '#' ) {
    is.setstate( std::ios::failbit );
    return is;
  }
  // Skip the line
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  
  std::size_t nWeights, nClusters, nFeatures;
  is >> nWeights >> nClusters >> nFeatures;
  if ( !is || nWeights > nFeatures ) {
    is.setstate( std::ios::failbit );
    return is;
  }
  
  typedef ClusterModel<TDistanceFunctor> ModelType;
  typename ModelType::VectorType weights( nWeights ), labels( nClusters );
  typename ModelType::MatrixType centers( nClusters, nFeatures );

  for ( std::size_t i = 0; i < nWeights; ++i ) {
    is >> weights(i);
  }
  
  for ( std::size_t i = 0; i < nClusters; ++i ) {
    is >> labels(i);
  }

  for ( std::size_t i = 0; i < nClusters; ++i ) {
    for ( std::size_t j = 0; j < nFeatures; ++j ) {
      is >> centers(i,j);
    }
  }

  obj.weights() = weights;
  obj.setLabels( labels );
  obj.setCenters( centers );
  obj.build();
  
  return is;
}



#endif
