#ifndef __ScalarLosses_h
#define __ScalarLosses_h

double L1ScalarLoss( double t, double y ) {
  return t < y ? return y - t : return t - y;
}

double L2ScalarLoss( double t, double y ) {
  double d = t - y;
  return d*d;
}
