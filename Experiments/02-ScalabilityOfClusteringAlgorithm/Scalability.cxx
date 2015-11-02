/* Run Experiment-1: Scalability.
   See README.md for details.
*/

#include <array>
#include <fstream>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "RuntimeMeasurement.h"
#include "IO.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("Scalability.", ' ', VERSION);

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

  TCLAP::MultiArg<size_t> 
    clustersArg("k", 
		"clusters", 
		"Number of clusters",
		true,
		">= 2", 
		cmd);

  TCLAP::ValueArg<size_t> 
    burninArg("b", 
	      "burnin", 
	      "Number of iterations to burn before measuring",
	      false,
	      10,
	      ">= 0", 
	      cmd);

  TCLAP::ValueArg<size_t> 
    iterationsArg("I", 
		  "iterations", 
		  "Number of measurement iterations",
		  false,
		  100,
		  ">= 0", 
		  cmd);

  TCLAP::ValueArg<int> 
    branchingArg("r", 
		  "branching", 
		  "Branching parameter for kmeans algorithm. Set to 1 to use branching = clusters.",
		  false,
		  1,
		  ">= 1", 
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
  const std::vector<size_t> clusters( clustersArg.getValue() );
  const size_t burnin( burninArg.getValue() );
  const size_t iterations( iterationsArg.getValue() );
  int branchingOpt( branchingArg.getValue() );  
  //// Commandline parsing is done ////

  // Fixed parameters
  const size_t kmeans_iterations{ 11 };
  const flann::flann_centers_init_t centers_init{ flann::FLANN_CENTERS_KMEANSPP };

  
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
  std::copy(buffer.begin(), buffer.end(), instances.data());
  

  // We have equal sized histograms
  assert( instances.cols() % nHistograms == 0 );
  const size_t histSize{ instances.cols()/nHistograms };
  
  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights ); 

  std::vector< double > measurements( iterations );
  std::vector< std::pair< double, double > > statistics;
  
  for ( const auto k : clusters ) {
    int branching = branchingOpt == 1 ? k : branchingOpt;

    std::cout << "k = " << k << std::endl
	      << "branching = " << branching << std::endl;
    
    ClustererType clusterer( k, instances.cols(), branching, kmeans_iterations, centers_init );

    std::cout << "Burning " << burnin << " measurements."  << std::endl;    
    for ( size_t i = 0; i < burnin; ++i ) {
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
    
    std::cout << "Measuring " << iterations << " runs."  << std::endl;
    for ( size_t i = 0; i < iterations; ++i ) {
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
	  	  << "iterations " << kmeans_iterations << std::endl
		  << "i " << i << std::endl;
      }
    }

    if ( measurements.size() > 0 ) {
      // Do something with the measurements
      auto meanRunTime =
	std::accumulate( measurements.begin(), measurements.end(), 0.0 ) / measurements.size();
      
      auto stddevRunTime = measurements.size() > 1
	? std::sqrt( std::accumulate( measurements.begin(), measurements.end(), 0.0,
				      [meanRunTime]( double acc, double x ) {
					double diff = meanRunTime - x;
					return acc + diff*diff;
				      } ) / (measurements.size() - 1)
		     )
	: 0;
    
      statistics.push_back( std::make_pair( meanRunTime, stddevRunTime ) );
    }
  }

  if ( statistics.size() > 0 ) {
    std::ofstream out( outputPath );
    if ( out.good() ) {
      // write header
      out << "numSamples, numHistograms, histSize, k, branching, meanTime, stddevTime" << std::endl;
      for (size_t i = 0;
	   i < statistics.size() && i < clusters.size();
	   ++i ) {
	out << instances.rows() << ", "
	    << nHistograms << ", "
	    << histSize << ", "
	    << clusters[i] << ", "
	    << (branchingOpt == 1 ? clusters[i] : branchingOpt) << ", "
	    << statistics[i].first << ", "
	    << statistics[i].second
	    << std::endl;
      }
    }
    else {
      std::cerr << "Could not write to file " << outputPath << std::endl;
      for (size_t i = 0;
	   i < statistics.size() && i < clusters.size();
	   ++i ) {
	std::cerr << instances.rows() << ", "
		  << nHistograms << ", "
		  << histSize << ", "
		  << clusters[i] << ", "
		  << (branchingOpt == 1 ? clusters[i] : branchingOpt) << ", "
		  << statistics[i].first << ", "
		  << statistics[i].second
		  << std::endl;
      }
    }
  }
  return 0;
}


  
