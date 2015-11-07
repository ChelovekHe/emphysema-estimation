/*
  Test the ClusterLabeller
 */

#include <algorithm>
#include <random>
#include <iostream>

#include "gtest/gtest.h"

#include "Eigen/Dense"

#include "ClusterLabeller.h"


class ClusterLabellerTest : public ::testing::Test {
public:
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixType;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorType;
  typedef ClusterLabeller<MatrixType, VectorType> LabellerType;
  
protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDis(2, 20);
    std::uniform_real_distribution<> dis(0, 1);
    
    rows = sizeDis(gen);
    cols = sizeDis(gen);

    p = VectorType::Zero(rows);
    for ( std::size_t i = 0; i < rows; ++i ) {
      p(i) = dis(gen);
    }

    // The precision we require in order to call values equal.
    precision = 1e-4;
  } 
  VectorType p;
  std::size_t rows, cols;
  double precision;
};

/*
  C is the identiy and we have one parameter for each outcome.
  We should get the x = p.
*/
TEST_F( ClusterLabellerTest, Identity ) {
  MatrixType C = MatrixType::Identity(rows,rows);
  VectorType x = VectorType::Zero(rows);
  LabellerType labeller(p);
  labeller.findOptimalLabels(C, x);
  
  for ( std::size_t i = 0; i < rows; ++i ) {
    ASSERT_NEAR( p(i), x(i), precision );
  }
};

/*
  C(i,j) = 1/rows for all i,j. This implies that for y = C*x, we have 
  y(i) = y(j) for all i,j.
  In this case we should get C*x = E[p]
*/
TEST_F( ClusterLabellerTest, EqualDistribution ) {
  MatrixType C = MatrixType::Zero(rows,rows);
  for ( std::size_t i = 0; i < rows; ++i ) {
    for ( std::size_t j = 0; j < rows; ++j ) {
      C(i,j) = 1.0/rows;
    }
  }
  
  VectorType x = VectorType::Zero(rows);
  LabellerType labeller(p);
  labeller.findOptimalLabels(C, x);

  VectorType y = C*x;

  double Ep = p.mean();
  
  for ( std::size_t i = 0; i < rows; ++i ) {
    ASSERT_NEAR( Ep, y(i), precision );
  }
};


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
