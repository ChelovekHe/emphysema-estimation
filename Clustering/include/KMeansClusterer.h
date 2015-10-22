#ifndef __Clustering_h
#define __Clustering_h

#include <vector>
#include <cassert>

#include "flann/flann.hpp"
#include "Eigen/Dense"

template< typename Distance >
class KMeansClusterer {
public:
  typedef typename Distance::ElementType ElementType;
  typedef Eigen::Matrix<ElementType, Eigen::Dynamic, Eigen::Dynamic> MatrixType;
  
  // The number of cluster will be at most nClusters.
  // If exactly nClusters are needed, then make sure that branching and nClusters
  // satisfy
  // (branching - 1) * k + 1 = nCLusters, for k >= 0
  // e.g. (16 - 1) * 1 + 1 = 16
  //      (32 - 1) * 2 + 1 = 63
  // Branching should probably be some power of 2
  KMeansClusterer( size_t nClusters,
		   size_t nFeatures,
		   // Flann parameters with defaults from flann
		   int branching=32, 
		   int iterations=11,
		   flann::flann_centers_init_t centers_init=flann::FLANN_CENTERS_RANDOM,
		   float cb_index=0.2 )
    : m_NClusters( nClusters ),
      m_NFeatures( nFeatures ),
      m_CentersBuffer( nClusters * nFeatures ),
      m_Params( branching, iterations, centers_init, cb_index )
  { }

  // void setIterations( int iterations ) {
  //   assert( iterations >= 0 );
  //   m_Params["iterations"] = iterations;
  // }

  // void setBranching( int branching ) {
  //   assert( branching >= 0 );
  //   m_Params["branching"] = branching;
  // }


  MatrixType
  cluster( MatrixType& instances, Distance& dist ) {
    InternalMatrixType _instances( instances.data(), instances.rows(), instances.cols());
    InternalMatrixType _centers( &m_CentersBuffer[0], m_NClusters, m_NFeatures );
    int nActualClusters =
      flann::hierarchicalClustering< Distance >( _instances, _centers, m_Params, dist );
    assert( nActualClusters > 0 );

    // Copy the centers to a new matrix
    auto rows = nActualClusters;
    auto cols = m_NFeatures;
    MatrixType centers(rows, cols);
    std::copy( _centers.ptr(), _centers.ptr() + rows*cols, centers.data() );
    return centers;
  }
    
private:
  typedef flann::Matrix<ElementType> InternalMatrixType;
  const size_t m_NClusters;
  const size_t m_NFeatures;
  std::vector< ElementType > m_CentersBuffer;
  flann::KMeansIndexParams m_Params;
};

#endif
