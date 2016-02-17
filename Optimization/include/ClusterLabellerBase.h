#ifndef __ClusterLabellerBase_h
#define __ClusterLabellerBase_h

/*
  Base class classes the implement a cluster labelling scheme.
  Clusters should be labelled such that they minimize some error function
  on the labelling and a known bag label proportion.

*/

template< typename TMatrix,
	  typename TVector,
	  template< typename, typename > class TCostFunctor >
class ClusterLabellerBase {
public:
  typedef TMatrix MatrixType;
  typedef TVector VectorType;
  typedef TCostFunctor< VectorType, MatrixType > CostFunctorType;  
    
  /*
    @param p  The known label proportions of bags
    @param C  Mapping from cluster labels to bag labels
    @param x  Starting point for the optimization.
              The optimal labelling is stored in x.

    @return   Objective value at optimal x
   */
  virtual
  double
  label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const = 0;
};

#endif
