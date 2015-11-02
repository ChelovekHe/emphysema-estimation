/* Run Experiment-1: Stability-1.
   See README.md for details.
*/

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <numeric>

#include "Eigen/Dense"
#include "tclap/CmdLine.h"

#include "Hausdorff.h"
#include "WeightedEarthMoversDistance.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("Experiment-1:Stability.", ' ', VERSION);

  TCLAP::ValueArg<size_t> 
    nHistogramsArg("n", 
		   "nHistograms", 
		   "Number of histograms",
		   true,
		   1,
		   "size_t", 
		   cmd);

  TCLAP::ValueArg<size_t> 
    histSizeArg("s", 
		"histogram-size", 
		"Number of bins in each histogram",
		true,
		2,
		"size_t", 
		cmd);

  
  TCLAP::ValueArg<std::string> 
    outputArg("o", 
	      "output", 
	      "Path to output file to write results to",
	      true,
	      "",
	      "path", 
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
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const size_t histSize{ histSizeArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef Eigen::Matrix< ElementType, Eigen::Dynamic, Eigen::Dynamic >
    MatrixType;
  
  const std::vector<size_t> numberOfClusters{2, 4, 8, 16, 32, 64 };
  const size_t numberOfIterations = 100;

  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights ); 

  //
  std::ofstream out( outputPath );
  if ( out.good() ) {
    // write header
    out << "k, distance" << std::endl;
  }
  else {
    std::cerr << "Could not write to file " << outputPath << std::endl;
    return EXIT_FAILURE;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1);
  
  for ( const auto k : numberOfClusters ) {
    std::cout << "Creating cluster centers:" << std::endl
	      << "k = " << k << std::endl;
    std::vector< MatrixType > allCenters;
    for ( size_t i = 0; i < numberOfIterations; ++i ) {
      // For each histogram we draw random numbers from (0,1) and normalize
      
      MatrixType M{k, nHistograms*histSize};
      for ( size_t i = 0; i < k; ++i ) {
	// Generate the i'th center
	size_t rowOffset = i*M.cols();
	for ( size_t j = 0; j < nHistograms; ++j ) {
	  auto begin = M.data() + rowOffset +     j * histSize;
	  auto end =   M.data() + rowOffset + (j+1) * histSize;
	  std::generate(begin, end, [&dis,&gen](){return dis(gen);});
	  DistanceType::ResultType s = std::accumulate(begin, end, 0.0);
	  std::transform(begin, end, begin,
	   		 [s](DistanceType::ResultType x){ return x/s; });
	}
      }
      allCenters.push_back( M );
    }
    
    // Now we have a std::vector of centers matrices. We want to calculate the
    // Hausdorff distance between each pair of matrices.
    std::cout << "Calculating distances" << std::endl;
    std::vector< DistanceType::ResultType > distances;
    for ( size_t i = 0; i < allCenters.size(); ++i ) {
      auto A = allCenters[i];
      for ( size_t j = i + 1; j < allCenters.size(); ++j ) {
	auto B = allCenters[j];
	auto distance = hausdorff( A.data(), A.data() + A.size(),
				   B.data(), B.data() + B.size(),
				   A.cols(), dist );
	out << A.rows() << ", "
	    << distance << '\n';
	if ( !out.good() ) {
	  std::cerr << "Could not write to file " << outputPath << std::endl;
	  return EXIT_FAILURE;
	}
      }
    }
  }

  return EXIT_SUCCESS;
}


  
