/* 
   Predict instances with a CMS model
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "BagProportionError2.h"
#include "ClusterModel.h"
#include "WeightedEarthMoversDistance2.h"

#include "IO.h"
#include "Types.h"


const std::string LLP_VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("05-LLP", ' ', LLP_VERSION);

  // We need a path to some data
  TCLAP::ValueArg<std::string> 
    instancesArg("i", 
	     "instances", 
	     "Path to instance matrix stored as csv file",
	     true,
	     "",
	     "path", 
	     cmd);
  

  TCLAP::ValueArg<std::string> 
    modelArg("M", 
	     "model", 
	     "Path to model.",
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

  TCLAP::ValueArg<std::string> 
    outputArg("o", 
	      "output", 
	      "Base path for output files",
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
  const std::string instancesPath{ instancesArg.getValue() };
  const std::string modelPath{ modelArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType VectorType;
  
  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef ClusterModel< DistanceFunctorType > ModelType;
  
  typedef int LabelType;
  typedef double ElementType;
  
  // Parse the data into a matrix
  std::ifstream is( instancesPath );
  if ( !is.good() ) {
    std::cerr << "Error reading instances." << std::endl
	      << "instancesPath: " << instancesPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< ElementType > buffer;
  char colSep = ',', rowSep = '\n';
  auto dataDim =
    readTextMatrix<ElementType>( is, std::back_inserter(buffer), colSep, rowSep );
  MatrixType instances(dataDim.first, dataDim.second);
  std::copy( buffer.cbegin(), buffer.cend(), instances.data() );

  // Verify that we have equal sized histograms
  if ( instances.cols() % nHistograms != 0 ) {
    std::cerr << "Histograms must have equal size" << std::endl
	      << "instances.cols() % nHistograms = "
	      << instances.cols() % nHistograms << std::endl;
    return EXIT_FAILURE;
  }
  const size_t nBins = instances.cols()/nHistograms;
  
  ModelType model;
  std::ifstream modelIs( modelPath );
  if ( modelIs >> model ) {
    VectorType predictions = model.predictInstances( instances );
    std::ofstream out( outputPath );
    out << predictions << std::endl;
  }
  else {
    std::cerr << "Failed to load model" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
