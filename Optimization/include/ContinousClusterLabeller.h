#ifndef __ContinousClusterLabeller_h
#define __ContinousClusterLabeller_h

/*
  Find optimal proportional labelling of K clusters of N Bags.
  Let 
     x \in [0,1]^K
     p \in [0,1]^N
     C \in R^{N \times K}, with sum_j C_{i,j} = 1, forall i.

  x is an unkown labelling of clusters we want to find
  p is a known labelling of bags
  C maps cluster labels to bag labels

  We solve this as a box-constrained nonlinear least squares problem with
  regularization.
  Let
     r_i(x) = (p_i - (C*x)_i), for 1 <= i <= N
     r_{N+1} = sum_i p_i - (C*x)_i
     \lambda >= 0

  r_i(x)     is the signed error on bag i
  r_{N+1}(x) is the sum of signed errors over all bags. 
             The purpose of this term is to make the labelling respect the
	     population label proportions.
  \lambda    is a regularization parameter that determines that determines the
             weighting of local/population error. If set to N, it will weigh the
	     terms equally.

  Then we want to solve

     argmin_x 1/2 sum_i r_i(x)^2 + \lambda r_{N+1}(x)^2
     s.t
       0 <= x_j <= 1, for 1 <= j <= K
*/

#include "ClusterLabellerBase.h"
#include "Types.h"

template< template<typename, typename> class TCostFunctor >
class ContinousClusterLabeller
  : public ClusterLabellerBase<
  ee_llp::DoubleRowMajorMatrixType,
  ee_llp::DoubleColumnVectorType,
  TCostFunctor >
{
public:
  typedef ClusterLabellerBase< ee_llp::DoubleRowMajorMatrixType,
			       ee_llp::DoubleColumnVectorType,
			       TCostFunctor > Super;
  
  typedef typename Super::MatrixType MatrixType;
  typedef typename Super::VectorType VectorType;
  typedef typename Super::CostFunctorType CostFunctorType;

  ContinousClusterLabeller() {};
  
  ~ContinousClusterLabeller() {}
    
  /*
    @param p  The known label proportions of bags
    @param C  Mapping from cluster labels to bag labels
    @param x  \in [0,1]^n. Starting point for the optimization
              The optimal labelling is stored in x.

    @return   Objective value at optimal x
   */
  double
  label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const override;
};

#include "ContinousClusterLabeller.hxx"

#endif
