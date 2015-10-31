/*
  Test the histogra edges estimation
 */
#include <algorithm>
#include <random>
#include "gtest/gtest.h"

#include "DetermineEdgesForEqualizedHistogram.h"

typedef float RealType;

class EdgesTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10, 10);

    size_t nSamples = 1000;
    samples.resize(nSamples);
    
    std::generate(samples.begin(), samples.end(),
		  [&dis,&gen](){return dis(gen);});
    std::sort(samples.begin(), samples.end());
  } 
  std::vector< RealType > samples;
};


TEST( DetermineEdgesForEqualizedHistogramTest, UniqueEqualizable ) {
  std::vector< RealType > values{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  std::vector< RealType > edges(2);
  determineEdgesForEqualizedHistogram(values.begin(),
				      values.end(),
				      edges.begin(),
				      3);
  ASSERT_EQ(4, edges[0]);
  ASSERT_EQ(7, edges[1]);
}

TEST( DetermineEdgesForEqualizedHistogramTest, AllValuesAreEqual ) {
  std::vector< RealType > values(8, 1);
  std::vector< RealType > edges{0,123};
  determineEdgesForEqualizedHistogram(values.begin(),
				      values.end(),
				      edges.begin(),
				      2);
  ASSERT_EQ(1, edges[0]);
}

TEST( DetermineEdgesForEqualizedHistogramTest, UnevenDistribution ) {
  std::vector< RealType > values{1,1,1,1,1,2,2,3,3,3};
  std::vector< RealType > edges(2);
  determineEdgesForEqualizedHistogram(values.begin(),
				      values.end(),
				      edges.begin(),
				      3);
  ASSERT_EQ(2, edges[0]);
  ASSERT_EQ(3, edges[1]);
}

TEST( DetermineEdgesForEqualizedHistogramTest, TooManyBins ) {
  std::vector< RealType > values{1,2,3,4,5,6,7,8,9};
  std::vector< RealType > edges(9);
  ASSERT_THROW( determineEdgesForEqualizedHistogram(values.begin(),
						    values.end(),
						    edges.begin(),
						    10),
		std::out_of_range );
}


TEST_F( EdgesTest, EdgesAreIncreasing ) {
  std::vector< RealType > edges(49);
  determineEdgesForEqualizedHistogram(samples.begin(),
				      samples.end(),
				      edges.begin(),
				      edges.size() + 1);
  for ( size_t i = 1; i < edges.size(); ++i ) {
    ASSERT_LT(edges[i-1], edges[i]);
  }
}

TEST_F( EdgesTest, BinsAreEqualSize ) {
  std::vector< RealType > edges(49);
  determineEdgesForEqualizedHistogram(samples.begin(),
				      samples.end(),
				      edges.begin(),
				      edges.size() + 1);
  size_t binSize = samples.size() / (edges.size() + 1);
  size_t n = 0;
  for ( size_t i = 0, j = 0; i < samples.size(); ++i, ++n ) {
    if ( j < edges.size() && samples[i] >= edges[j] ) {
      EXPECT_EQ( binSize, n );
      n = 0;
      ++j;
    }
  }
  // Check the last bin
  EXPECT_EQ( binSize, n );
}




int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
