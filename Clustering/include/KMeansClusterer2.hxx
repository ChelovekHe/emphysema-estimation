#ifndef __KMeansClusterer2_hxx
#define __KMeansClusterer2_hxx

#include "KMeansClusterer2.h"

template< typename TDistanceFunctor >
KMeansClusterer2< TDistanceFunctor >
::KMeansClusterer2(int branching,
		  int iterations,
		  flann::flann_centers_init_t centersInit,
		  float cbIndex)
  : m_Branching(branching),
    m_Iterations(iterations),
    m_CentersInit(centersInit),
    m_CbIndex(cbIndex)
{}



template< typename TDistanceFunctor >
typename KMeansClusterer2< TDistanceFunctor >::ClusteringType
KMeansClusterer2< TDistanceFunctor >
::cluster( const MatrixType& instances,
	   DistanceFunctorType& dist,
	   std::size_t requestedK
	   ) {
  // Figure out the actual number of clusters
  // We need to have that the following equation holds
  // (m_branching - 1) * n + 1 = k
  // for some n >= 0.
  if ( requestedK < m_Branching ) {
    throw std::invalid_argument( "k must be >= branching" );
  }
  auto n =  (requestedK - 1) / (m_Branching - 1);
  size_t k = (m_Branching - 1) * n + 1;  
  assert( k <= requestedK );
  assert( (k-1) % (m_Branching-1) == 0 );

  // Setup the data for flann
  InternalMatrixType
    _instances( const_cast<ElementType*>(instances.data()),
		instances.rows(),
		instances.cols());

  MatrixType centers( k, instances.cols() );
  InternalMatrixType
    _centers( centers.data(), centers.rows(), centers.cols() );


  // Setup the parameters for the clustering
  flann::KMeansIndexParams kmeansParams( m_Branching,
					 m_Iterations,
					 m_CentersInit,
					 m_CbIndex );

  // Get the cluster centers
  int actualK =
    flann::hierarchicalClustering< DistanceFunctorType >( _instances,
							  _centers,
							  kmeansParams,
							  dist );

  // We have specifically calculated k so it is feasible
  assert( actualK == k );

  
  // Now we build an index on the centers and query with the instances
  //flann::KDTreeIndexParams kdTreeParams( 1 );
  //  flann::KDTreeIndex< DistanceFunctorType >kdTree ( _centers, kdTreeParams, dist );
  //  kdTree.buildIndex();
  flann::LinearIndexParams centersIndexParams;
  flann::LinearIndex< DistanceFunctorType > centersIndex ( _centers, centersIndexParams, dist );
  centersIndex.buildIndex();

  // Allocate matrices for the query
  IndexVectorType indices( instances.rows() );
  InternalIndexVectorType _indices( indices.data(), indices.size(), 1 );
  MatrixType distances( instances.rows(), 1 );
  InternalMatrixType _distances( distances.data(),
				 distances.rows(),
				 distances.cols() );

  // Setup search params
  flann::SearchParams searchParams( flann::FLANN_CHECKS_UNLIMITED );
  searchParams.use_heap = flann::FLANN_False;
  searchParams.cores = 0;

  // Do the search and return the result
  centersIndex.knnSearch( _instances,
			  _indices,
			  _distances,
			  1, // We only want one neighbour
			  searchParams );
  
  return ClusteringType( centers, indices, distances );
}

#endif
