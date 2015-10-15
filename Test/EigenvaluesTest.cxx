/*
  Tests of the eigenvalue computation.
  We need the eigenvalues to be returned in decreasing magnitude.
  The expected values are either derived analytically or with 
  numpy.linalg.eig
  using python 3.4.2 and numpy 1.8.2
 */
#include "gtest/gtest.h"

#include "itkFixedArray.h"
#include "Eigenvalues.h"

typedef float RealType;
typedef itk::FixedArray<RealType, 3> Vector3Type;
typedef itk::FixedArray<RealType, 6> SymmetricMatrix3x3Type;

// Test eigenvalues for equality using gtests float equality
Vector3Type
__TestEigenvalues( std::vector<RealType> mBuf,
		   std::vector<RealType> vBuf ) {
  SymmetricMatrix3x3Type matrix(&mBuf[0]);
  Vector3Type expected(&vBuf[0]);
  Vector3Type actual = eigenvalues_symmetric3x3(matrix);
  EXPECT_FLOAT_EQ(expected[0], actual[0]);
  EXPECT_FLOAT_EQ(expected[1], actual[1]);
  EXPECT_FLOAT_EQ(expected[2], actual[2]);
}

// Test eigenvalues for equality using a delta value
Vector3Type
__TestEigenvaluesNear( std::vector<RealType> mBuf,
		       std::vector<RealType> vBuf,
		       RealType epsilon ) {
  SymmetricMatrix3x3Type matrix(&mBuf[0]);
  Vector3Type expected(&vBuf[0]);
  Vector3Type actual = eigenvalues_symmetric3x3(matrix);
  EXPECT_NEAR(expected[0], actual[0], epsilon);
  EXPECT_NEAR(expected[1], actual[1], epsilon);
  EXPECT_NEAR(expected[2], actual[2], epsilon);
}




TEST ( EigenvaluesTest, Identity ) {
  std::vector< RealType > mBuf{1,0,0,1,0,1};
  std::vector< RealType > vBuf{1,1,1};
  __TestEigenvalues( mBuf, vBuf );
}

TEST ( EigenvaluesTest, DiagonalPos ) {
  std::vector< RealType > mBuf{1,0,0,2,0,3};
  std::vector< RealType > vBuf{3,2,1};
  __TestEigenvalues( mBuf, vBuf );
}

TEST ( EigenvaluesTest, DiagonalNeg ) {
  std::vector< RealType > mBuf{-1,0,0,-2,0,-3};
  std::vector< RealType > vBuf{-3,-2,-1};
  __TestEigenvalues( mBuf, vBuf );
}

TEST ( EigenvaluesTest, DiagonalPosNeg ) {
  std::vector< RealType > mBuf{1,0,0,-2,0,3};
  std::vector< RealType > vBuf{3,-2,1};
  __TestEigenvalues( mBuf, vBuf );
}

TEST ( EigenvaluesTest, Ones ) {
  std::vector< RealType > mBuf{1,1,1,1,1,1};
  // It should be 3,0,0 but numerics dont want it that way, so we just check
  // that we are close enough to zero.
  std::vector< RealType > vBuf{3,0,0};
  __TestEigenvaluesNear( mBuf, vBuf, 1e-15 );
}

TEST ( EigenvaluesTest, RandomsSmallNums ) {
  std::vector< RealType > mBuf{0.27, 0.92, 0.58, 0.24, 0.75, 0.04};
  std::vector< RealType > vBuf{1.70680634,-0.7205504,-0.43625594};
  __TestEigenvalues( mBuf, vBuf );
}

TEST ( EigenvaluesTest, RandomsBigNums ) {
  std::vector< RealType > mBuf{599,860,-835,-941,817,-207};
  std::vector< RealType > vBuf{-2005.21004566,1183.41690727,272.79313839};
  __TestEigenvalues( mBuf, vBuf );
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
