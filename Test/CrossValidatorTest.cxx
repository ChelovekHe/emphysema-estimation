/*
  Test the CrossValidator
 */

#include <algorithm>
#include <random>
#include <iostream>

#include "gtest/gtest.h"

#include "Types.h"
#include "CrossValidator.h"


struct DummyModel {
public:
  typedef double LossResultType;
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType ColVectorType;
  typedef ee_llp::DoubleRowVectorType RowVectorType;

  DummyModel() :
    m_V() {}
  
  ColVectorType
  predict( const MatrixType& m ) const {
    ColVectorType res(m.rows());
    for ( std::size_t i = 0; i < m.rows(); ++i ) {
      res(i) = (m_V - m.row(i)).sum();
    }
    return res;
  }

  LossResultType
  loss( const MatrixType& m ) const {
    return predict( m ).sum();
  }
  
  MatrixType m_V;
};


class DummyTrainer {
public:
  typedef std::size_t ParamsType;
  typedef DummyModel ModelType;
  typedef typename ModelType::MatrixType MatrixType;
  typedef typename ModelType::LossResultType LossResultType;

  DummyTrainer( const MatrixType& D, const unsigned int& ) :
    m_D(D) {}

  LossResultType
  train( ModelType& m, const ParamsType& p ) {
    m.m_V = m_D.row(p);
    return m.loss( m_D );
  }

  MatrixType m_D;
};


struct DummyTester {
public:
  typedef DummyModel ModelType;
  typedef typename ModelType::MatrixType MatrixType;
  typedef typename ModelType::LossResultType LossResultType;

  DummyTester( const MatrixType& D ) :
    m_D(D) {}

  LossResultType
  test( const ModelType& m ) {
    return m.loss( m_D );
  }

  MatrixType m_D;
};



class CrossValidatorTest : public ::testing::Test {
public:

  typedef CrossValidator< DummyTrainer, DummyTester > ValidatorType;
  typedef typename ValidatorType::ResultType ValidatorResultType;
  typedef typename ValidatorType::MatrixType MatrixType;
  
protected:
  virtual void SetUp() {
    d = MatrixType::Random( 2, 5 );
  }

  MatrixType d;
};

/*
  C is the identiy and we have one parameter for each outcome.
  We should get the x = p.
*/
TEST_F( CrossValidatorTest, NoShuffle ) {
  CrossValidationParams cvParams( CrossValidationType::K_FOLD,
				  2,
				  false );
  std::size_t p = 0;
  ValidatorType cv;
  auto res = cv.run( d, 1, p, cvParams );

  // The DummyTrainer sets the model to the first row of the train data
  // We have one row of train data in each of the two folds.
  // In the first fold it is the second row of d, in the second fold it
  // is the first row of d.
  for ( std::size_t j = 0; j < d.cols(); ++j ) {
    ASSERT_FLOAT_EQ( d(1,j), res.models[0].m_V(j) );
    ASSERT_FLOAT_EQ( d(0,j), res.models[1].m_V(j) );
  }
 
  ASSERT_FLOAT_EQ( 0, res.trainingLosses[0] );
  ASSERT_FLOAT_EQ( 0, res.trainingLosses[1] );

  // The test loss is sum(train - test)
  ASSERT_FLOAT_EQ( (d.row(1) - d.row(0)).sum(), res.testLosses[0] );
  ASSERT_FLOAT_EQ( (d.row(0) - d.row(1)).sum(), res.testLosses[1] );
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
