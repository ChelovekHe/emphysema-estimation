/* Run Experiment-1: Scalability.
   See README.md for details.
*/

#include <array>

#include "flann/io/hdf5.h"
#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "RuntimeMeasurement.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("Experiment-1:Scalability.", ' ', VERSION);

  // We need a path to some data
  TCLAP::ValueArg<std::string> 
    inputArg("i", 
	     "input", 
	     "Path to input data stored as an hdf5 file",
	     true,
	     "",
	     "path", 
	     cmd);

  // We need the name of the hdf dataset
  TCLAP::ValueArg<std::string> 
    datasetArg("d", 
	       "dataset", 
	       "Name of dataset in input",
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
  const std::string datasetPath{ datasetArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  
  //// Commandline parsing is done ////
  
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef KMeansClusterer< DistanceType > ClustererType;
  
  // Parse the data into a matrix
  flann::Matrix< ElementType > instances;
  flann::load_from_file( instances, inputPath, datasetPath );
  
  const std::array<size_t, 3> numberOfClusters{ 4, 16, 64 };
  const size_t numberOfBurninIterations{ 10 };
  const size_t numberOfMeasurementIterations{ 100 };
        
  const size_t iterations{ 11 };
  const flann::flann_centers_init_t centers_init{ flann::FLANN_CENTERS_KMEANSPP };

  // We have equal sized histograms
  assert( instances.cols % nHistograms == 0 );
  const size_t histSize{ instances.cols/nHistograms };

  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights ); 

  std::vector< double > measurements{ numberOfMeasurementIterations };
  std::vector< std::pair< double, double > > statistics;
  
  for ( const auto k : numberOfClusters ) {
    ClustererType clusterer( k, nHistograms, k, iterations, centers_init );
    for ( size_t i = 0; i < numberOfBurninIterations; ++i ) {
      RuntimeMeasurement<>::execution([&clusterer, &instances, &dist]() {
	  return clusterer.cluster(instances, dist);
	});
    }
    for ( size_t i = 0; i < numberOfMeasurementIterations; ++i ) {
      measurements[i] +=
	static_cast<double>(
	  RuntimeMeasurement<>::execution([&clusterer, &instances, &dist]() {
	      return clusterer.cluster(instances, dist);
	    }));
    }
    // Do something with the measurements
    auto meanRunTime =
      std::accumulate( measurements.begin(), measurements.end(), 0.0 )
      / measurements.size();

    auto stddevRunTime =
      std::accumulate( measurements.begin(), measurements.end(), 0.0,
		       [meanRunTime]( double acc, double x ) {
			 double diff = meanRunTime - x;
			 return acc + x*x;
		       } );
    statistics.push_back( std::make_pair( meanRunTime, stddevRunTime ) );
  }

  for ( auto stat : statistics ) {
    std::cout << stat.first << " (" << stat.second << ")" << std::endl;
  }

  delete[] instances.ptr();
  
  return 0;
}


  
