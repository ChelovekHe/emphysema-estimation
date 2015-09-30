#ifndef __Labeller_h
#define __Labeller_h

#include <algorithm>
#include <cstring>
#include <numeric>
#include <random>
#include <vector>

#include "BagLoss.h"

template< typename MatrixType, typename ClusterLabelType >
class ClusterLabeller {
  typedef typename MatrixType::type ElementType;
  
  ClusterLabeller(){}

  /*
    Let k be the number of clusters, h the number of bags and l the number of 
    labels.
    clusterBagRatio is an h x k matrix where entry i,j gives the fraction of 
    instances in bag i that belongs to cluster j. The rows sum to 1.

    clusterLabels is a vector of labels assigned to each cluster.
       
    modelProportions is an h x l matrix where entry i,j gives the fraction
    of instances in bag i that has label j under the model given by the cluster
    labels
  */
  void
  calculateModelProportions( const MatrixType& clusterBagRatio,
			     std::vector< ClusterLabelType > clusterLabels,
			     MatrixType& modelProportions ) {
    const size_t h = modelProportions.rows;
    const size_t l = modelProportions.cols;
    const size_t k = clusterBagRatio.cols;
    std::memset( modelProportions.ptr(), 0, h * l * sizeof(ElementType) );

    // It is not given which kind of iteration is faster.
    // One pass that don't respect row major order or several passes that do
    // If performance is a problem we should look into it
    for ( size_t i = 0; i < h; ++i ) {
      for ( size_t j = 0; j < k; ++j ) {
	modelProportions[i][clusterLabels[j]] += clusterBagRatio[i][j];
      }
    }
  }
  
  /*
    Let k be the number of clusters, h the number of bags and l the number of 
    labels.
    clusterBagRatio is an h x k matrix where entry i,j gives the fraction of 
    instances in bag i that belongs to cluster j. The rows sum to 1.
       
    bagProportions is an h x l matrix where entry i,j gives the known fraction
    of instances in bag i that has label j

    relativeBagSizes is an h x 1 matrix where entry i,1 is the relative size of
    bag i. The column sum to 1.

   */
  std::pair< ElementType, std::vector< ClusterLabelType > >
  greedyLabelling( const MatrixType& clusterBagRatio,
		   const MatrixType& bagProportions,
		   const MatrixType& relativeBagSizes ) {

    const size_t nBags = bagProportions.rows;
    const size_t nLabels = bagProportions.cols;
    const size_t nClusters = clusterBagRatio.cols;
    
    // Create the matrix that holds the bag proportions under the current model
    std::vector< ElementType > modelProportionsBuffer( nBags*nLabels );
    MatrixType modelProportions( &modelProportionsBuffer[0], nBags, nLabels );

    // Initially assign label 0 too all clusters
    std::vector< size_t > clusterLabels( nClusters );
    auto bestClusterLabels = clusterLabels;
    
    // Calculate model proportions under initial model
    calculateModelProportions( clusterBagRatio,
			       clusterLabels,
			       modelProportions );

    // Calculate the initial error when all cluster have label 0
    ElementType bestError = bagLoss( bagProportions,
				     modelProportions, 
				     relativeBagSizes );

    // We greedily assign clusters labels by picking the label that minimizes
    // the loss when all clusters other than the current one is kept fixed.
    // To keep us from staying in the same small area of label space, we
    // randomize the order of labelling.
    // If necesary we can do this several times and keep the best labelling.
    std::vector< size_t > clusterIndices( nClusters );
    std::iota( clusterIndices.begin(), clusterIndices.end(), 0 );
    std::random_device rd;
    std::mt19937 g(rd());    
    std::shuffle( clusterIndices.begin(), clusterIndices.end(), g);

    for ( size_t i : clusterIndices ) {
      for ( size_t j = 1; j < nLabels; ++j ) {
	clusterLabels[i] = j;
	calculateModelProportions( clusterBagRatio,
				   clusterLabels,
				   modelProportions );
	ElementType error = bagLoss( bagProportions,
				     modelProportions, 
				     relativeBagSizes );
	if ( error < bestError ) {
	  bestError = error;
	  bestClusterLabels[i] = j;
	}
	else {
	  clusterLabels[i] = bestClusterLabels[i];
	}
      }
    }
    return { bestError, bestClusterLabels };
  }


  
};


#endif
