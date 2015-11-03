#ifndef __KMeansClusterer_h
#define __KMeansClusterer_h

#include <vector>
#include <cassert>

#include "flann/flann.hpp"
#include "Eigen/Dense"

template< typename MatrixType, typename VectorType >
struct ClusteringResult {
  ClusteringResult(MatrixType _centers, VectorType _indices )
    : centers(_centers), indices(_indices) {}
  MatrixType centers;
  VectorType indices;
};


template< typename TDistanceFunctor >
class KMeansClusterer {
public:
  typedef TDistanceFunctor DistanceFunctorType;
  typedef typename DistanceFunctorType::ElementType ElementType;
  typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
  typedef std::vector<int> IndexVectorType;
  typedef ClusteringResult< MatrixType, IndexVectorType > ResultType;
  
  KMeansClusterer(int branching=32,
		  int iterations=11,
		  flann::flann_centers_init_t centersInit=flann::FLANN_CENTERS_KMEANSPP,
		  float cbIndex=0.2);
  ~KMeansClusterer() {};

  // Cluster instances into at most requestedK clusters using dist to
  // calculate distances. The actual number of clusters depends on the
  // branching factor
  ResultType cluster( MatrixType& instances,
		      DistanceFunctorType& dist,
		      size_t requestedK );
  
  
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


#include "KMeansClusterer.hxx"

#endif
