
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "Eigen/Dense"

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

#include "IO.h"
#include "ROIReader.h"

const std::string VERSION("0.1");

int main(int argc, char *argv[]) {
  typedef int MaskPixelType;
  
  // Commandline parsing
  TCLAP::CmdLine cmd("Create a heat map from a set of ROIs and an image.", ' ', VERSION);

  //
  // Required arguments
  //
  
  // We need a single mask
  TCLAP::ValueArg<std::string> 
    maskArg("m", 
	    "mask", 
	    "Path to mask.",
	    true,
	    "",
	    "path", 
	    cmd);
  
  TCLAP::ValueArg<std::string> 
    roiArg("r", 
	   "roi-file", 
	   "Path to ROI file. If given the ROIs in this file will be used,"
	   "otherwise ROIs will be generated.",
	   true,
	   "",
	   "path", 
	   cmd);

  TCLAP::ValueArg<MaskPixelType> 
    roiMaskValueArg("v", 
		    "roi-mask-value", 
		    "Value in the mask that should be used for inclusion.",
		    false,
		    1,
		    "MaskPixelType", 
		    cmd);

  TCLAP::ValueArg<std::string> 
    predictionsArg("p", 
		   "predictions", 
		   "Path to predictions file.",
		   true,
		   "",
		   "path", 
		   cmd);

  
  TCLAP::ValueArg<std::string> 
    outArg("o", 
	   "out", 
	   "Path to output file",
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
  const std::string maskPath( maskArg.getValue() );
  const std::string roiPath( roiArg.getValue() );
  const std::string predictionsPath( predictionsArg.getValue() );
  const std::string outPath( outArg.getValue() );

  const MaskPixelType roiMaskValue( roiMaskValueArg.getValue() );
  //// Commandline parsing is done ////

  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef itk::Image< MaskPixelType, Dimension >  MaskImageType;

  typedef typename MaskImageType::SizeType SizeType;
  typedef typename MaskImageType::IndexType IndexType;
  typedef typename MaskImageType::RegionType RegionType;


  // Read the mask
  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( maskPath );
  try {
    maskReader->Update();
  }
  catch (itk::ExceptionObject& e ) {
    std::cerr << "Failed to read mask image." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }


  // Read the predictions
  char rowSep = '\n';
  std::ifstream predictionsIs( predictionsPath );
  if ( !predictionsIs.good() ) {
    std::cerr << "Error reading patch predictions." << std::endl
	      << "predictionsPath: " << predictionsPath << std::endl;
    return EXIT_FAILURE;
  }
  std::vector< int > predictions;
  readTextSequence< int >( predictionsIs,
			    std::back_inserter( predictions ),
			    rowSep );
  
  
  // Read the roi specification
  std::vector< RegionType > rois;
  typedef ROIReader< RegionType > ROIReaderType;
  try {
    rois = ROIReaderType::read( roiPath, false );
    std::cout << "Got " << rois.size() << " rois." << std::endl;
  }
  catch ( std::exception &e ) {
    std::cerr << "Error reading ROIs" << std::endl
	      << "roiPath: " << roiPath << std::endl
	      << "exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if ( rois.size() != predictions.size() ) {
    std::cerr << "Number of ROIs and number of predictions do not match." << std::endl
	      << "Number of ROIs: " << rois.size() << std::endl
	      << "Number of predictions: " << predictions.size() << std::endl;
    return EXIT_FAILURE;
  }
  
  // Copy the dimensions and spacing of the mask
  MaskImageType::Pointer mask = maskReader->GetOutput();
  MaskImageType::Pointer roiMask = MaskImageType::New();
  roiMask->SetRegions( mask->GetLargestPossibleRegion() );
  roiMask->SetSpacing( mask->GetSpacing() );
  roiMask->SetOrigin( mask->GetOrigin() );
    
  try {
    roiMask->Allocate();
  }
  catch ( itk::ExceptionObject& e ) {
    std::cerr << "Failed to allocate new image." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  

  // We need to iterate over the mask to get the pixels to sample
  typedef itk::ImageRegionConstIteratorWithIndex< MaskImageType >  MaskIteratorType;
  MaskIteratorType maskIter( mask, mask->GetLargestPossibleRegion() );

  SizeType singleVoxel{1,1,1};  
  for ( maskIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter ) {
    if ( maskIter.Get() == roiMaskValue ) {
      auto idx = maskIter.GetIndex();
      int voxelPrediction = 0;
      for ( std::size_t i = 0; i < rois.size(); ++i ) {
	if ( rois[i].IsInside( RegionType(idx, singleVoxel) ) ) {
	  voxelPrediction += predictions[i];
	}
      }
      roiMask->SetPixel( idx, voxelPrediction );
    }
  }
  

  // Setup the writer
  typedef itk::ImageFileWriter< MaskImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( roiMask );
  writer->SetFileName( outPath );
  
  try {
    writer->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to process." << std::endl
	      << "Mask: " << maskPath << std::endl
	      << "ROI: " << roiPath << std::endl
	      << "Out: " << outPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  

  return EXIT_SUCCESS;
}
