
#include <unordered_map>
#include <iostream>


#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

#include "ROIReader.h"

const std::string VERSION("0.1");

int main( int argc, char* argv[] ) {
    // Commandline parsing
  TCLAP::CmdLine cmd("Calculate RA measurements inside regions of interest.", ' ', VERSION);

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

  TCLAP::ValueArg<std::string> 
    maskArg("m", 
	    "mask", 
	    "Path to mask.",
	    true,
	    "",
	    "path", 
	    cmd);

  TCLAP::ValueArg<int> 
    maskValueArg("v", 
		 "mask-value", 
		 "Value in the mask that should be used for inclusion.",
		 false,
		 1,
		 "MaskPixelType", 
		 cmd);

  
  TCLAP::ValueArg<std::string> 
    roiArg("r", 
	   "roi", 
	   "Path to roi.",
	   true,
	   "",
	   "path", 
	   cmd);

  TCLAP::ValueArg<bool> 
    roiHasHeaderArg("R", 
		    "roi-has-header", 
		    "Flag indicating if the ROI file has a header.",
		    false,
		    false,
		    "boolean", 
		    cmd);

  TCLAP::ValueArg<int> 
    raArg("t", 
	  "ra-threshold", 
	  "Threshold value for RA calculation.",
	  false,
	  -950,
	  "int", 
	  cmd);
 
  TCLAP::ValueArg<std::string> 
    outArg("o", 
	   "out", 
	   "Output path",
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
  const std::string imagePath( imageArg.getValue() );
  const std::string maskPath( maskArg.getValue() );
  const int maskValue( maskValueArg.getValue() );
  const std::string roiPath( roiArg.getValue() );
  const bool roiHasHeader( roiHasHeaderArg.getValue() );
  const std::string outPath( outArg.getValue() );
  const int raThreshold( raArg.getValue() );
  //// Commandline parsing is done ////
  
  typedef int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;

  
  // Setup the ROI reader
  typedef typename ImageType::RegionType RegionType;
  typedef ROIReader< RegionType > ROIReaderType;
  
  // Setup the image reader
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( imagePath );
  try {
    reader->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to update reader" << std::endl       
	      << "imagePath: " << imagePath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  ReaderType::Pointer maskReader = ReaderType::New();
  maskReader->SetFileName( maskPath );
  try {
    maskReader->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to update mask reader" << std::endl       
	      << "imagePath: " << maskPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  
  // Setup the image iterator
  typedef itk::ImageRegionConstIterator< ImageType > IteratorType;
  
  // Read the roi specification
  std::vector< RegionType > rois;
  try {
    rois = ROIReaderType::read( roiPath, roiHasHeader );
    std::cout << "Got " << rois.size() << " rois." << std::endl;
  }
  catch ( std::exception &e ) {
    std::cerr << "Error reading ROIs" << std::endl
	      << "roiPath: " << roiPath << std::endl
	      << "exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  // Open the output file
  std::ofstream out( outPath );
  
  // Iterate through the ROIs and count voxels <= RA threshold
  for ( size_t i = 0; i < rois.size(); ++i ) {
    if ( !out.good() ) {
      std::cerr << "Error writing to " << outPath << std::endl;
      return EXIT_FAILURE;
    }   

    // Get the counts for each pixel value
    int raCount = 0;
    int voxelCount = 0;
    try {
      IteratorType iIt( reader->GetOutput(), rois[i] );
      IteratorType mIt( maskReader->GetOutput(), rois[i] );
      for ( mIt.GoToBegin(), iIt.GoToBegin();
	    !(mIt.IsAtEnd() || iIt.IsAtEnd());
	    ++mIt, ++iIt ) {
	if ( mIt.Get() != 0 ) {
	  if ( iIt.Get() <= raThreshold ) {
	    ++raCount;
	  }
	  ++voxelCount;
	}
      }
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to process" << std::endl       
		<< "ROI: " << rois[i] << std::endl
		<< "Image->LargestPossibleRegion(): "
		<< reader->GetOutput()->GetLargestPossibleRegion() << std::endl
		<< "Mask->LargestPossibleRegion(): "
		<< maskReader->GetOutput()->GetLargestPossibleRegion() << std::endl
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }

    double proportion = static_cast<double>(raCount) / static_cast<double>(voxelCount);
    out << proportion << std::endl; // flush so we can see what happens
  }

  return EXIT_SUCCESS;
}
