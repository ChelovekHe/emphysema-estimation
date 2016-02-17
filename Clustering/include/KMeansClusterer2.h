#ifndef __KMeansClusterer2_h
#define __KMeansClusterer2_h

#include <vector>
#include <cassert>

#include "flann/flann.hpp"

#include "Types.h"
#include "ClustererBase.h"

template< typename TDistanceFunctor >
class KMeansClusterer2
  : public ClustererBase< ee_llp::DoubleRowMajorMatrixType,
			  ee_llp::DoubleColumnVectorType,
			  std::vector< int >,
			  TDistanceFunctor >
{
public:
  typedef ClustererBase< ee_llp::DoubleRowMajorMatrixType,
			 ee_llp::DoubleColumnVectorType,
			 std::vector< int >,
			 TDistanceFunctor > Super;
  
  typedef typename Super::DistanceFunctorType DistanceFunctorType;
  typedef typename Super::MatrixType MatrixType;
  typedef typename Super::VectorType VectorType;
  typedef typename Super::IndexVectorType IndexVectorType;
  typedef typename Super::ClusteringType ClusteringType;
  
  typedef double ElementType;
  
  KMeansClusterer2(int branching=32,
		  int iterations=11,
		  flann::flann_centers_init_t centersInit=flann::FLANN_CENTERS_KMEANSPP,
		  float cbIndex=0.2);
  ~KMeansClusterer2() {};

  // Cluster instances into at most requestedK clusters using dist to
  // calculate distances. The actual number of clusters depends on the
  // branching factor
  ClusteringType cluster( const MatrixType& instances,
			  DistanceFunctorType& dist,
			  std::size_t requestedK ) override;  
  
  // Set/Get parameters
  void setBranching( int branching ) {
    assert( branching > 1 );
    m_Branching = branching;
  }
  int getBranching( ) const { return m_Branching; }

  void setIterations( int iterations ) {
    assert( iterations >= 0 );
    m_Iterations = iterations;
  }
  int getIterations( ) const { return m_Iterations; }

  void setCentersInit( flann::flann_centers_init_t centersInit ) {
    m_CentersInit = centersInit;
  }
  flann::flann_centers_init_t getCentersInit() {
    return m_CentersInit;
  }


private:
  typedef flann::Matrix<ElementType> InternalMatrixType;
  typedef flann::Matrix<int> InternalIndexVectorType;
  int m_Branching;
  int m_Iterations;
  flann::flann_centers_init_t m_CentersInit;
  float m_CbIndex;
};


#include "KMeansClusterer2.hxx"

#endif
