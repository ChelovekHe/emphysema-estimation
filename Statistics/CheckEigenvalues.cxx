/* 
*/
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageRegionIteratorWithIndex.h"

const std::string VERSION("0.1");


int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Check that |I1| >= |I2| >= |I3|.", ' ', VERSION);

  TCLAP::ValueArg<std::string> 
    image1Arg("a", 
	      "image-1", 
	      "Path to first image.",
	      true,
	      "",
	      "path", 
	      cmd);

  TCLAP::ValueArg<std::string> 
    image2Arg("b", 
	     "image-2", 
	     "Path to second image.",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<std::string> 
    image3Arg("c", 
	     "image-3", 
	     "Path to third image.",
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
  const std::string image1Path( image1Arg.getValue() );
  const std::string image2Path( image2Arg.getValue() );
  const std::string image3Path( image3Arg.getValue() );

  //// Commandline parsing is done ////

  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  // Setup the readers
  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( image1Path );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( image2Path );

  ReaderType::Pointer reader3 = ReaderType::New();
  reader3->SetFileName( image3Path );

  // We need to read the data before setting up iterators
  try {
    reader1->Update();
    reader2->Update();
    reader3->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Error updating readers." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  typedef itk::ImageRegionConstIteratorWithIndex< ImageType > IteratorType;
  IteratorType iter1( reader1->GetOutput(), reader1->GetOutput()->GetRequestedRegion() );
  IteratorType iter2( reader2->GetOutput(), reader2->GetOutput()->GetRequestedRegion() );
  IteratorType iter3( reader3->GetOutput(), reader3->GetOutput()->GetRequestedRegion() );

  iter1.GoToBegin();
  iter2.GoToBegin();
  iter3.GoToBegin();
  unsigned long n = 0;
  while ( !(iter1.IsAtEnd() || iter2.IsAtEnd() || iter3.IsAtEnd()) ) {
    auto v1 = std::abs(iter1.Get());
    auto v2 = std::abs(iter2.Get());
    auto v3 = std::abs(iter3.Get());
    if ( v1 < v2 ) {
      std::cout << "|v1| < |v2|: " << v1 << " : " << v2 << " : " << v3 << std::endl
		<< "at " << iter1.GetIndex() << iter2.GetIndex() << iter3.GetIndex() << std::endl;
    }
    if ( v2 < v3 ) {
      std::cout << "|v2| < |v3|: " << v1 << " : " << v2 << " : " << v3 << std::endl
		<< "at " << iter1.GetIndex() << iter2.GetIndex() << iter3.GetIndex() << std::endl;
    }
    ++iter1; ++iter2; ++iter3;
    ++n;
    // if ( v3 > 0 ) {
    //   std::cout << v1 << " >= " << v2 << " >= " << v3 << std::endl;
    // }
  }
  std::cout << n << " values checked" << std::endl;
  return EXIT_SUCCESS;
}
