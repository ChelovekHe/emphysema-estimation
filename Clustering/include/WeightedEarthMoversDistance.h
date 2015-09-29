#ifndef __WeightedEarthMoversDistance_h
#define __WeightedEarthMoversDistance_h

#include <vector>

#include "flann.hpp"

/*
  Compute weigthed Earth Movers Distance between samples represented by multiple
  histograms. This allow us to represent an image as a collection of histograms 
  of filter responses, where each of the histograms can we given a weight.

  FLANN does not directly support this kind of hierarchial feature space, but
  we can concatenate the histograms as a single vector and keep track of which
  parts of the vector represents which histogram

 */
template< typename T >
struct WeigthedEarthMoversDistance {

  typedef T ElementType;
  typedef typename flann::Accumulator<T>::Type ResultType;
  typedef std::pair< unsigned int, ResultType > FeatureWeightType;

  WeigthedEarthMoversDistance( std::initializer_list< FeatureWeightType > weights ) : m_Weigths( weights ) {}

  WeigthedEarthMoversDistance( FeatureWeightType* begin, FeatureWeightType* end )
    : m_Weigths( begin, end ) {}
  
  template< typename ForwardIter1, typename ForwardIter2 >
  ResultType operator()( ForwardIter1 a, ForwardIter2 b, size_t size,
			 ResultType /*worst_dist*/= -1) const {

    ResultType result = ResultType();
    size_t i = 0;
    for ( auto weight : m_Weights ) {
      assert( weight.first < size );
      ResultType featureResult = ResultType();
      ResultType emd = ResultType();
      for ( ; i < weight.first; ++i ) {
	emd += *a++ - *b++;
	featureResult += emd;
      }
      result += weight.second * featureResult;
    }
    return result;
  }

private:
  std::vector< FeatureWeightType > m_Weights;
  
};



#endif
