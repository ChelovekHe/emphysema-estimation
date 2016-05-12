#ifndef __IntervalLabel_h
#define __IntervalLabel_h

#include <cassert>

/**
   \brief Label indicating interval, e.g. [0.2, 0.4]
 */
template< typename TNumeric >
struct IntervalLabel {
  typedef TNumeric ValueType;

  IntervalLabel( ValueType low, ValueType high )
    : low( low )
    , high( high )
  {
    assert( low <= high );
  }    

  /** 
      \brief Return the absolute distance from x to the interval.
             If x is inside the interval, the distance is 0.
  */
  inline ValueType L1( const ValueType& x ) const {
    if ( x < low ) {
      return low - x;
    }
    else if ( x > high ) {
      return high - x;
    }
    else {
      return 0;
    }
  }

  
  ValueType low, high;
}
  

#endif
