/* 05-LLP
   See README.md for details.
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "Eigen/Dense"

#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "libcmaes/cmaes.h"
//#include "libcmaes/pwq_bound_strategy.h"

#include "LLP2.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
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

  typedef std::size_t LabelType;
  typedef double ElementType;
  typedef Eigen::Matrix< ElementType,
			 Eigen::Dynamic,
			 Eigen::Dynamic,
			 Eigen::RowMajor > MatrixType;
  typedef Eigen::Matrix< ElementType, Eigen::Dynamic, 1 > VectorType;
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
  
  
  // Fixed flann internal parameters
  const size_t iterations = 11;
  const flann::flann_centers_init_t centersInit{ flann::FLANN_CENTERS_KMEANSPP };


  //
  std::cout << "nRows " << instances.rows() << std::endl
	    << "nCols " << instances.cols() << std::endl
	    << "nHistograms " << nHistograms << std::endl
	    << "nBins " << nBins << std::endl;


  // We need a function that gets a set of feature weights and return an objective value + the cluster centers.
  
  // All histograms have equal weight initially
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(nBins, 1.0) };
      
  DistanceType dist( weights );

  // Create the LLP2 functor that is our objective
  LLP2 llp( bagProportions, instances, bagLabels, k, nHistograms, nBins );
  std::function< double(const double*, const int&) > llp_wrapper
    = llp;

  // Set bounds on the weights
  // See https://github.com/beniz/libcmaes/wiki/Defining-and-using-bounds-on-parameters
  const unsigned int dim = weights.size();
  std::vector< double > lbounds( dim, 0.0 );
  std::vector< double > ubounds( dim, 1.0 );
  std::vector< double > x0( dim, 0.5 );

  typedef libcmaes::GenoPheno< libcmaes::pwqBoundStrategy > GenoPheno;
  typedef libcmaes::CMAParameters< GenoPheno > CMAParameters;
  typedef libcmaes::CMASolutions CMASolutions;
  
  GenoPheno gp(&lbounds.front(), &ubounds.front(), dim);

  const int lambda = -1;   // Automatically decided
  const double sigma = -1; // Automatically set
  const uint64_t seed = 0; // Automatically set
  CMAParameters cmaParams(dim, &x0.front(), sigma, lambda, seed, gp); 
  cmaParams.set_algo( aBIPOP_CMAES ); // Using BIPOP restart strategy
  //cmaParams.set_algo( aCMAES );

  // We want trace of execution we can visualize after
  cmaParams.set_fplot(outputPath);

  // We want a progress function that prints some info
  libcmaes::ProgressFunc<
    CMAParameters,
    CMASolutions > select_time = [](const CMAParameters &cmaparams,
				    const CMASolutions &cmasols) {
    if (cmasols.niter() % 100 == 0)
      std::cerr << cmasols.elapsed_last_iter() << std::endl;
    return 0;
  };

  if ( maxIters > 0 ) {
  // We want to stop after a reasonable number of iterations
    cmaParams.set_max_iter( maxIters );
  }
  
  // Optimize
  std::cout << "Running CMA-ES optimization" << std::endl;
  CMASolutions cmaSols =
    libcmaes::cmaes< GenoPheno >(llp_wrapper, cmaParams, select_time);

  std::cout << "best solution: " << cmaSols << std::endl;
  std::cout << "optimization took " << cmaSols.elapsed_time() / 1000.0 << " seconds\n";

  cmaSols.print(std::cout, 0, gp);

  auto x = gp.pheno(cmaSols.best_candidate().get_x_dvec());
  llp(x.data(), nHistograms);
  
  auto result = llp.getResult();
  std::cout << "Result from last call to llp" << std::endl
	    << "C: " << std::endl << result.C << std::endl
	    << "labels: " << std::endl << result.labels << std::endl
	    << "error: " << std::endl << result.error << std::endl
	    << "status: " << std::endl << cmaSols.run_status() << std::endl;
    
  return cmaSols.run_status();
}
