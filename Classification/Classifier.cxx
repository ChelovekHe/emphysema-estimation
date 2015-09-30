/*
  Genetic algorithm wrapper around
    Cluster
    Label
 */

#include <iostream>
#include <vector>

#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "ClusterLabeller.h"

int main() {


  std::string inPath( inPathArg.getValue() );

  // Genetic wrapper args
  unsigned int nOuterIterations( nOuterIterationsArg.getValue() );
  unsigned int featureWeightPopulationSize( featureWeightPopulationSizeArg.getValue() );
  float mutationVariance( mutationVarianceArg.getValue() );
  float crossoverPropability( crossoverPropabilityArg.getValue() );
  unsigned int tournamentSize( tournamentSizeArg.getValue() );
  
  // kMeans args
  unsigned int nClusters( nClustersArg.getValue() );

  
  // We need to know
  // * Features
  // * Number of outer iterations
  // * Size of feature weight population
  // * Parameters for evolving the feature weight population
  // * kMeans parameters
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef flann::Matrix< ElementType > MatrixType;

  typedef typename Distance::FeatureWeightType FeatureWeightType;
  
  // We use binary labels
  typedef bool ClusterLabelType;


  // Read the instance vector, the bag partition and the label proportions
  // All instances are concatenated in a large instance vector
  // The bags are given as an array of indices into the instance vector
  // that indicates when a new bag begins. The label proportions is given as
  // a fraction of positive instances in each bag.
  flann::Matrix< ElementType > instances;
  std::vector< size_t > bags;
  std::vector< ElementType > labelProportions;
  

  // K-Means parameters
  size_t nClusters = 32;
  int branching = 32;
  int iterations = 11;
  flann_centers_init_t centers_init = FLANN_CENTERS_RANDOM;
  float cb_index = 0.2;
  KMeansIndexParams params( branching, iterations, centers_init, cb_index );

  std::vector< ElementType > centersBuffer( nClusters * features.cols );
  flann::Matrix< ElementType > centers( &centersBuffer[0],
					nClusters,
					features.cols );

  // Each cluster is assigned a binary label
  std::vector< bool > labels( nClusters );

  // Setup the feature weight vectors
  std::vector< ElementType >
    featureWeightVectorsBuffer( weightPopulationSize * nFeatures );
  flann::Matrix< ElementType >
    featureWeightVectors( &featureWeightVectorsBuffer[0],
			  weightPopulationSize,
			  nFeatures );
  std::vector< ElementType > featureWeightErrors( featureWeightVectors.size() );

  // Setup the clusterer
  typedef KMeansClusterer< DistanceType > ClustererType;
  ClustererType clusterer( nClusters, nInstances, nClusters );
  
  for ( unsigned int i = 0; i < nOuterIterations; ++i ) {
    // What should we do here?
    for ( unsigned int j = 0; j < featureWeightVectors.rows; ++i ) {
      DistanceType dist( featureWeightVectors[i], featureWeightVectors.cols );
      MatrixType centers = clusterer.cluster( instances, dist );
      
      // auto error = labelling( centers, clusterLabels, instances, bags, labelProportions );
      featureWeightErrors[j] = error;
      if ( error < bestError ) {
	bestError = error;
	bestCenters = centers;
	bestClusterLabels = clusterLabels;
      }      
    }
  }

      
  

  return EXIT_SUCCESS;
}
