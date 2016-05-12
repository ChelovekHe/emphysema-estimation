/*
  Test CMSTrainer
 */

#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>


#include "gtest/gtest.h"

#include "CMSTrainer.h"
#include "WeightedEarthMoversDistance2.h"
#include "BaggedDataset.h"
#include "KMeansWeightedDistanceInstanceClusterer.h"
#include "GreedyBinaryClusterLabeler.h"



class CMSTrainerTest : public ::testing::Test {
public:
  // typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  // static const size_t InstanceLabelDim = 3;
  // static const size_t BagLabelDim = 2;
  // typedef BaggedDataset< BagLabelDim, InstanceLabelDim > BaggedDatasetType;
  // typedef CMSModel< DistanceFunctorType, BaggedDatasetType > ModelType;
  // typedef ModelType::MatrixType MatrixType;
  // typedef typename ModelType::LabelVectorType LabelVectorType;
  
protected:
  virtual void SetUp() {
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
