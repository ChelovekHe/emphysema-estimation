/* 
   Test a CMS model
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
    bagMembershipArg("b", 
		     "bag-membership", 
		     "Path to bag membership vector stored as csv file. Should have one bag id for each instance.",
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
  const std::string bagMembershipPath{ bagMembershipArg.getValue() };
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
  

  // Parse the mebership into an array
  std::ifstream bagMembershipIs( bagMembershipPath );
  if ( !bagMembershipIs.good() ) {
    std::cerr << "Error reading bag membership." << std::endl
	      << "bagMembershipPath: " << bagMembershipPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< LabelType > bagMembership;
  readTextSequence< LabelType >( bagMembershipIs,
				 std::back_inserter(bagMembership),
				 rowSep );

  // Verify our assumptions about the bag membership and data
  if ( bagMembership.size() != instances.rows() ) {
    std::cerr << "There must be exactly one bag membership for each instance"
	      << std::endl
	      << "bagMembership.size() = " << bagMembership.size() << std::endl
	      << "instances.rows() = " << instances.rows() << std::endl;
    return EXIT_FAILURE;
  }  

  std::vector< LabelType > bagMembershipCopy( bagMembership );
  std::sort( bagMembershipCopy.begin(), bagMembershipCopy.end() );
  auto last = std::unique( bagMembershipCopy.begin(), bagMembershipCopy.end() );
  bagMembershipCopy.erase( last, bagMembershipCopy.end() );
  std::size_t numberOfBags = bagMembershipCopy.size();
  if ( bagMembershipCopy.front() != 0 || bagMembershipCopy.back() != numberOfBags - 1 ) {
    std::cerr << "Bag membership must belong to the range [0," << numberOfBags - 1 << ")" << std::endl;
    return EXIT_FAILURE;
  }
  

  ModelType model;
  std::ifstream modelIs( modelPath );
  if ( modelIs >> model ) {
    VectorType instancePredictions = model.predictInstances( instances );
    VectorType bagPredictions = model.predictBags( instances, bagMembership, numberOfBags );

    std::string bagPredictionsOutputPath = outputPath + "bag_predictions.txt";
    std::ofstream bagPredictionsOut( bagPredictionsOutputPath );
    bagPredictionsOut << "prediction" << std::endl;
    for ( std::size_t i = 0; i < bagPredictions.size(); ++i ) {
      bagPredictionsOut << bagPredictions(i) << std::endl;
    }

    std::string instancePredictionsOutputPath = outputPath + "instance_predictions.txt";
    std::ofstream instancePredictionsOut( instancePredictionsOutputPath );
    instancePredictionsOut << "prediction" << std::endl;
    for ( std::size_t i = 0; i < instancePredictions.size(); ++i ) {
      instancePredictionsOut << instancePredictions(i) << std::endl;
    }
  }
  else {
    std::cerr << "Failed to load model" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
