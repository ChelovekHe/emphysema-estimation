/*
  Test IntervalLosses
 */

#include <random>
#include "gtest/gtest.h"

#include "IntervalLosses.h"

struct Interval {
  double a, b;
  Interval( double a=0, double b=0 ) : a(a), b(b) {}
};


class IntervalLossesTest : public ::testing::Test {
public:
  
protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    double lowa = -10.0;
    double lowb = 10.0;
    double higha = 10.0;
    double highb = 100.0;
    double a = lowa - 10;
    double b = highb + 10;
    size_t nIntervals = 1000;
    
    std::uniform_real_distribution< double > low( lowa, lowb );
    std::uniform_real_distribution< double > high( higha, highb );
    std::uniform_real_distribution< double > candidates( a, b );

    intervals.resize( nIntervals );
    testPoints.resize( nIntervals );
    for ( size_t i = 0; i < nIntervals; ++i ) {
      intervals[i] = Interval( low( gen ), high( gen ) );
      testPoints[i] = candidates( gen );
    }
  }

  std::vector< Interval > intervals;
  std::vector< double > testPoints;
};


TEST_F( IntervalLossesTest, L1OnlyZeroInsideInterval ) {
  L1_IntervalLoss L1;
  for ( std::size_t i = 0; i < intervals.size(); ++i ) {
    auto a = intervals[i].a;
    auto b = intervals[i].b;
    auto x = testPoints[i];
    auto L = L1(a,b,x);
    if ( x >= a && x <= b ) {
      ASSERT_EQ( L, 0 );
    }
    else {
      ASSERT_NE( L, 0 );
    }
  }
}

TEST_F( IntervalLossesTest, L1AlwaysNonNegative ) {
  L1_IntervalLoss L1;
  for ( std::size_t i = 0; i < intervals.size(); ++i ) {
    auto a = intervals[i].a;
    auto b = intervals[i].b;
    auto x = testPoints[i];
    auto L = L1(a,b,x);
    ASSERT_GE( L, 0 );
  }
}

TEST_F( IntervalLossesTest, L1 ) {
  L1_IntervalLoss L1;
  for ( std::size_t i = 0; i < intervals.size(); ++i ) {
    auto a = intervals[i].a;
    auto b = intervals[i].b;
    auto x = testPoints[i];
    auto L = L1(a,b,x);
    if ( x < a ) {
      ASSERT_EQ( L, a - x );
    }
    else if ( x > b ) {
      ASSERT_EQ( L, x - b );
    }
    else {
      ASSERT_EQ( L, 0 );
    }
  }
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
