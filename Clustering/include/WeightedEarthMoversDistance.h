#ifndef __WeightedEarthMoversDistance_h
#define __WeightedEarthMoversDistance_h

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
template< typename T >
struct WeightedEarthMoversDistance {

  typedef T ElementType;
  typedef typename flann::Accumulator<T>::Type ResultType;
  typedef std::pair< unsigned int, ResultType > FeatureWeightType;

  WeightedEarthMoversDistance( std::initializer_list< FeatureWeightType > weights ) : m_Weights( weights ) {}

  WeightedEarthMoversDistance( std::vector<FeatureWeightType> weights )
    : m_Weights( weights ) {}
  
  template< typename ForwardIter1, typename ForwardIter2 >
  ResultType operator()( ForwardIter1 a, ForwardIter2 b, size_t size,
			 ResultType /*worst_dist*/= -1) const {

    ResultType result = ResultType();
    size_t i = 0;
    size_t j = 0;
    for ( auto weight : m_Weights ) {
      j += weight.first;
      assert( j <= size );
      ResultType featureResult = ResultType();
      ResultType emd = ResultType();
      for ( ; i < j; ++i ) {
	emd += *a++ - *b++;
	featureResult += std::abs(emd);
      }
      result += weight.second * featureResult;
    }
    return result;
  }

private:
  std::vector< FeatureWeightType > m_Weights;
  
};



#endif
