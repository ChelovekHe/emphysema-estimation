#ifndef __ExhaustiveBinaryClusterLabeller_hxx
#define __ExhaustiveBinaryClusterLabeller_hxx

#include "ExhaustiveBinaryClusterLabeller.h"
#include "BagProportionError.h"

template< typename TMatrix,
	  typename TVector,
	  template<typename, typename> class TCostFunctor >
double
ExhaustiveBinaryClusterLabeller< TMatrix, TVector, TCostFunctor >
::label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const {
  CostFunctorType costFunction;
  VectorType bestX = x;
  double bestError = std::numeric_limits<double>::infinity();
  
  // We try every possible labelling, starting from the one with least
  // 1 labels
  const std::size_t N = x.size();
  for ( std::size_t i = 0; i <= N; ++i ) {
    // Create a sequence of N-i zeros folowed by i ones
    if ( i == 0 ) {
      x = VectorType::Zero( N );
    }
    else if ( i == N ) {
      x = VectorType::Ones( N );
    }
    else {
      x.segment(0, N-i) = VectorType::Zero( N-i );
      x.segment(N-i, i) = VectorType::Zero( i );
    }
    do {
      VectorType y = C*x;
      double error =  costFunction(p, y);
      if ( error < bestError ) {
	bestError = error;
	bestX = x;
      }
    } while ( std::next_permutation( x.data(), x.data() + N ) );
  }
  x = bestX;
  return bestError;
}

#endif
