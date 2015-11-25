/* 05-LLP
   See README.md for details.
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "KMeansClusterer2.h"
#include "LLPCostFunction.h"
#include "BagProportionError.h"
#include "ContinousClusterLabeller.h"

#include "ClusterModel.h"
#include "ClusterModelTrainer3.h"
#include "ClusterModelTrainerParameters.h"
#include "ClusterModelTester2.h"

#include "LLPCrossValidator.h"

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

    TCLAP::ValueArg<int> 
    maxItersArg("m", 
		"max-iters", 
		"Maximum iterations of CMA-ES. Set to <= 0 to let CMA-ES decide.",
		false,
		0,
		"it", 
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
  const int maxIters{ maxItersArg.getValue() };
  
  //// Commandline parsing is done ////

  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef KMeansClusterer2< DistanceFunctorType > ClustererType;
  typedef ContinousClusterLabeller< LLPCostFunction > LabellerType;

  typedef ClusterModelTrainer3< ClustererType, LabellerType > TrainerType;
  typedef typename TrainerType::ModelType ModelType;  
  typedef typename ModelType::MatrixType MatrixType;
  typedef typename ModelType::VectorType VectorType;
  typedef ClusterModelTester<
    ModelType,
    BagProportionError< VectorType, MatrixType > > TesterType;
  
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

  
  // We want to use cross validation to estimate the performance
  typedef LLPCrossValidator< TrainerType, TesterType > ValidatorType;
  const unsigned int nFolds = 10;
  CrossValidationParams cvParams(
    CrossValidationType::K_FOLD,
    nFolds, // how many folds to use
    true    // Shuffle instances before partitioning in folds
  );

  // This guy should include more clustering specific parameters
  ClusterModelTrainerParameters trainerParams(
    nHistograms, // Feature space dimension
    k,           // Number of clusters
    maxIters,    // Maximum number of iterations of CMA-ES
    outputPath,  // Path to trace file for CMA-ES
    -1,          // Sigma for CMA-ES
    -1,          // Lambda for CMA-ES
    0,           // Random seed for CMA-ES
    false        // Toggle trace for trainer
  );

  ClustererType clusterer( branching );
  LabellerType labeller;
  TrainerType trainer( clusterer, labeller, trainerParams );
  TesterType tester;
  
  ValidatorType validator;
  auto result = validator.run( bagProportions,
			       instances,
			       bagLabels,
			       trainer,
			       tester,
			       cvParams );
  
  // Store the results
  std::ofstream out( "LLP3.result" );
  out << "trainLoss, testLoss" << std::endl; 
  for ( std::size_t i = 0; i < result.trainingLosses.size(); ++i ) {
    out << result.trainingLosses[i] << ", "
	<< result.testLosses[i] << std::endl;
  }
  
  return 0;
}
