#ifndef __GreedyBinaryClusterLabeller_hxx
#define __GreedyBinaryClusterLabeller_hxx

#include "GreedyBinaryClusterLabeller.h"
#include "BagProportionError.h"
#include <unordered_set>

template< typename TMatrix,
	  typename TVector,
	  template<typename, typename> class TCostFunctor >
double
GreedyBinaryClusterLabeller< TMatrix, TVector, TCostFunctor >
::label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const {  
  // We start with all zeros. Then we find best labelling using a single
  // ones cluster. We continue like that untill we cannot label more
  // clusters with ones without increasing the error,
  // We must have at least one cluster with a one label
  const std::size_t N = x.size();

  // We keep track of which clusters are labelled zero
  std::unordered_set< std::size_t > zeroIdxs;
  for ( std::size_t i = 0; i < N; ++i ) {
    zeroIdxs.insert(i);
  }

  CostFunctorType costFunction;
  VectorType bestX = VectorType::Zero( N );
  double bestError = std::numeric_limits<double>::infinity();

  for ( std::size_t i = 1; i <= N; ++i ) {
    // We continue from the currently best labelling
    x = bestX;

    // Keep track of whether or not we improved the error
    bool improved = false;

    // Keep track of an iterator to the cluster that gave largest
    // improvement when labelled as one
    auto bestIdx = zeroIdxs.begin();
    
    for ( auto it = zeroIdxs.begin(); it != zeroIdxs.end(); ++it ) {
      // Try setting the label of cluster *it to one
      x(*it) = 1;
      double error =  costFunction(p, C*x);
      if ( error < bestError ) {
	improved = true;
	bestError = error;
	bestX = x;
	bestIdx = it;
      }
      // Reset the cluster label to zero so we can try the next
      x(*it) = 0;
    }

    // If we did not improve we are done
    if ( !improved ) {
      break;
    }

    // If we did improve, then erase the cluster that gave best
    // improvement from the set of zero-labelled clusters.
    zeroIdxs.erase( bestIdx );
  }
  return bestError;
}

#endif
