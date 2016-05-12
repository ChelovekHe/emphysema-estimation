/*
  Test IntervalRisk
 */


#include "gtest/gtest.h"

#include "IntervalRisk.h"
#include "IntervalLosses.h"

class IntervalRiskTest : public ::testing::Test {
public:
  typedef IntervalRisk< L1_IntervalLoss > Risk;
  typedef Risk::KnownLabelVectorType IntervalMatrix;
  typedef Risk::PredictedLabelVectorType PointMatrix;
  
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

    intervals = IntervalMatrix::Zero( nIntervals, 2 );
    points = PointMatrix::Zero( nIntervals );
    for ( size_t i = 0; i < nIntervals; ++i ) {
      intervals(i,0) = low( gen );
      intervals(i,1) = high( gen );
      points(i) = candidates( gen );
    }
  }

  IntervalMatrix intervals;
  PointMatrix points;
};

TEST_F( IntervalRiskTest, AlwaysNonNegative ) {
  Risk risk;
  ASSERT_GE( risk(intervals, points), 0 );
}

TEST_F( IntervalRiskTest, ZeroInsideInterval ) {
  Risk risk;
  ASSERT_EQ( 0, risk( intervals, intervals.col(0) ) );
  ASSERT_EQ( 0, risk( intervals, intervals.col(1) ) );
}


TEST_F( IntervalRiskTest, L1 ) {
  Risk risk;
  L1_IntervalLoss L1;
  double sum = 0;
  for ( std::size_t i = 0; i < intervals.rows(); ++i ) {
    auto a = intervals(i,0);
    auto b = intervals(i,1);
    auto x = points(i);
    sum += L1(a,b,x);
  }
  ASSERT_DOUBLE_EQ( sum/intervals.rows(), risk(intervals, points)  );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
