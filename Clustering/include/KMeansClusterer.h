#ifndef __Clustering_h
#define __Clustering_h

#include <vector>
#include <cassert>

#include "flann/flann.hpp"

template< typename Distance >
class KMeansClusterer {
public:
  typedef typename Distance::ElementType ElementType;
  typedef flann::Matrix<ElementType> MatrixType;
  
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
		   flann_centers_init_t centers_init=FLANN_CENTERS_RANDOM,
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
  cluster( const MatrixType& instances, Distance& dist ) {
    MatrixType centers( &m_CentersBuffer[0], m_NClusters, m_NFeatures );
    int actualNClusters =
      hierarchicalClustering< Distance >( instances, centers, m_Params, dist );
    assert( actualNCLusters > 0 );

    // If we get fewer clusters than requested we just "resize" the matrix so the
    // caller dont have to handle it.
    if ( static_cast<size_t>(nActualClusters) < m_NClusters ) {
      centers.rows = static_cast<size_t>( nActualClusters );
    }
    return centers;
  }
    
private:
  const size_t m_NClusters;
  const size_t m_NFeatures;
  std::vector< ElementType > m_CentersBuffer;
  KMeansIndexParams m_Params;
};

#endif
