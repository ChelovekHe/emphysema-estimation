#ifndef __IntervalLosses_h
#define __IntervalLosses_h

#include <cmath>

struct L1_IntervalLoss {
  double operator()( double low, double high, double y ) {
    if ( y < low ) {
      return low - y;
    }
    if ( y > high ) {
      return y - high;
    }
    return 0;
  }
};

#endif
