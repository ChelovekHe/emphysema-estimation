#ifndef __KMeansWeightedDistanceInstanceClusterer_h
#define __KMeansWeightedDistanceInstanceClusterer_h

#include <vector>
#include <cassert>

#include "flann/flann.hpp"

#include "BaggedDataset.h"
#include "InstanceClustering.h"
#include "KMeansClusteringParameters.h"

template< typename TBaggedDataset, typename TWeightedDistance >
class KMeansWeightedDistanceInstanceClusterer
{
public:
  typedef TBaggedDataset BaggedDatasetType;
  typedef TWeightedDistance DistanceType;
  typedef KMeansWeightedDistanceInstanceClusterer< BaggedDatasetType, DistanceType > Self;

  typedef KMeansClusteringParameters ParameterType;
  
  typedef typename BaggedDatasetType::MatrixType MatrixType;
  typedef InstanceClustering< MatrixType > InstanceClusteringType;
  
  KMeansWeightedDistanceInstanceClusterer( const ParameterType& params ); 
  ~KMeansWeightedDistanceInstanceClusterer() {};


  /**
     Cluster all instances from bags using the weighted featurespace defined by
     DistanceType and weights.

     @param bags             A collection of bags of instances.
     @param weights          Feature space weights
     @param numberOfWeights  Length of weights array
     @return                 A clustering of instances n bags
  */
  InstanceClusteringType Cluster( BaggedDatasetType& bags, const double* weights, const int& numberOfWeights )  {
    DistanceType dist( weights, numberOfWeights );

    // We use a k-means tree to do the clustering. The possible number of
    // clusters we can have is given by the equation
    //   (branching - 1) * n + 1 = k ,
    // where branching is the number of branches at each node of the tree, n is
    // an integer >= 0 and k is the allowed number of clusters.
    //
    // We set the numnber of clusters to the smallest allowed k that is not less
    // than the requested number of clusters.

    if ( m_Params.k <= m_Params.branching ) {
      m_Params.k = m_Params.branching;
    }
    else {  
      auto n =  (m_Params.k - 1) / (m_Params.branching - 1);
      m_Params.k = (m_Params.branching - 1) * n + 1;  
    }
    assert( (m_Params.k-1) % (m_Params.branching-1) == 0 );

    
    // Flann has a simple matrix type that assumes data is row-major-order and
    // pre-allocated.
    // BaggedDatasetType is designed with a row-major Eigen matrix, but if someone
    // has fiddled with it and made it into a column-major matrix, we will get
    // bogus results and no error because the ammount of allocated memory is the
    // same.
    // TODO: Make this a compile-time check of MatrixType

    if ( ! bags.Instances().IsRowMajor ) {
      throw std::logic_error( "Matrix storage order must be row-major" );
    }

    FlannMatrixType flannInstances ( const_cast<double*>( bags.Instances.data() ),
				     bags.NumberOfInstances(),
				     bags.Dimension() );


    InstanceClusteringType clustering;
    clustering.centroids = MatrixType( m_Params.k, bags.Dimension() );
    FlannMatrixType flannCentroids( clustering.centroids.data(), m_Params.k, bags.Dimension() );


    flann::KMeansIndexParams kmeansParams( m_Params.branching,
					   m_Params.iterations,
					   m_Params.centersInit,
					   m_Params.cbIndex );

    int actualK = flann::hierarchicalClustering< DistanceType >( flannInstances,
								 flannCentroids,
								 kmeansParams,
								 dist );

    if ( actualK != m_Params.k ) {
      throw std::logic_error( "Calculated k is not equal to actual k" );
    }


    // The hierarchicalClustering gives us centroids, but not a clustering of
    // instances, so we build an index with the the centroids and cluster
    // instances
    flann::LinearIndexParams centroidsIndexParams;
    flann::LinearIndex< DistanceType > centroidsIndex ( flannCentroids, centroidsIndexParams, dist );
    centroidsIndex.buildIndex();
    
    clustering.clusterMembershipIndices.resize( bags.NumberOfInstances() );
    FlannIndexVectorType flannIndices( clustering.clusterMembershipIndices.data(), bags.NumberOfInstances(), 1 );
    
    std::vector< double > distances( bags.NumberOfInstances() );
    FlannMatrixType flannDistances( distances.data(),
				    distances.size(),
				    1 );
    
    flann::SearchParams searchParams( flann::FLANN_CHECKS_UNLIMITED );
    searchParams.use_heap = flann::FLANN_False;
    searchParams.cores = 0;

    centroidsIndex.knnSearch( flannInstances,
			      flannIndices,
			      flannDistances,
			      1,            // We only want the closest cluster
			      searchParams );

    clustering.clusterBagMap = MatrixType::Zero( bags.NumberOfBags(), m_Params.k );
    coOccurenceMatrix( bags.Indices().cbegin(),
		       bags.Indices().cend(),
		       clustering.clusterMembershipIndices.cbegin(),
		       clustering.clusterMembershipIndices.cend(),
		       clustering.clusterBagMap
		       );
    
    return clustering;
  }


  ParameterType& Parameters() {
    return m_Params;
  }
  
private:
  typedef flann::Matrix< double > FlannMatrixType;
  typedef flann::Matrix< int >    FlannIndexVectorType;
  ParameterType m_Params;
};

#endif
