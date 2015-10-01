/* 
   Sample ROIs from an image
*/
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"

#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".nii.gz");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Sample ROIs from image.", ' ', VERSION);

  // We need a single image
  TCLAP::ValueArg<std::string> 
    imageArg("i", 
	     "image", 
	     "Path to image.",
	     true,
	     "",
	     "path", 
	     cmd);

  // We need a single ROI file
  TCLAP::ValueArg<std::string> 
    roiArg("r", 
	   "ROIs", 
	   "Path to ROI file",
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
  std::string imagePath( imageArg.getValue() );
  std::string roiPath( roiArg.getValue() );
  std::string outDirPath( outDirArg.getValue() );
  std::string prefix( prefixArg.getValue() );
  //// Commandline parsing is done ////


  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
    
  // Setup the readers
  ReaderType::Pointer imageReader = ReaderType::New();
  imageReader->SetFileName( imagePath );
  
  // Setup the ROI extraction filter 
  typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;
  ROIFilterType::Pointer roiFilter = ROIFilterType::New();
  roiFilter->SetInput( imageReader->GetOutput() );


  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( roiFilter->GetOutput() );
  
  // Base file name for output images
  std::string baseFileName = Path::join( outDirPath, prefix );

  // Read the ROI info and start the ROI sampling
  std::ifstream is( roiPath );
  const std::streamsize count = std::numeric_limits<std::streamsize>::max();  
  if ( is.good() ) {
    // Discard header
    is.ignore(count , '\n' );
    while ( is.good() ) {
      ImageType::IndexType start;
      ImageType::SizeType size;
      unsigned int nROI;
      is >> nROI;     is.ignore( count, '[' );
      is >> start[0]; is.ignore( count, ',' );
      is >> start[1]; is.ignore( count, ',' );
      is >> start[2]; is.ignore( count, '[' );
      is >> size[0];  is.ignore( count, ',' );
      is >> size[1];  is.ignore( count, ',' );
      is >> size[2];  is.ignore( count, '\n' );

      roiFilter->SetRegionOfInterest( ImageType::RegionType ( start, size ) );

      // Create a filename and write the ROI to disk
      std::ostringstream outFileBuf( baseFileName + "ROI_", std::ios_base::ate );
      outFileBuf << std::setfill('0') << std::setw(5) << nROI << OUT_FILE_TYPE;
      writer->SetFileName( outFileBuf.str() );
      try {
	writer->Update();
      }
      catch ( itk::ExceptionObject &e ) {
	std::cerr << "Failed to process." << std::endl       
	  	  << "Image: " << imagePath << std::endl
		  << "ROI: " << roiPath << std::endl
		  << "Base file name: " << baseFileName << std::endl
		  << "ExceptionObject: " << e << std::endl;
	return EXIT_FAILURE;
      }
    }
  }
  else {
    std::cerr << "No ROIs read." << std::endl
	      << "Image: " << imagePath << std::endl
	      << "ROI: " << roiPath << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
