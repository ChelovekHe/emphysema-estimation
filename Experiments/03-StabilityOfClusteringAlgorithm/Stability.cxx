/* 03-StabilityOfClusteringAlgorithm
   See README.md for details.
*/

#include <fstream>

#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "Hausdorff.h"
#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "IO.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("03-StabilityOfClusteringAlgorithm", ' ', VERSION);

  // We need a path to some data
  TCLAP::ValueArg<std::string> 
    inputArg("i", 
	     "input", 
	     "Path to input matrix stored as csv file",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<size_t> 
    nHistogramsArg("n", 
		   "histograms", 
		   "Number of histograms",
		   true,
		   1,
		   "size_t", 
		   cmd);

  TCLAP::ValueArg<size_t> 
    nIterationsArg("N", 
		   "iterations", 
		   "Number of iterations",
		   true,
		   2,
		   "size_t", 
		   cmd);

  TCLAP::ValueArg<int> 
    branchingArg("b", 
		 "branching", 
		 "Branching parameter to pass to flann",
		 true,
		 2,
		 ">=2", 
		 cmd);

  
  TCLAP::MultiArg<size_t> 
    nClustersArg("k", 
		 "clusters", 
		 "Number of clusters",
		 true,
		 ">=2", 
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
  const std::string inputPath{ inputArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const size_t nIterations{ nIterationsArg.getValue() };
  const int branching{ branchingArg.getValue() };
  const std::vector<size_t> nClusters{ nClustersArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef KMeansClusterer< DistanceType > ClustererType;
  typedef typename ClustererType::MatrixType MatrixType;
  
  // Parse the data into a matrix
  std::ifstream is( inputPath );
  if ( !is.good() ) {
    std::cerr << "Error reading input." << std::endl
	      << "inputPath: " << inputPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< ElementType > buffer;
  char colSep = ',', rowSep = '\n';
  auto dim =
    readTextMatrix<ElementType>( is, std::back_inserter(buffer), colSep, rowSep );
  MatrixType instances(dim.first, dim.second);
  std::copy( buffer.cbegin(), buffer.cend(), instances.data() );
  
  // Fixed flann internal parameters
  const size_t iterations = 11;
  const flann::flann_centers_init_t centers_init{ flann::FLANN_CENTERS_KMEANSPP };

  // We have equal sized histograms
  assert( instances.cols() % nHistograms == 0 );
  const size_t nBins{ instances.cols()/nHistograms };

  std::cout << "nRows " << instances.rows() << std::endl
	    << "nCols " << instances.cols() << std::endl
	    << "nHistograms " << nHistograms << std::endl
	    << "nBins " << nBins << std::endl;
  
  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(nBins, 1.0) };
      
  DistanceType dist( weights ); 

  // Prepare the outfile
  std::ofstream out( outputPath );
  if ( out.good() ) {
    // write header
    out << "k, branching, distance" << std::endl;
  }
  else {
    std::cerr << "Could not write to file " << outputPath << std::endl;
    return EXIT_FAILURE;
  }
  ClustererType clusterer( branching, iterations, centers_init );
  for ( const auto k : nClusters ) {
    std::cout << "Finding cluster centers:" << std::endl
	      << "branching " << branching << std::endl
	      << "k = " << k << std::endl;
    std::vector< MatrixType > allCenters;
    for ( size_t i = 0; i < nIterations; ++i ) {
      try {
	allCenters.push_back( clusterer.cluster(instances, dist, k).centers );
      }
      catch ( flann::FLANNException& e ) {
	std::cerr << "Clusterer failed: " << e.what() << std::endl
		  << "k " << k << std::endl
		  << "i " << i << std::endl;
      }
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
	    << branching << ", "
	    << distance << std::endl; // Flush in case of later errors
	if ( !out.good() ) {
	  std::cerr << "Error writing to file " << outputPath << std::endl;
	  return EXIT_FAILURE;
	}
      }
    }
  }

  return EXIT_SUCCESS;
}
