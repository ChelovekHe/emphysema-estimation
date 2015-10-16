/* Run Experiment-1: Scalability.
   See README.md for details.
*/

#include <array>
#include <fstream>

#include "flann/io/hdf5.h"
#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "RuntimeMeasurement.h"
#include "IO.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("Experiment-1:Scalability.", ' ', VERSION);

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
		   "nHistograms", 
		   "Number of histograms",
		   true,
		   1,
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
  const std::string inputPath{ inputArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef KMeansClusterer< DistanceType > ClustererType;
  
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
  flann::Matrix< ElementType > instances(&buffer[0], dim.first, dim.second);
  
  const std::array<size_t, 3> numberOfClusters{ 4, 16, 64 };
  const size_t numberOfBurninIterations{ 10 };
  const size_t numberOfMeasurementIterations{ 100 };
        
  const size_t iterations{ 11 };
  const flann::flann_centers_init_t centers_init{ flann::FLANN_CENTERS_KMEANSPP };
  const int maxK = 64;

  // We have equal sized histograms
  assert( instances.cols % nHistograms == 0 );
  const size_t histSize{ instances.cols/nHistograms };

  std::cout << "nRows " << instances.rows << std::endl
	    << "nCols " << instances.cols << std::endl
	    << "nHistograms " << nHistograms << std::endl
	    << "histSize " << histSize << std::endl;
  
  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights ); 

  std::vector< double > measurements( numberOfMeasurementIterations );
  std::vector< std::pair< double, double > > statistics;
  
  for ( const auto k : numberOfClusters ) {
    int branching = std::min<int>(k, maxK);
    std::cout << "k = " << k << std::endl
	      << "branching = " << branching << std::endl;
    
    ClustererType clusterer( k, instances.cols, branching, iterations, centers_init );
    std::cout << "Burning " << numberOfBurninIterations << " measurements."
	      << std::endl;
    for ( size_t i = 0; i < numberOfBurninIterations; ++i ) {
      try {
	RuntimeMeasurement<>::execution([&clusterer, &instances, &dist]() {
	    return clusterer.cluster(instances, dist);
	  });
      }
      catch ( flann::FLANNException& e ) {
	std::cerr << "Burnin clusterer failed: " << e.what() << std::endl
		  << "k " << k << std::endl
		  << "i " << i << std::endl;
      }
    }
    std::cout << "Measuring " << numberOfMeasurementIterations << " runs."
	      << std::endl;
    for ( size_t i = 0; i < numberOfMeasurementIterations; ++i ) {
      try {
	measurements[i] =
	  static_cast<double>(
			      RuntimeMeasurement<>::execution([&clusterer, &instances, &dist]() {
				  return clusterer.cluster(instances, dist);
				}));
	std::cout << "Measurement " << i << " = " << measurements[i] << std::endl;
      }
      catch ( flann::FLANNException& e ) {
	std::cerr << "Clusterer failed: " << e.what() << std::endl
		  << "k " << k << std::endl
		  << "branching " << branching << std::endl
	  	  << "iterations " << iterations << std::endl
		  << "i " << i << std::endl;
      }
    }
    // Do something with the measurements
    auto meanRunTime =
      std::accumulate( measurements.begin(), measurements.end(), 0.0 )
      / measurements.size();

    auto stddevRunTime =
      std::accumulate( measurements.begin(), measurements.end(), 0.0,
		       [meanRunTime]( double acc, double x ) {
			 double diff = meanRunTime - x;
			 return acc + diff*diff;
		       } ) / (measurements.size() - 1);
    
    statistics.push_back( std::make_pair( meanRunTime, stddevRunTime ) );
  }

  std::ofstream out( outputPath );
  if ( out.good() ) {
    // write header
    out << "k, branching, meanTime, stddevTime" << std::endl;
    for (size_t i = 0;
	 i < statistics.size() && i < numberOfClusters.size();
	 ++i ) {
      out << numberOfClusters[i] << ", "
	  << std::min<int>(numberOfClusters[i], maxK) << ", "
	  << statistics[i].first << ", "
	  << statistics[i].second
	  << std::endl;
    }
  }
  else {
    std::cerr << "Could not write to file " << outputPath << std::endl;
    for (size_t i = 0;
	 i < statistics.size() && i < numberOfClusters.size();
	 ++i ) {
      std::cout << "k = " << numberOfClusters[i] << ". Time = "
		<< statistics[i].first
		<< " (" << statistics[i].second << ")"
		<< std::endl;
    }
  }
  return 0;
}


  
