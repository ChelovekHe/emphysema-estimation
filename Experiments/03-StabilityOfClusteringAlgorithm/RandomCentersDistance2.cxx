/* 
   See README.md for details.
*/

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <numeric>

#include "tclap/CmdLine.h"

#include "Hausdorff.h"
#include "WeightedEarthMoversDistance.h"
#include "DenseHistogram.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("Random centers 2.", ' ', VERSION);

  TCLAP::ValueArg<size_t> 
    histSizeArg("s", 
		"histogram-size", 
		"Number of bins in each histogram",
		true,
		41,
		"size_t", 
		cmd);

  
  try {
    cmd.parse(argc, argv);
  } catch(TCLAP::ArgException &e) {
    std::cerr << "Error : " << e.error() 
	      << " for arg " << e.argId() 
	      << std::endl;
    return EXIT_FAILURE;
  }

  // Store the arguments
  const size_t histSize{ histSizeArg.getValue() };
  //  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;  

  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ 1, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights ); 
  std::random_device rd;
  std::mt19937 gen(rd());

  // We need N samples from M diferent distributions
  const size_t N = 1000;
  const size_t M = 10;
  
  std::vector< float > samples( N * M );
  for ( size_t i = 0; i < M; ++i ) {
    std::normal_distribution<float> dis(i, 1.0);
    for ( size_t n = 0; n < N; ++n ) {
      samples[i*N + n] = dis(gen);
    }
  }
  // Now we have 1000 samples from 10 distributions.
  // We should map them to [0,1] and discretize into histSize bins.
  // Get min/max so we can normalize;
  auto minmax = std::minmax_element(samples.begin(), samples.end());
  auto min = *(minmax.first);
  auto max = *(minmax.second);

  // shift and scale so we get [0,1]
  std::transform( samples.begin(),
		  samples.end(),
		  samples.begin(),
		  [min,max](float x) { return (x - min)/(max - min); }
		  );

  // We want 40 edges distributed evenly on [0,1]
  std::vector< float > edges;
  for ( size_t i = 0; i < histSize; ++i ) {
    edges.push_back( static_cast<float>(i)/static_cast<float>(histSize) );
  }
  
  typedef DenseHistogram< float > HistType;
  std::vector< std::vector<float> > histograms;
  for ( size_t i = 0; i < M; ++i ) {
    HistType hist( edges.begin(), edges.end() );
    for ( size_t n = 0; n < N; ++n ) {
      hist.insert(samples[i*N + n]);
    }
    histograms.push_back(hist.getFrequencies());
  }

  // Now we have the histograms for each distribution
  std::cout << "== Histogram frequencies ==" << std::endl;
  for ( size_t i = 0; i < M; ++i ) {
    for ( auto c : histograms[i] ) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }

  // We want to calculate the EMD between each pair of distributions
  std::cout << "== Histogram distances ==" << std::endl;
  float closestSum = 0;
  for ( size_t i = 0; i < M; ++i ) {
    for ( size_t j = i + 1; j < M; ++j ) {
      auto d = dist(histograms[i].begin(), histograms[j].begin(), histSize );
      if (j == i + 1) {
	closestSum += d;
      }
      std::cout << d << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "mean closest distance: " << closestSum/M << std::endl;

  return EXIT_SUCCESS;
}


  
