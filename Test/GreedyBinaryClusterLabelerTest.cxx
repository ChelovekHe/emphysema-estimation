/*
  Test CMSTrainer
 */

#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>


#include "gtest/gtest.h"

#include "IntervalLosses.h"
#include "IntervalRisk.h"
#include "GreedyBinaryClusterLabeler.h"



class CMSTrainerTest : public ::testing::Test {
public:
  typedef IntervalRisk< L1_IntervalLoss > Risk;
  typedef GreedyBinaryClusterLabeler< Risk, 2 > Labeler;
  typedef Labeler::MatrixType MatrixType;
  typedef Labeler::BagLabelVectorType BagLabelVectorType;
  typedef Labeler::ClusterLabelVectorType ClusterLabelVectorType;
  
protected:
  virtual void SetUp() {

    // Setup a bagged dataset
    
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<size_t> disCentroids(3, 50);
    // std::uniform_int_distribution<size_t> disFeatureSize(20, 50);
    // std::uniform_int_distribution<size_t> disWeights(8, 25);

    // numberOfCentroids = disCentroids( gen );
    // featureSize = disFeatureSize( gen );
    // numberOfWeights = disWeights( gen );
    // dimension = featureSize * numberOfWeights;

    // centroids = MatrixType::Random( numberOfCentroids, dimension );
    // centroidLabels = LabelVectorType::Random( numberOfCentroids, InstanceLabelDim );
    // weights = std::vector< double >( numberOfWeights, 0.5 );
  }

  // MatrixType centroids;
  // LabelVectorType centroidLabels;
  // std::vector< double > weights;
  // size_t numberOfCentroids, featureSize, numberOfWeights, dimension;
};


TEST_F( CMSTrainerTest, NoBags ) {
  FAIL() << "TODO: Define behaviour when no bags are given" ;
}

TEST_F( CMSTrainerTest, BagsAreInstances_OneClusterPerBag ) {
  FAIL() <<  "TODO: Write test" ;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
