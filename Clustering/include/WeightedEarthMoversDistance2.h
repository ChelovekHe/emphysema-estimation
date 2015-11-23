#ifndef __WeightedEarthMoversDistance2_h
#define __WeightedEarthMoversDistance2_h

#include <vector>

#include "flann/flann.hpp"

/*
  Compute weighted Earth Movers Distance between samples represented by multiple
  histograms. This allow us to represent an image as a collection of histograms 
  of filter responses, where each of the histograms can we given a weight.

  FLANN does not directly support this kind of hierarchial feature space, but
  we can concatenate the histograms as a single vector and keep track of which
  parts of the vector represents which histogram

 */
struct WeightedEarthMoversDistance2 {
  typedef double ElementType;
  typedef double ResultType;

  WeightedEarthMoversDistance2(const ResultType* w, const int& N) :
    m_Weights(w), m_N( N )
  { }
  
  // The signature is forced by flann
  template< typename ForwardIter1, typename ForwardIter2 >
  ResultType operator()( ForwardIter1 a, ForwardIter2 b, size_t size,
			 ResultType /*worst_dist*/= -1) const {

    const std::size_t bins = size / m_N;
    ResultType totalDistance = ResultType();
    for ( int i = 0; i < m_N; ++i ) {
      ResultType histogramDistance = ResultType();
      ResultType accumulatedDifference = ResultType();
      for ( std::size_t j = 0; j < bins; ++j ) {
	accumulatedDifference += *a++ - *b++;
	histogramDistance += std::abs( accumulatedDifference );
      }
      totalDistance += m_Weights[i] * histogramDistance;
    }
    return totalDistance;
  }

private:
  const ResultType* m_Weights;
  const int m_N;
};

#endif
