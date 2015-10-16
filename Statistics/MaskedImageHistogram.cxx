/* 
   Calculate histogram of masked image using specified bin widths
*/
#include <string>
#include <vector>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageConstIterator.h"

#include "DateTime.h"
#include "DenseHistogram.h"
#include "IO.h"
#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".txt");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Calculate histogram of masked image.", ' ', VERSION);

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

  // We need a histogram specification
  TCLAP::ValueArg<std::string> 
    histArg("s", 
	    "histogram", 
	    "Path to histogram specification.",
	    true,
	    "",
	    "path", 
	    cmd);

  // We need a directory for storing the resulting histogram
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
	      "histogram_", 
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
  std::string maskPath( maskArg.getValue() );
  std::string histPath( histArg.getValue() );
  std::string outDirPath( outDirArg.getValue() );
  std::string prefix( prefixArg.getValue() );
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

  // Force an update of the readers so the iterators will work
  try {
    imageReader->Update();
    maskReader->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to Update mask reader." << std::endl
	      << "Image: " << imagePath << std::endl
	      << "Mask: " << maskPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  // // Read the histogram spec
  // // Assumes the histogram is specified with the right edge of bins,
  // // and that there is an implicit bin after the last edge.
  std::ifstream histStream( histPath );
  std::vector< PixelType > edges;
  readTextSequence<float,char>( histStream, std::back_inserter(edges) );
  DenseHistogram< PixelType > hist( edges.begin(), edges.end() );
    
  
  // Setup the mask iterator
  typedef itk::ImageRegionConstIterator< MaskImageType > MaskIteratorType;
  MaskIteratorType
    maskIter( maskReader->GetOutput(),
	      maskReader->GetOutput()->GetLargestPossibleRegion() );

  ImageType::Pointer image( imageReader->GetOutput() );
  
  for ( maskIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter ) {
    if ( maskIter.Get() ) {
      hist.insert( image->GetPixel( maskIter.GetIndex() ) );
    }
  }

  // We need the histogram frequencies
  auto frequencies = hist.getFrequencies();
  
  std::string fileName = prefix + "hist" + OUT_FILE_TYPE;
  std::string outPath( Path::join( outDirPath, fileName ) );
  std::ofstream out( outPath );
  writeSequenceAsText( out, frequencies.begin(), frequencies.end() );
  if ( !out.good() ) {
    std::cerr << "Error writing histogram to file" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
