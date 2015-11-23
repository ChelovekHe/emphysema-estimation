#include <iostream>

#include "KMeansClusterer2.h"
#include "LLPCostFunction.h"
#include "BagProportionError.h"
#include "ContinousClusterLabeller.h"
#include "ExhaustiveBinaryClusterLabeller.h"
#include "GreedyBinaryClusterLabeller.h"
#include "ClusterModelTrainer.h"
#include "WeightedEarthMoversDistance2.h"


int main() {
  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef KMeansClusterer2< DistanceFunctorType > ClustererType;
  typedef ContinousClusterLabeller< LLPCostFunction > LabellerType;

  typedef ClusterModelTrainer< ClustererType,
			       LabellerType > TrainerType;
  

  std::cout << "Types defined" << std::endl;
}
