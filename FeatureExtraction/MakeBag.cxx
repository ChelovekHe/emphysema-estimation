
// We need an image, a mask, a histogram specification and optionally a set of ROIs

#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "itkClampImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkVectorImage.h"

#include "DenseHistogram.h"
#include "ImageToEmphysemaFeaturesFilter.h"
#include "IO.h"
#include "Path.h"
#include "RegionOfInterestGenerator.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".txt");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Create a bag of instances samples from an image.", ' ', VERSION);

  //
  // Required arguments
  //
  
  // We need a single image
  TCLAP::ValueArg<std::string> 
    imageArg("i", 
	     "image", 
	     "Path to image.",
	     true,
	     "",
	     "path", 
	     cmd);

  // We need a single mask
  TCLAP::ValueArg<std::string> 
    maskArg("m", 
	    "mask", 
	    "Path to mask.",
	    true,
	    "",
	    "path", 
	    cmd);

  // We need a histogram specificaton
  TCLAP::ValueArg<std::string> 
    histArg("H", 
	    "histogram-spec", 
	    "Path to histogram specification.",
	    true,
	    "",
	    "path", 
	    cmd);

  
  // We need a directory for storing the ROIs
  TCLAP::ValueArg<std::string> 
    outDirArg("o", 
	      "outdir", 
	      "Path to output directory",
	      true, 
	      "", 
	      "path", 
	      cmd);

  // We need scales for the multi scale features
  TCLAP::MultiArg<float> 
    scalesArg("s", 
	      "scale", 
	      "Scales for the Gauss applicability function",
	      true, 
	      "double", 
	      cmd);
  
  //
  // Optional arguments
  //
  
  // We can use a ROI file or generate one
  TCLAP::ValueArg<std::string> 
    roiArg("r", 
	   "roi-file", 
	   "Path to ROI file",
	   false,
	   "",
	   "path", 
	   cmd);

  TCLAP::ValueArg<size_t> 
    numROIsArg("n", 
	       "num-rois", 
	       "Number of ROIs to sample",
	       false,
	       50,
	       "N>=2", 
	       cmd);
  
  TCLAP::ValueArg<size_t> 
    roiSizeXArg("x", 
		"roi-size-x", 
		"Size of ROI in x dimension",
		false,
		41,
		"N>=1", 
		cmd);

    TCLAP::ValueArg<size_t> 
    roiSizeYArg("y", 
		"roi-size-y", 
		"Size of ROI in y dimension",
		false,
		41,
		"N>=1", 
		cmd);

    TCLAP::ValueArg<size_t> 
    roiSizeZArg("z", 
		"roi-size-z", 
		"Size of ROI in z dimension",
		false,
		41,
		"N>=1", 
		cmd);

  
  // We can use a prefix to generate filenames
  TCLAP::ValueArg<std::string> 
    prefixArg("p", 
	      "prefix", 
	      "Prefix to use for output filenames",
	      false, 
	      "", 
	      "string", 
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
  const std::string imagePath( imageArg.getValue() );
  const std::string maskPath( maskArg.getValue() );
  const std::string histPath( histArg.getValue() );
  const std::string outDirPath( outDirArg.getValue() );
  const std::vector< float > scales( scalesArg.getValue() );

  const std::string roiPath( roiArg.getValue() );
  const size_t numROIs( numROIsArg.getValue() );
  const size_t roiSizeX = roiSizeXArg.getValue();
  const size_t roiSizeY = roiSizeYArg.getValue();
  const size_t roiSizeZ = roiSizeZArg.getValue();
  const std::string prefix( prefixArg.getValue() );
  //// Commandline parsing is done ////

  const size_t numFeatures = 8;

  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef float PixelType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::Image< MaskPixelType, Dimension >  MaskImageType;
  typedef itk::VectorImage< PixelType, Dimension >  VectorImageType;

  typedef typename ImageType::SizeType SizeType;
  typedef typename ImageType::IndexType IndexType;
  typedef typename ImageType::RegionType RegionType;


  // Setup the readers
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName( imagePath );

  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( maskPath );

  // Setup a filter that ensures the mask is binary.
  // This is necesary for the feature filter, because the lung segmentation is
  // 0 => not lung. 1 => right lung.  2 => left lung.
  typedef itk::ClampImageFilter< MaskImageType,
				 MaskImageType > ClampFilterType;
  ClampFilterType::Pointer clampFilter = ClampFilterType::New();
  clampFilter->InPlaceOff();
  clampFilter->SetBounds(0, 1);
  clampFilter->SetInput( maskReader->GetOutput() );
  

  // Setup the feature filter
  typedef itk::ImageToEmphysemaFeaturesFilter<
    ImageType,
    MaskImageType,
    VectorImageType > FeatureFilterType;
  FeatureFilterType::Pointer featureFilter = FeatureFilterType::New();
  featureFilter->SetInputImage( imageReader->GetOutput() );
  featureFilter->SetInputMask( clampFilter->GetOutput() );

 
  // If we have a ROI specification file we use that, otherwise we
  // generate a set of ROIs
  std::vector< RegionType > rois;
  if ( roiPath.empty() ) {
    // Generate the ROIs
    typedef itk::RegionOfInterestGenerator< MaskImageType > ROIGeneratorType;
    ROIGeneratorType roiGenerator( clampFilter->GetOutput() );
    SizeType roiSize{ roiSizeX, roiSizeY, roiSizeZ };
    try {
      rois = roiGenerator.generate( numROIs, roiSize );
      // We should store the generated ROIs
      std::string roiFileName = prefix + "ROIInfo.txt";
      std::string roiOutPath( Path::join( outDirPath, roiFileName ) );
      std::ofstream out( roiOutPath );
      for ( auto roi : rois ) {
	out << roi.GetIndex() << roi.GetSize() << '\n';
      }
      if ( !out.good() ) {
	std::cerr << "Error writing ROI info file" << std::endl;
	return EXIT_FAILURE;
      }
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to generate ROIs." << std::endl       
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }
  }
  else {   
    // Read the roi specification
    std::ifstream is( roiPath );
    const std::streamsize count =
      std::numeric_limits<std::streamsize>::max();
    // Discard header by ignoring the first line
    is.ignore(count , '\n' );
    while ( is.good() ) {
      IndexType start;
      SizeType size;
      is.ignore( count, '[' );
      is >> start[0]; is.ignore( count, ',' );
      is >> start[1]; is.ignore( count, ',' );
      is >> start[2]; is.ignore( count, '[' );
      is >> size[0];  is.ignore( count, ',' );
      is >> size[1];  is.ignore( count, ',' );
      is >> size[2];  is.ignore( count, '\n' );
      rois.emplace_back( RegionType( start, size ) );
    }
  }
  // Now we have the rois
  
  // Setup the histogram containers
  typedef DenseHistogram< PixelType > HistogramType;
  std::vector< HistogramType > histograms;

  // We want to known how many bins there are in total
  size_t histSize = 0;
  
  // Read the histogram edge spec
  std::ifstream isHist( histPath );
  if ( !isHist.good() ) {
    std::cerr << "Could not read histogram file '" << histPath << "'" << std::endl;
    return EXIT_FAILURE;
  }
  
  while ( isHist.good() ) {
    std::string line;
    std::getline( isHist, line );
    if ( line.empty() ) {
      std::cout << "Empty line. Breaking" << std::endl;
      break;
    }
    if ( line[0] == '#' ) {
      // Skip it
      std::cout << "Skipping a line" << std::endl;
      continue;
    }
    std::stringstream ss( line );
    std::vector< PixelType > edges;
    readTextSequence< PixelType, char >( ss, std::back_inserter(edges) );
    histograms.emplace_back( HistogramType( edges.begin(), edges.end() ));
    if ( histSize == 0 ) {
      histSize = edges.size() + 1;
    }
    else if ( histSize != (edges.size() + 1) ) {
      std::cerr << "Histograms must have the same bin count"
		<< std::endl
		<< "Expected " << histSize << " Got " << edges.size()
		<< std::endl
		<< "Number of histograms " << histograms.size()
		<< std::endl;
      return EXIT_FAILURE;
    }
  }

  if ( histograms.size() != numFeatures * scales.size() ) {
    std::cerr << "Number of histograms must match number of features times number of scales"
	      << std::endl
	      << "Number of histograms = " << histograms.size() << std::endl
	      << "Number of features*scales = " << numFeatures*scales.size()
	      << std::endl;
    return EXIT_FAILURE;
  }

  const size_t totalBins = histSize * histograms.size();

  // Setup the ROI extraction filter
  // We need one for the features and one for the mask
  typedef itk::RegionOfInterestImageFilter<
		 VectorImageType,
		 VectorImageType > ROIFilterType;
  ROIFilterType::Pointer roiFilter = ROIFilterType::New();
  roiFilter->SetInput( featureFilter->GetOutput() );

  typedef itk::RegionOfInterestImageFilter<
    MaskImageType,
    MaskImageType > MaskROIFilterType;
  MaskROIFilterType::Pointer maskROIFilter = MaskROIFilterType::New();
  maskROIFilter->SetInput( clampFilter->GetOutput() );
  
  // We need to iterate over the mask to get the pixels to sample
  typedef itk::ImageRegionConstIteratorWithIndex< MaskImageType >
    MaskIteratorType;
  
  // The matrix that will store the bag
  // Each row represent a bag.
  // Each column is a bin in one of the histograms
  typedef Eigen::Matrix< PixelType,
			 Eigen::Dynamic,
			 Eigen::Dynamic> MatrixType;
  MatrixType bag( rois.size(), totalBins );

  // Now we can run the pipeline
  // Which is way to complex to have here. Wrap the parts up and make it
  // simpler.
  for ( size_t i = 0; i < scales.size(); ++i ) {
    std::cout << "Processing scale " << scales[i] << std::endl;
    featureFilter->SetSigma(scales[i]);
    try {
      // We need to update the largest possible region to make it work
      // TODO: figure what happens to the regions when looping, because it is
      // not clear.
      featureFilter->UpdateLargestPossibleRegion();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to update featureFilter." << std::endl       
		<< "Scale: " << scales[i] << std::endl
		<< "RequestedRegion: " << featureFilter->GetOutput()->GetRequestedRegion() << std::endl
		<< "LargestPossibleRegion: " << featureFilter->GetOutput()->GetLargestPossibleRegion() << std::endl
		<< "Clamp: LargestPossibleRegion(): " << clampFilter->GetOutput()->GetLargestPossibleRegion() << std::endl
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }
    
    // We process one ROI at a time
    for ( size_t j = 0; j < rois.size(); ++j ) {
      roiFilter->SetRegionOfInterest( rois[j] );
      maskROIFilter->SetRegionOfInterest( rois[j] );
      try {
	roiFilter->Update();
  	maskROIFilter->Update();
      }
      catch ( itk::ExceptionObject &e ) {
	std::cerr << "Failed to update maskROIFilter." << std::endl       
		  << "ROI: " << rois[j] << std::endl
		  << "Feature filter region: " << featureFilter->GetOutput()->GetLargestPossibleRegion() << std::endl
		  << "Clamp filter region: " << clampFilter->GetOutput()->GetLargestPossibleRegion() << std::endl
		  << "ExceptionObject: " << e << std::endl;
	return EXIT_FAILURE;
      }
      
      // Setup the mask iterator
      MaskIteratorType
	maskIter( maskROIFilter->GetOutput(),
		  maskROIFilter->GetOutput()->GetRequestedRegion() );

      VectorImageType::Pointer features( roiFilter->GetOutput() );
  
      for ( maskIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter ) {
	if ( maskIter.Get() ) {
	  auto pixel = features->GetPixel( maskIter.GetIndex() );
	  for ( size_t k = 0; k < pixel.GetSize(); ++k ) {
	    // We want the features organized by scale
	    size_t histIdx = i * numFeatures + k;
	    histograms[histIdx].insert( pixel[k] );
	  }
	}
      }
      
      // Now we add the histograms to the bag at row j.
      // We need to use the column range
      //  [ i*(numFeatures*histSize), (i+1)*(numFeatures*histSize) )
      for ( size_t k = 0; k < numFeatures; ++k ) {
	size_t histIdx = i * numFeatures + k;
	auto frequencies = histograms[histIdx].getFrequencies();
	histograms[histIdx].resetCounts();
	size_t colOffset = histIdx * histSize;
	for ( size_t l = 0; l < frequencies.size(); ++l ) {
	  bag(j, colOffset + l) = frequencies[l];
	}
      }
    }
  }
  // At this point we should have that bag is a matrix of rois and histograms
  // If I understand Eigen correctly, we can just print the matrix directly
  std::string fileName = prefix + "bag" + OUT_FILE_TYPE;
  std::string outPath( Path::join( outDirPath, fileName ) );
  std::ofstream out( outPath );
  for ( size_t i = 0; i < bag.rows(); ++i ) {
    for ( size_t j = 0; j < bag.cols(); ++j ) {
      out << bag(i,j);
      if ( j + 1 < bag.cols() ) {
	out << ",";
      }
    }
    out << '\n';
  }
  if ( !out.good() ) {
    std::cerr << "Error writing histogram to file" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
