/*
  Test CMSModel
 */

#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>


#include "gtest/gtest.h"

#include "CMSModel.h"
#include "WeightedEarthMoversDistance2.h"
#include "BaggedDataset.h"

class CMSModelTest : public ::testing::Test {
public:
  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  static const size_t InstanceLabelDim = 3;
  static const size_t BagLabelDim = 2;
  typedef BaggedDataset< BagLabelDim, InstanceLabelDim > BaggedDatasetType;
  typedef CMSModel< DistanceFunctorType, BaggedDatasetType > ModelType;
  typedef ModelType::MatrixType MatrixType;
  typedef typename ModelType::LabelVectorType LabelVectorType;
  
protected:
  virtual void SetUp() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> disCentroids(3, 50);
    std::uniform_int_distribution<size_t> disFeatureSize(20, 50);
    std::uniform_int_distribution<size_t> disWeights(8, 25);

    numberOfCentroids = disCentroids( gen );
    featureSize = disFeatureSize( gen );
    numberOfWeights = disWeights( gen );
    dimension = featureSize * numberOfWeights;

    centroids = MatrixType::Random( numberOfCentroids, dimension );
    centroidLabels = LabelVectorType::Random( numberOfCentroids, InstanceLabelDim );
    weights = std::vector< double >( numberOfWeights, 0.5 );
  }

  MatrixType centroids;
  LabelVectorType centroidLabels;
  std::vector< double > weights;
  size_t numberOfCentroids, featureSize, numberOfWeights, dimension;
};


TEST_F( CMSModelTest, TooFewLabels ) {
  auto tooFewLabels = Eigen::Map< MatrixType >( centroidLabels.data(), centroidLabels.rows() - 1, centroidLabels.cols() );
  ASSERT_THROW( ModelType( centroids, tooFewLabels, weights ), std::logic_error );
}

TEST_F( CMSModelTest, Build ) {
  ModelType model( centroids, centroidLabels, weights );
  ASSERT_NO_THROW( model.Build() );
}

TEST_F( CMSModelTest, PredictClustersAsSelf ) {
  BaggedDatasetType bags = BaggedDatasetType::Random( numberOfCentroids, 1, dimension );
  ASSERT_NE( centroidLabels, bags.InstanceLabels() );
  ModelType::Pointer model = ModelType::New( bags.Instances(), centroidLabels, weights );
  model->Predict( bags );
  ASSERT_EQ( centroidLabels, bags.InstanceLabels() );
}

TEST_F( CMSModelTest, LoadSave ) {
  std::string path = "CMSModelTest.LoadSave.model";
  ModelType::Pointer m1 = ModelType::New( centroids, centroidLabels, weights );
  
  std::ofstream os( path );
  m1->Save( os );

  std::ifstream is( path );
  ModelType::Pointer m2 = ModelType::Load( is );

  auto m1w = m1->Weights();
  auto m2w = m2->Weights();
  ASSERT_EQ( m1w.size(), m2w.size() );
  for ( size_t i = 0 ; i < m1w.size(); ++i ) {
    ASSERT_EQ( m1w[i], m2w[i] );
  }

  ASSERT_EQ( m1->Centroids(), m2->Centroids() );
  ASSERT_EQ( m1->Labels(), m2->Labels() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
