/*
  Test the Weighted EMD calculation
 */

#include <algorithm>
#include <random>
#include <iostream>

#include "gtest/gtest.h"

#include "WeightedEarthMoversDistance.h"

typedef float RealType;
typedef WeightedEarthMoversDistance<float> DistanceType;
typedef DistanceType::FeatureWeightType FeatureWeightType;

class WeightedEarthMoversDistanceTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> sizeDis(2, 20);
    std::uniform_real_distribution<> dis(0, 1);
    std::uniform_real_distribution<> weightDis(0, 1);

    // We have random number of histograms
    size_t numHistograms = sizeDis(gen);
    size_t totalBins = 0;
    for ( size_t i = 0; i < numHistograms; ++i ) {
      // Each histogram has a random number of bins
      auto nBins = sizeDis(gen);
      // And it gets a random weight
      auto weight = weightDis(gen);
      W.push_back( std::make_pair(nBins, weight) );
      totalBins += nBins;
    }

    A.resize(totalBins);
    B.resize(totalBins);
    C.resize(totalBins);
    
    std::generate(A.begin(), A.end(), [&dis,&gen](){return dis(gen);});
    std::generate(B.begin(), B.end(), [&dis,&gen](){return dis(gen);});
    std::generate(C.begin(), C.end(), [&dis,&gen](){return dis(gen);});

    // Normalize the histograms in A, B and C
    size_t i = 0;
    for ( auto w : W ) {
      RealType a=0, b=0, c=0;
      size_t j = i;
      for ( ; i < w.first; ++i ) {
	a += A[i];
	b += B[i];
	c += C[i];
      }
      for ( ; j < w.first; ++j ) {
	A[j] /= a;
	B[j] /= b;
	C[j] /= c;
      }      
    }
  }
  
  std::vector< RealType > A, B, C;
  std::vector< FeatureWeightType > W;
};

/* Test that the WEMD distance is a proper distance. It must hold that
   d(x,y) >= 0
   d(x,y) == 0 <==> x == y
   d(x,y) == d(y,x)
   d(x,y) + d(y,z) >= d(x,z)
*/

TEST_F( WeightedEarthMoversDistanceTest, DistanceToSelfIsZero ) {
  // We must have that d(x,x) = 0
  WeightedEarthMoversDistance<RealType> dist{W};
  ASSERT_EQ(0, dist(A.begin(), A.begin(), A.size()) ) ;
};

TEST_F( WeightedEarthMoversDistanceTest, DistanceToOtherIsPositive ) {
  // We must have that d(x,y) > 0 when x != y
  // Theoretically this can fail if we sample values so A[i] == B[i] for all i,
  // but if that is the case we need to take a good hard look at the random
  // number generator and verify that it is not completely broken.
  WeightedEarthMoversDistance<RealType> dist{W};
  ASSERT_LT(0, dist(A.begin(), B.begin(), A.size()) );
};

TEST_F( WeightedEarthMoversDistanceTest, Symmetry ) {
  // We must have that d(x,y) = d(y,x);
  WeightedEarthMoversDistance<RealType> dist{W};
  auto distAB = dist( A.begin(), B.begin(), A.size() );
  auto distBA = dist( B.begin(), A.begin(), B.size() );
  ASSERT_EQ(distAB, distBA);
}

TEST_F( WeightedEarthMoversDistanceTest, TriangleInequality ) {
  // We must have that d(x,y) = d(y,x);
  WeightedEarthMoversDistance<RealType> dist{W};
  auto distAB = dist( A.begin(), B.begin(), A.size() );
  auto distBC = dist( B.begin(), C.begin(), B.size() );
  auto distAC = dist( A.begin(), C.begin(), A.size() );
ASSERT_LE(distAC, distAB + distBC);
}


/*
  Test that we get the expected results
 */
TEST( Deterministic_WeightedEarthMoversDistanceTest, Scalar ) {
  // This does not make a lot of sense, since we assume that the histograms have
  // equal mass. But it is a good sanity check.
  std::vector<RealType> A{1};
  std::vector<RealType> B{2};
  std::vector< FeatureWeightType > W{1, std::make_pair(1,1) };
  WeightedEarthMoversDistance<RealType> dist{W};

  EXPECT_EQ(1, dist( A.begin(), B.begin(), A.size() ) );
}

TEST( Deterministic_WeightedEarthMoversDistanceTest, SingleHistogram ) {
  // The distance should be
  //   |1/3 - 1/4|          (= |1/12|)
  // + |1/12 + 1/3 - 1/2|   (= |-1/12|)
  // + |-1/12 + 1/3 - 1/4|  (= |0|)
  // = 1/6
  std::vector<RealType> A{1.0/3,1.0/3,1.0/3};
  std::vector<RealType> B{1.0/4,1.0/2,1.0/4};
  std::vector< FeatureWeightType > W{1, std::make_pair(3,1) };
  WeightedEarthMoversDistance<RealType> dist{W};

  EXPECT_FLOAT_EQ(1.0/6, dist( A.begin(), B.begin(), A.size() ) );
}

TEST( Deterministic_WeightedEarthMoversDistanceTest, ManyHistograms ) {
  // The distance should be
  //   |1/3 - 1/4|          (= |1/12|)
  // + |1/12 + 1/3 - 1/2|   (= |-1/12|)
  // + |-1/12 + 1/3 - 1/4|  (= |0|)
  // + |1/3 - 1/2|          (= |-1/6|)
  // + |-1/6 + 2/3 - 1/2|   (= |0|)
  // = 1/3
  //
  std::vector<RealType> A{1.0/3,1.0/3,1.0/3, 1.0/3,2.0/3};
  std::vector<RealType> B{1.0/4,1.0/2,1.0/4, 1.0/2,1.0/2};
  std::vector< FeatureWeightType >
    W{ std::make_pair(3,1), std::make_pair(2,1) };
  WeightedEarthMoversDistance<RealType> dist{W};

  EXPECT_FLOAT_EQ(1.0/3, dist( A.begin(), B.begin(), A.size() ) );
}


TEST( Deterministic_WeightedEarthMoversDistanceTest, DifferentWeights ) {
  // The distance should be
  //   2*(|1/3 - 1/4|           (= |1/12|)
  // +    |1/12 + 1/3 - 1/2|    (= |-1/12|)
  // +    |-1/12 + 1/3 - 1/4|)  (= |0|)
  // + 4*(|1/3 - 1/2|           (= |-1/6|)
  // +    |-1/6 + 2/3 - 1/2|)   (= |0|)
  // = 1
  //
  std::vector<RealType> A{1.0/3,1.0/3,1.0/3, 1.0/3,2.0/3};
  std::vector<RealType> B{1.0/4,1.0/2,1.0/4, 1.0/2,1.0/2};
  std::vector< FeatureWeightType >
    W{ std::make_pair(3,2), std::make_pair(2,4) };
  WeightedEarthMoversDistance<RealType> dist{W};

  EXPECT_FLOAT_EQ(1.0, dist( A.begin(), B.begin(), A.size() ) );
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
