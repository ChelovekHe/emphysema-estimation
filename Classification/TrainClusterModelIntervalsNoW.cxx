/* 
   Train a CMS model with binary cluster labeling and interval error
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "KMeansClusterer2.h"
#include "IntervalSumOfAbsoluteError.h"
#include "GreedyBinaryClusterLabeller.h"

#include "ClusterModel.h"
#include "ClusterModelTrainerNoCMAES.h"
#include "ClusterModelTrainerParameters.h"

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
    bagLabelsArg("p", 
		 "bag-labels", 
		 "Path to bag labels vector stored as csv file. Should have one label for each bag.",
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

  TCLAP::ValueArg<int> 
    kMeansIterationsArg("I", 
			"kmeans-iterations", 
			"Iterations parameter to pass to flann",
			false,
			11,
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
	      "Base path for output files",
	      true,
	      "",
	      "path", 
	      cmd);

    TCLAP::ValueArg<int> 
    maxItersArg("m", 
		"max-iterations", 
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
  const std::string instancesPath{ instancesArg.getValue() };
  const std::string bagMembershipPath{ bagMembershipArg.getValue() };
  const std::string bagLabelsPath{ bagLabelsArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const int branching{ branchingArg.getValue() };
  const int kMeansIterations{ kMeansIterationsArg.getValue() };
  const size_t k{ kArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  const int maxIters{ maxItersArg.getValue() };
  
  //// Commandline parsing is done ////
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType VectorType;
  
  typedef WeightedEarthMoversDistance2 DistanceFunctorType;
  typedef KMeansClusterer2< DistanceFunctorType > ClustererType;
  typedef GreedyBinaryClusterLabeller<
    MatrixType,
    VectorType,
    IntervalSumOfAbsoluteError > LabellerType;

  typedef ClusterModelTrainerNoCMAES< ClustererType, LabellerType > TrainerType;
  typedef typename TrainerType::ModelType ModelType;  
  
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
    std::cerr << "There must be exactly one bag label for each instance"
	      << std::endl
	      << "bagMembership.size() = " << bagMembership.size() << std::endl
	      << "instances.rows() = " << instances.rows() << std::endl;
    return EXIT_FAILURE;
  }  


  // Parse the bag labels into a matrix
  std::ifstream bagLabelsIs( bagLabelsPath );
  if ( !bagLabelsIs.good() ) {
    std::cerr << "Error reading bag labels." << std::endl
	      << "bagLabelsPath: " << bagLabelsPath
	      << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< ElementType > bagLabelsBuffer;
  auto bagLabelsDim =
    readTextMatrix<ElementType>(bagLabelsIs, std::back_inserter(bagLabelsBuffer), colSep, rowSep);
  if ( bagLabelsDim.second != 2 ) {
    std::cerr << "Bag labels must be intervals given as start, end" << std::endl;
    return EXIT_FAILURE;
  }
  MatrixType bagLabels( bagLabelsDim.first, bagLabelsDim.second );
  std::copy( bagLabelsBuffer.cbegin(), bagLabelsBuffer.cend(), bagLabels.data() );
  
  
  // Verify that there is at least one instance in each bag, and that each
  // instance belongs to a valid bag
  std::vector< LabelType > bagMembershipCopy( bagMembership );
  std::sort( bagMembershipCopy.begin(), bagMembershipCopy.end() );
  auto last = std::unique( bagMembershipCopy.begin(), bagMembershipCopy.end() );
  bagMembershipCopy.erase( last, bagMembershipCopy.end() );

  // Now we should have the range of numbers [0, bagLabels.rows())
  if ( bagMembershipCopy.size() != bagLabels.rows() ) {
    std::cerr << "Size of bag membership does not match number of labels" << std::endl
	      << "Unique bags: " << bagMembershipCopy.size() << std::endl
	      << "bagLabels.rows(): " << bagLabels.rows() << std::endl;
    return EXIT_FAILURE;
  }

  // If the length match and we have unique sorted values it is enough to check
  // the front and back.
  if ( bagMembershipCopy.front() != 0 || bagMembershipCopy.back() != bagLabels.rows() - 1 ) {
    std::cerr << "Bag membership must belong to the range [0," << bagLabels.size() - 1 << ")"
	      << std::endl;
    return EXIT_FAILURE;
  }
   
  ClusterModelTrainerParameters trainerParams(
    nHistograms,      // Feature space dimension
    k,                // Number of clusters
    maxIters,         // Maximum number of iterations of CMA-ES
    outputPath,       // Path to store model and trace file for CMA-ES
    0.5,              // Sigma for CMA-ES
    -1,               // Lambda for CMA-ES
    0,                // Random seed for CMA-ES
    false             // Toggle trace for trainer
  );

  ClustererType clusterer( branching, kMeansIterations );
  LabellerType labeller;
  TrainerType trainer( clusterer, labeller, trainerParams );
  ModelType model;

  auto trainLoss = trainer.train( bagLabels,
  				  instances,
  				  bagMembership,
  				  model );

  std::string lossesOutputPath = outputPath + "training_loss.txt";
  std::ofstream lossesOut( lossesOutputPath );
  lossesOut << trainLoss << std::endl;
  
  return 0;
}
