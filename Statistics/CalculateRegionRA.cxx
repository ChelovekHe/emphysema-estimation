
#include <unordered_map>
#include <iostream>


#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

const std::string VERSION("0.1");

int main( int argc, char* argv[] ) {
    // Commandline parsing
  TCLAP::CmdLine cmd("Calculate RA measurements inside a masked region.", ' ', VERSION);

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
  
  TCLAP::ValueArg<int> 
    raArg("t", 
	  "ra-threshold", 
	  "Threshold value for RA calculation.",
	  false,
	  -950,
	  "int", 
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
  const int raThreshold( raArg.getValue() );
  //// Commandline parsing is done ////
  
  typedef int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  
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

  
  // The image iterator
  typedef itk::ImageRegionConstIterator< ImageType > IteratorType;
    
  // Get the counts for each pixel value
  int raCount = 0;
  int voxelCount = 0;
  try {
    IteratorType iIt( reader->GetOutput(), reader->GetOutput()->GetRequestedRegion() );
    IteratorType mIt( maskReader->GetOutput(), maskReader->GetOutput()->GetRequestedRegion() );
    for ( mIt.GoToBegin(), iIt.GoToBegin();
	  !(mIt.IsAtEnd() || iIt.IsAtEnd());
	  ++mIt, ++iIt ) {
      if ( mIt.Get() == maskValue ) {
	if ( iIt.Get() <= raThreshold ) {
	  ++raCount;
	}
	++voxelCount;
      }
    }
    std::cout << "raCount = " << raCount << ". voxelCount = " << voxelCount << std::endl;
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to process" << std::endl       
	      << "Image->RequestedRegion(): "
	      << reader->GetOutput()->GetRequestedRegion() << std::endl
	      << "Mask->RequestedRegion: "
	      << maskReader->GetOutput()->GetRequestedRegion() << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  double proportion = static_cast<double>(raCount) / static_cast<double>(voxelCount);
  std::cout << proportion << std::endl;

  return EXIT_SUCCESS;
}
