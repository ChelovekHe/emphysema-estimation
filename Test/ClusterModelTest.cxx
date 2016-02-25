/*
  Test ClusterModel
 */

#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>


#include "gtest/gtest.h"

#include "ClusterModel.h"
#include "WeightedEarthMoversDistance2.h"


class ClusterModelTest : public ::testing::Test {
public:
  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef ClusterModel< DistanceFunctorType > ModelType;
  
protected:
  virtual void SetUp() {
    std::string clusterModelPath = "Data/ClusterModelSimple.in";
    std::ifstream clusterModelIs( clusterModelPath );
    clusterModelIs >> simpleModel;
  }
  
    ModelType simpleModel;
};

/*
*/
TEST_F( ClusterModelTest, IO ) {
  std::string path = "Data/ClusterModelSimple.out";
  std::ofstream os( path );
  os << simpleModel;

  ASSERT_TRUE( false );
};


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
