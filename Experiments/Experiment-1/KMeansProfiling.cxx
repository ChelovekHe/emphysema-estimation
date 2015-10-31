/* Run Experiment-1: KMeans profiling.
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
  std::copy(buffer.begin(), buffer.end(), instances.data());
  
  const size_t k = 32;
  const int branching = 32;
  const size_t iterations{ 11 };
  const flann::flann_centers_init_t centers_init{ flann::FLANN_CENTERS_KMEANSPP };
  const int maxK = 64;

  // We have equal sized histograms
  assert( instances.cols() % nHistograms == 0 );
  const size_t histSize{ instances.cols()/nHistograms };
  
  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType > weights{ nHistograms, std::make_pair(histSize, 1.0) };
      
  DistanceType dist( weights );     
  ClustererType clusterer( k, instances.cols(), branching, iterations, centers_init );
  clusterer.cluster(instances, dist);

  return 0;
}


  
