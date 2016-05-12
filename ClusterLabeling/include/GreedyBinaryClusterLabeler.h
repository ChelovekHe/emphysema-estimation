#ifndef __GreedyBinaryClusterLabeller_h
#define __GreedyBinaryClusterLabeller_h

#include <unordered_set>

#include "GreedyBinaryClusterLabelerParameters.h"
#include "BaggedDataset.h"

/*
  Find optimal binary labelling of K clusters of N Bags using a greedy 
  strategy.
  Let 
     x \in {0,1}^K
     p \in [0,1]^N
     C \in R^{N \times K}, with sum_j C_{i,j} = 1, forall i.

  x is an unkown labelling of clusters we want to find
  p is a known labelling of bags
  C maps cluster labels to bag labels

  We solve this using a greedy search. First we find the best labelling
  where only one cluster is labelled wit 1. If this is better than the
  labelling where all clusters are zero, we try to label another cluster
  with 1. This is continued untill labelling a new cluster with 1, does 
  not decrease the error.

  TRisk must define
    double operator()( const BagLabelVectorType& bagLabels, const ClusterLabelVectorType& clusterLabels)
  which calculates the risk when assigning clusterLabels given bagLabels

*/
template< typename TRisk, size_t BagLabelDim=1 >
class GreedyBinaryClusterLabeler
{
public:
  typedef TRisk RiskType;
  typedef GreedyBinaryClusterLabeler< RiskType > Self;

  typedef BaggedDataset< BagLabelDim, 1 > BaggedDatasetType;
  
  typedef typename BaggedDatasetType::MatrixType MatrixType;
  typedef typename BaggedDatasetType::BagLabelVectorType BagLabelVectorType;
  typedef typename BaggedDatasetType::InstanceLabelVectorType ClusterLabelVectorType;
  

  typedef GreedyBinaryClusterLabelerParameters ParameterType;
  
  GreedyBinaryClusterLabeler( ParameterType& params )
    : m_Params( params )
  {}
  
  ~GreedyBinaryClusterLabeler() {}
    
  /*
    @param bags           Set of bags including their known labels
    @param clusterBagMap  Mapping from cluster labels to bag labels
    @param labeling       Final labeling

    @return   Objective value at best cluster labeling
   */
  double Label( const BaggedDatasetType& bags,
		const MatrixType& clusterBagMap,
		ClusterLabelVectorType& labeling ) {
    // We start with all zeros. Then we find best labeling using a single
    // one cluster. We continue like that untill we cannot label more
    // clusters with one without increasing the error.
    const std::size_t K = labeling.rows();

    // We keep track of which clusters are labelled zero
    std::unordered_set< std::size_t > zeroIdxs;
    for ( std::size_t i = 0; i < K; ++i ) {
      zeroIdxs.insert(i);
    }

    RiskType risk;
    ClusterLabelVectorType bestLabeling = ClusterLabelVectorType::Zero( K );
    double bestRisk = risk( bags.BagLabels(), clusterBagMap * bestLabeling );

    for ( std::size_t i = 1; i <= K; ++i ) {
      labeling = bestLabeling;
      bool improved = false;
      auto bestIdx = zeroIdxs.begin();
    
      for ( auto it = zeroIdxs.begin(); it != zeroIdxs.end(); ++it ) {
	labeling(*it) = 1;
	double thisRisk = risk( bags.BagLabels(), clusterBagMap * labeling);
	if ( thisRisk < bestRisk ) {
	  improved = true;
	  bestRisk = thisRisk;
	  bestLabeling = labeling;
	  bestIdx = it;
	}
	// Reset the cluster label to zero so we can try the next
	labeling(*it) = 0;
      }
      
      if ( !improved ) {
	break;
      }     
      zeroIdxs.erase( bestIdx );
    }
    return bestRisk;
  }


 private:
  ParameterType m_Params;
  
};

#endif
