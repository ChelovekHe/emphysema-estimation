#ifndef __GreedyBinaryClusterLabeller_h
#define __GreedyBinaryClusterLabeller_h

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
*/

#include "ClusterLabellerBase.h"

template< typename TMatrix,
	  typename TVector,
	  template<typename, typename> class TCostFunctor >
class GreedyBinaryClusterLabeller
  : public ClusterLabellerBase< TMatrix, TVector, TCostFunctor >
{
public:
  typedef ClusterLabellerBase< TMatrix, TVector, TCostFunctor > Super;
  typedef typename Super::MatrixType MatrixType;
  typedef typename Super::VectorType VectorType;
  typedef typename Super::CostFunctorType CostFunctorType;

  GreedyBinaryClusterLabeller() {};
  
  ~GreedyBinaryClusterLabeller() {}
    
  /*
    @param p  The known label proportions of bags
    @param C  Mapping from cluster labels to bag labels
    @param x  \in {0,1}^n. Starting point for the optimization
              The optimal labelling is stored in x.

    @return   Objective value at optimal x
   */
  double
  label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const override;

  /*
    @param I  The known interval of label proportions for bags
    @param C  Mapping from cluster labels to bag labels
    @param x  \in {0,1}^n. Starting point for the optimization
              The optimal labelling is stored in x.

    @return   Objective value at optimal x
   */
  double
  label( const MatrixType& I,
	 const MatrixType& C,
	 VectorType& x ) const;
};

#include "GreedyBinaryClusterLabeller.hxx"

#endif
