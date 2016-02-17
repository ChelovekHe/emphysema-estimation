#ifndef __MyL2Dist_h
#define __MyL2Dist_h

#include <vector>

#include "flann/flann.hpp"

/*


 */
struct MyL2Dist {
  typedef double ElementType;
  typedef double ResultType;

  MyL2Dist(const ResultType* w, const int& N) :
    m_Weights(w), m_N( N )
  { }
  
  // The signature is forced by flann
  template< typename ForwardIter1, typename ForwardIter2 >
  ResultType operator()( ForwardIter1 a, ForwardIter2 b, size_t size,
			 ResultType /*worst_dist*/= -1) const {
    ResultType totalDistance = ResultType();
    for ( int i = 0; i < size; ++i ) {
      auto diff = *a++ - *b++;
      totalDistance += m_Weights[i] * diff*diff;
    }
    return totalDistance;
  }

private:
  const ResultType* m_Weights;
  const int m_N;
};

#endif
