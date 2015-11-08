/* 04-InterpretationOfClusters
   See README.md for details.
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>

#include "flann/flann.hpp"
#include "tclap/CmdLine.h"

#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"
#include "IO.h"

const std::string VERSION = "1";

int main(int argc, char *argv[]) {
  TCLAP::CmdLine cmd("04-InterpretationOfClusters", ' ', VERSION);

  // We need a path to some data
  TCLAP::ValueArg<std::string> 
    inputArg("i", 
	     "input", 
	     "Path to input matrix stored as csv file",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<std::string> 
    labelsArg("l", 
	      "labels", 
	      "Path to label vector stored as csv file",
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

  TCLAP::ValueArg<size_t> 
    nIterationsArg("N", 
		   "iterations", 
		   "Number of iterations",
		   true,
		   10,
		   "size_t", 
		   cmd);

  TCLAP::ValueArg<int> 
    branchingArg("b", 
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
  const std::string labelsPath{ labelsArg.getValue() };
  const size_t nHistograms{ nHistogramsArg.getValue() };
  const size_t nIterations{ nIterationsArg.getValue() };
  const int branching{ branchingArg.getValue() };
  const size_t k{ kArg.getValue() };
  const std::string outputPath{ outputArg.getValue() };
  
  //// Commandline parsing is done ////

  typedef unsigned char LabelType;
  typedef float ElementType;
  typedef WeightedEarthMoversDistance< ElementType > DistanceType;
  typedef KMeansClusterer< DistanceType > ClustererType;
  typedef typename ClustererType::MatrixType MatrixType;
  typedef Eigen::Matrix< ElementType, Eigen::Dynamic, 1 > VectorType;

  
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


  // Parse the labels into a vector
  std::ifstream labelsIs( labelsPath );
  if ( !labelsIs.good() ) {
    std::cerr << "Error reading labels." << std::endl
	      << "labelsPath: " << labelsPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< LabelType > labels;
  readTextSequence< LabelType >( labelsIs, std::back_inserter(labels), rowSep );

  // Verify our assumptions about the labels and data
  if ( labels.size() != instances.rows() ) {
    std::cerr << "Labels and instances must have equal number of rows" << std::endl
	      << "labels.size() = " << labels.size() << std::endl
	      << "instances.rows() = " << instances.rows() << std::endl;
    return EXIT_FAILURE;
  }  
  
  // Find the size of the label space
  std::vector< LabelType > labelSpace( labels );
  std::sort( labelSpace.begin(), labelSpace.end() );
  auto last = std::unique( labelSpace.begin(), labelSpace.end() );
  labelSpace.erase( last, labelSpace.end() );

  // Create a mapping from the label space to [labelSpace.size()]
  std::unordered_map< LabelType, size_t > labelMap;
  for ( size_t i = 0; i < labelSpace.size(); ++i ) {
    labelMap.insert( {labelSpace[i], i} );
  }
  
  // Fixed flann internal parameters
  const size_t iterations = 11;
  const flann::flann_centers_init_t centersInit{ flann::FLANN_CENTERS_KMEANSPP };

  // We have equal sized histograms
  assert( instances.cols() % nHistograms == 0 );
  const size_t nBins{ instances.cols()/nHistograms };

  std::cout << "nRows " << instances.rows() << std::endl
	    << "nCols " << instances.cols() << std::endl
	    << "nHistograms " << nHistograms << std::endl
	    << "nBins " << nBins << std::endl;
  
  // All histograms have equal weight
  typedef DistanceType::FeatureWeightType FeatureWeightType;
  std::vector< FeatureWeightType >
    weights{ nHistograms, std::make_pair(nBins, 1.0) };
      
  DistanceType dist( weights ); 

  // Prepare the outfile
  std::ofstream out( outputPath );

  // Setup the clusterer
  typedef typename ClustererType::ResultType ResultType;
  ClustererType clusterer( branching, iterations, centersInit );
  
  // We do several iterations and calculate entropy for each
  for ( size_t n = 0; n < nIterations; ++n ) {
    if ( !out.good() ) {
      std::cerr << "Error writing to file: " << outputPath << std::endl;
      return EXIT_FAILURE;
    }
    ResultType result = clusterer.cluster( instances, dist, k );

    // We should have that there is one cluster index for each instance
    assert( result.indices.size() == instances.rows() );
  
    // Setup the matrix holding the cluster label counts
    // One row for each cluster center and one column for each possible label.
    // To avoid having zero counts we initialize everything to one.
    // This will overestimate the entropy. TODO: Check for other methods
    MatrixType clusterLabelCounts = MatrixType::Ones( result.centers.rows(), labelSpace.size() );

    // Loop through the clustering and assign labels to cluster centers
    for ( size_t i = 0; i < result.indices.size(); ++i ) {
      clusterLabelCounts( result.indices[i], labelMap[labels[i]] ) += 1;
    }
  
    // Calculate the entropy for each center
    // Calculate the normalization for each row and replicate it in matrix form
    MatrixType clusterSizes = clusterLabelCounts.rowwise().sum().rowwise().replicate( clusterLabelCounts.cols() ); 

    // Estimate P(Label=label_i) with the frequencies
    MatrixType p = clusterLabelCounts.cwiseQuotient( clusterSizes );

    // Calculate entropy for each cluster as the negated sum of of p(x)*log_2(p(x))
    VectorType entropy = -1 * (p.array() * p.array().log() ).rowwise().sum();

    // Ordering of centers is arbitrary, so we sort according to entropy
    std::sort( entropy.data(), entropy.data() + entropy.rows() );
    writeSequenceAsText( out,
			 entropy.data(),
			 entropy.data() + entropy.rows() )
      << std::endl; // Flush so we can follow output
  }

  
  return EXIT_SUCCESS;
}
