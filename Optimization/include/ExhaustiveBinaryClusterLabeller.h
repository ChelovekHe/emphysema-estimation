#ifndef __ExhaustiveBinaryClusterLabeller_h
#define __ExhaustiveBinaryClusterLabeller_h

/*
  Find optimal binary labelling of K clusters of N Bags using exhaustive
  search strategy.
  Let 
     x \in {0,1}^K
     p \in [0,1]^N
     C \in R^{N \times K}, with sum_j C_{i,j} = 1, forall i.

  x is an unkown labelling of clusters we want to find
  p is a known labelling of bags
  C maps cluster labels to bag labels

  We solve this by trying all possible labellings. This has runtime O(2^K)
  so should only be used when K is small.

  For larger K, look at "GreedyBinaryClusterLabeller.h" or 
  "ContinousClusterLabeller.h"
*/
#include "ClusterLabellerBase.h"


template< typename TMatrix,
	  typename TVector,
	  template<typename, typename> class TCostFunctor >
class ExhaustiveBinaryClusterLabeller
  : public ClusterLabellerBase< TMatrix, TVector, TCostFunctor >
{
public:
  typedef ClusterLabellerBase< TMatrix, TVector, TCostFunctor > Super;
  typedef typename Super::MatrixType MatrixType;
  typedef typename Super::VectorType VectorType;
  typedef typename Super::CostFunctorType CostFunctorType;

  ExhaustiveBinaryClusterLabeller() {};
  
  ~ExhaustiveBinaryClusterLabeller() {}
    
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
};

#include "ExhaustiveBinaryClusterLabeller.hxx"

#endif
