/* 
   Sample cubic ROIs at random from a masked volume.
*/
#include <string>
#include <vector>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkImageRandomConstIteratorWithIndex.h"

#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".nii.gz");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Calculate gradient based features.", ' ', VERSION);

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
	    "Path to mask. Must match image dimensions.",
	    true,
	    "",
	    "path", 
	    cmd);
  
  // We need a directory for storing the resulting images
  TCLAP::ValueArg<std::string> 
    outDirArg("o", 
	      "outdir", 
	      "Path to output directory",
	      true, 
	      "", 
	      "path", 
	      cmd);
  
  // We can use a prefix to generate filenames
  TCLAP::ValueArg<std::string> 
    prefixArg("p", 
	      "prefix", 
	      "Prefix to use for output filenames",
	      false, 
	      "cubicroi__", 
	      "string", 
	      cmd);

  // We need a number of ROIs to sample
  TCLAP::ValueArg<unsigned int> 
    nROIsArg("n", 
	     "numROIs", 
	     "Number of ROIs to sample",
	     false, 
	     50, 
	     "unsigned int", 
	     cmd);

  // We need the size of the ROIs
  TCLAP::ValueArg<unsigned int> 
    roiSizeArg("s", 
	       "size", 
	       "Size of the ROIs. All ROIs are cubes (s x s x s).",
	       false, 
	       41, 
	       "unsigned int", 
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
  std::string imagePath( imageArg.getValue() );
  std::string maskPath( maskArg.getValue() );
  std::string outDirPath( outDirArg.getValue() );
  std::string prefix( prefixArg.getValue() );
  unsigned int nROIs( nROIsArg.getValue() );
  unsigned int roiSize( roiSizeArg.getValue() );

  //// Commandline parsing is done ////


  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension >  MaskImageType;
  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
    
  // Setup the readers
  ReaderType::Pointer imageReader = ReaderType::New();
  imageReader->SetFileName( imagePath );

  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( maskPath );

  // Do we need to force an update of the mask reader?
  try {
    maskReader->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to Update mask reader." << std::endl
	      << "Image: " << imagePath << std::endl
	      << "Mask: " << maskPath << std::endl
      //	      << "Base file name: " << baseFileName << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << maskReader->GetOutput()->GetLargestPossibleRegion() << std::endl;
  
  // Setup the ROI extraction filter 
  typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;
  ROIFilterType::Pointer roiFilter = ROIFilterType::New();
  roiFilter->SetInput( imageReader->GetOutput() );

  // Setup the region. We know the size but start index is set in the loop
  assert( roiSize > 0 );
  ImageType::SizeType size;
  size.Fill( roiSize );
  ImageType::RegionType roi;
  roi.SetSize( size );

  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( roiFilter->GetOutput() );
  
  // Base file name for output images
  std::string baseFileName = Path<char>::Join( outDirPath, prefix );

  // Setup the random iterator that walks the mask image
  typedef itk::ImageRandomConstIteratorWithIndex< MaskImageType >
    RandomIteratorType;
  RandomIteratorType
    maskIterator( maskReader->GetOutput(),
		  maskReader->GetOutput()->GetLargestPossibleRegion() );

  // We want one sample for each ROI
    maskIterator.SetNumberOfSamples( nROIs );

  // We need to keep track of the ROI centers
  std::vector< MaskImageType::IndexType > centers;
  centers.reserve( nROIs );

  // Run the ROI sampling loop  
  for ( unsigned int nROI = 0; nROI < nROIs;  ) {
    for ( maskIterator.GoToBegin();
	  !maskIterator.IsAtEnd() && nROI < nROIs;
	  ++maskIterator ) {
      if ( maskIterator.Get() == 1 ) {
	// We sample this location. The location is the center of the ROI, so we
	// need to offset it by half the size
	MaskImageType::IndexType start = maskIterator.GetIndex();
	std::cout << "Got center " << start << std::endl;
	centers.push_back( start );
	start[0] -= size[0]/2;
	start[1] -= size[1]/2;
	start[2] -= size[2]/2;
	roi.SetIndex( start );
	roiFilter->SetRegionOfInterest( roi );

	// Create a filename and write the ROI to disk
	std::string outFile = baseFileName + "ROI_" + std::to_string( nROI )
	  + OUT_FILE_TYPE;
	writer->SetFileName( outFile );
	try {
	  writer->Update();
	}
	catch ( itk::ExceptionObject &e ) {
	  std::cerr << "Failed to process." << std::endl
		    << "Image: " << imagePath << std::endl
		    << "Mask: " << maskPath << std::endl
		    << "Base file name: " << baseFileName << std::endl
		    << "ExceptionObject: " << e << std::endl;
	  return EXIT_FAILURE;
	}
	++nROI;
      }
    }
  }

  // Write the ROI info
  std::string infoFile = baseFileName + "ROIInfo.txt";
  std::ofstream out( infoFile );
  if ( out.good() ) {
    out << "ROI, center, size" << std::endl;
  }
  for ( unsigned int i = 0; i < centers.size(); ++i ) {
    if ( out.good() ) {
      out << i << ", " << centers[i] << ", " << size << std::endl;
    }
  }
  if ( !out.good() ) {
    std::cerr << "Error writing ROI info file" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
