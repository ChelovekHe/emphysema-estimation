/* 
   Binarize an image
 */
#include <string>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"


const std::string VERSION("0.1");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Binarize an image.", ' ', VERSION);

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
    outArg("o", 
	   "outfile", 
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
  std::string imagePath( imageArg.getValue() );
  std::string outPath( outArg.getValue() );
  //// Commandline parsing is done ////


  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef unsigned short PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
    
  // Setup the readers
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( imagePath );

  // Setup the binary filter
  typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> FilterType;
   FilterType::Pointer filter = FilterType::New();
   filter->SetInput(reader->GetOutput());
   filter->SetLowerThreshold(0);
   filter->SetUpperThreshold(0);
   filter->SetInsideValue(0);
   filter->SetOutsideValue(1);
  
  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( outPath );
  
  try {
    writer->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to process." << std::endl
	      << "Image: " << imagePath << std::endl
	      << "Out: " << outPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
