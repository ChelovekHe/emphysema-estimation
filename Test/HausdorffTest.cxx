/*
  Test the Hausdorff distance calculation
 */

#include <algorithm>
#include <random>
#include <iostream>

#include "gtest/gtest.h"

#include "Hausdorff.h"

typedef float RealType;

struct L2 {
  typedef RealType ResultType;

  template<typename InputIter>
  ResultType operator()( InputIter A, InputIter B, size_t n ) {
    ResultType dist = 0;
    for (size_t i = 0; i < n; ++i) {
      ResultType diff = *A++ - *B++;
      dist += diff*diff;
    }
    return std::sqrt(dist);
  }
    
};

class HaussdorffTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDisA(2, 20);
    std::uniform_real_distribution<> dis(-10, 10);

    rows = sizeDisA(gen);
    cols = sizeDisA(gen);

    A.resize(rows*cols);
    B.resize(rows*cols);
    C.resize(rows*cols);
    
    std::generate(A.begin(), A.end(), [&dis,&gen](){return dis(gen);});
    std::generate(B.begin(), B.end(), [&dis,&gen](){return dis(gen);});
    std::generate(C.begin(), C.end(), [&dis,&gen](){return dis(gen);});
  } 
  std::vector< RealType > A, B, C;
  size_t rows, cols;
};

/* Test that the Hausdorff distance is a proper distance. It must hold that
   d(x,y) >= 0
   d(x,y) == 0 <==> x == y
   d(x,y) == d(y,x)
   d(x,y) + d(y,z) >= d(x,z)
*/

TEST_F( HaussdorffTest, DistanceToSelfIsZero ) {
  // We must have that d(x,x) = 0
  ASSERT_EQ(0, hausdorff(A.begin(), A.end(), A.begin(), A.end(), cols, L2()) );
};

TEST_F( HaussdorffTest, DistanceIsNonNegative ) {
  // We must have that d(x,y) >= 0
  ASSERT_LE(0, hausdorff(A.begin(), A.end(), B.begin(), B.end(), cols, L2()) );
};

TEST_F( HaussdorffTest, DistanceToOtherIsPositive ) {
  // We must have that d(x,y) > 0 when x != y
  // Theoretically this can fail if we sample values so A[i] == B[i] for all i,
  // but if that is the case we need to take a good hard look at the random
  // number generator and verify that it is not completely broken.
  EXPECT_LT(0, hausdorff(A.begin(), A.end(), B.begin(), B.end(), cols, L2()) );
};

TEST_F( HaussdorffTest, Symmetry ) {
  // We must have that d(x,y) = d(y,x);
  auto distAB = hausdorff(A.begin(), A.end(), B.begin(), B.end(), cols, L2());
  auto distBA = hausdorff(B.begin(), B.end(), A.begin(), A.end(), cols, L2());
  ASSERT_EQ(distAB, distBA);
}

TEST_F( HaussdorffTest, TriangleInequality ) {
  // We must have that d(x,z) <= d(x,y) + d(y,z);
  auto distAB = hausdorff(A.begin(), A.end(), B.begin(), B.end(), cols, L2());
  auto distBC = hausdorff(B.begin(), B.end(), C.begin(), C.end(), cols, L2());
  auto distAC = hausdorff(A.begin(), A.end(), C.begin(), C.end(), cols, L2());
  ASSERT_LE(distAC, distAB + distBC);
}


/*
  Test that we get the expected results
 */
TEST_F( HaussdorffTest, Scalar ) {
  std::vector<RealType> A{1};
  std::vector<RealType> B{2};

  EXPECT_EQ(1, hausdorff(A.begin(), A.end(), B.begin(), B.end(), 1, L2()));
}

TEST_F( HaussdorffTest, RowVector ) {
  // This is equivalent to the normal vector distance
  // sqrt(1 + 1 + 4) == 2.449489743
  std::vector<RealType> A{1,2,3,0};
  std::vector<RealType> B{2,2,2,2};

  EXPECT_FLOAT_EQ(2.449489743,
		  hausdorff(A.begin(), A.end(), B.begin(), B.end(), 4, L2()));
}

TEST_F( HaussdorffTest, ColVector ) {
  // This is equivalent to the largest componentwise distance
  // sqrt(4) == 2
  std::vector<RealType> A{1,2,3,0};
  std::vector<RealType> B{2,2,2,2};

  EXPECT_EQ(2, hausdorff(A.begin(), A.end(), B.begin(), B.end(), 1, L2()));
}

TEST_F( HaussdorffTest, MatrixRowVector ) {
  // It should be the largest of the distances from each row of A to the single
  // row of B. This will be the distance from (7,8,9) to (1,1,1) which is
  // sqrt(6^2 + 7^2 + 8^2) ==  12.206555616
  std::vector<RealType> A{1,2,3, 4,5,6, 7,8,9};
  std::vector<RealType> B{1,1,1};

  EXPECT_FLOAT_EQ(12.206555616, 
		  hausdorff(A.begin(), A.end(), B.begin(), B.end(), 3, L2()));
}

TEST_F( HaussdorffTest, PermutedColumns ) {
  // It should matter which order the columns are given in
  // In this case we should have that distance from any row in A to any row in B
  // is sqrt(2^2 + 1^2 + 1^2) == 2.449489743
  std::vector<RealType> A{1,2,3, 4,5,6, 7,8,9};
  std::vector<RealType> B{3,1,2, 6,4,5, 9,7,8};

  EXPECT_FLOAT_EQ(2.449489743,
		  hausdorff(A.begin(), A.end(), B.begin(), B.end(), 3, L2()));
}


TEST_F( HaussdorffTest, PermutedRows ) {
  // It should not matter which order the rows are given in
  std::vector<RealType> A{1,2,3, 4,5,6, 7,8,9};
  std::vector<RealType> B{4,5,6, 7,8,9, 1,2,3};

  EXPECT_FLOAT_EQ(0, 
		  hausdorff(A.begin(), A.end(), B.begin(), B.end(), 3, L2()));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
