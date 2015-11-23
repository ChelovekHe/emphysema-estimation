/* 05-LLP
   See README.md for details.
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "tclap/CmdLine.h"

#include "KMeansClusterer2.h"
#include "LLPCostFunction.h"
#include "BagProportionError.h"
#include "ContinousClusterLabeller.h"

#include "ClusterModel.h"
#include "ClusterModelTrainer.h"
#include "ClusterModelTrainerParameters.h"

#include "WeightedEarthMoversDistance2.h"

#include "IO.h"

const std::string LLP_VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("05-LLP", ' ', LLP_VERSION);

  // We need a path to some data
  TCLAP::ValueArg<std::string> 
    inputArg("i", 
	     "input", 
	     "Path to instance matrix stored as csv file",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<std::string> 
    bagLabelsArg("b", 
		 "bag-labels", 
		 "Path to bag label vector stored as csv file. Should have one bag id for each instance.",
		 true,
		 "",
		 "path", 
		 cmd);

  TCLAP::ValueArg<std::string> 
    bagProportionsArg("p", 
		      "bag-proportions", 
		      "Path to bag proportions vector stored as csv file. Should have one propotion for each bag.",
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

  TCLAP::ValueArg<int> 
    branchingArg("B", 
		 "branching", 
		 "Branching parameter to pass to flann",
		 true,
		 2,
		 ">=2", 
		 cmd);

  
  TCLAP::ValueArg<size_t> 
    kArg("k", 
	 "clusters", 
	 "Number of clusters",
	 true,
	 2,
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
  const std::string bagLabelsPath{ bagLabelsArg.getValue() };
  const std::string bagProportionsPath{ bagProportionsArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const int branching{ branchingArg.getValue() };
  const size_t k{ kArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////

  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef KMeansClusterer2< DistanceFunctorType > ClustererType;
  typedef ContinousClusterLabeller< LLPCostFunction > LabellerType;

  typedef ClusterModelTrainer< ClustererType,
			       LabellerType > TrainerType;

  typedef typename TrainerType::MatrixType MatrixType;
  typedef typename TrainerType::VectorType VectorType;
  
  typedef int LabelType;
  typedef double ElementType;

  // Parse the data into a matrix
  std::ifstream is( inputPath );
  if ( !is.good() ) {
    std::cerr << "Error reading input." << std::endl
	      << "inputPath: " << inputPath << std::endl;
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
  

  // Parse the labels into an array
  std::ifstream bagLabelsIs( bagLabelsPath );
  if ( !bagLabelsIs.good() ) {
    std::cerr << "Error reading bag labels." << std::endl
	      << "bagLabelsPath: " << bagLabelsPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< LabelType > bagLabels;
  readTextSequence< LabelType >( bagLabelsIs,
				 std::back_inserter(bagLabels),
				 rowSep );

  // Verify our assumptions about the bag labels and data
  if ( bagLabels.size() != instances.rows() ) {
    std::cerr << "There must be exactly one bag label for each instance"
	      << std::endl
	      << "bagLabels.size() = " << bagLabels.size() << std::endl
	      << "instances.rows() = " << instances.rows() << std::endl;
    return EXIT_FAILURE;
  }  


  // Parse the bag proportions into a vector
  std::ifstream bagProportionsIs( bagProportionsPath );
  if ( !bagProportionsIs.good() ) {
    std::cerr << "Error reading bag proportions." << std::endl
	      << "bagProportionsPath: " << bagProportionsPath
	      << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< ElementType > bagProportionsBuffer;
  readTextSequence<ElementType>(bagProportionsIs,
				std::back_inserter(bagProportionsBuffer),
				rowSep);
  VectorType bagProportions(bagProportionsBuffer.size());
  std::copy( bagProportionsBuffer.cbegin(),
	     bagProportionsBuffer.cend(),
	     bagProportions.data() );

  // Verify that there is at least one instance in each bag, and that each
  // instance belongs to a valid bag
  std::vector< LabelType > bagLabelsCopy( bagLabels );
  std::sort( bagLabelsCopy.begin(), bagLabelsCopy.end() );
  auto last = std::unique( bagLabelsCopy.begin(), bagLabelsCopy.end() );
  bagLabelsCopy.erase( last, bagLabelsCopy.end() );

  // Now we should have the range of numbers [0, bagProportions.size())
  if ( bagLabelsCopy.size() != bagProportions.size() ) {
    std::cerr << "Number of bag labels does not match nuber of proportions"
	      << std::endl
	      << "Unique bag labels: " << bagLabelsCopy.size() << std::endl
	      << "bagProportions.size(): " << bagProportions.size() << std::endl;
    return EXIT_FAILURE;
  }

  // If the length match and we have unique sorted values it is enough to check
  // the frint and back.
  if ( bagLabelsCopy.front() != 0 || bagLabelsCopy.back() != bagProportions.size() - 1 ) {
    std::cerr << "Bag labels must belong to the range [0," << bagProportions.size() - 1 << ")" << std::endl;
    return EXIT_FAILURE;
  }
  

  TrainerType trainer( bagProportions, instances, nHistograms,
		       bagLabels, true );
  typedef typename TrainerType::ClusterModelType ClusterModelType;
  ClusterModelType cm;
  ClusterModelTrainerParameters cmParams(k, 10);
  
  int status = trainer.train( cm, cmParams );

  std::cout << "Training status " << status << std::endl
    //	    << "Training error " << trainer.finalError() << std::endl
	    << "Weights " << cm.weights().transpose() << std::endl;

  return status;
}
