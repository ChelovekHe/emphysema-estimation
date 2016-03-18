/* 
   Determine the bin widths of a histogram, such that each bin has the same 
   frequency over the population.

   The population can be estimated by sampling or the entire population can be 
   used.
*/
#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <random>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkClampImageFilter.h"

#include "DetermineEdgesForEqualizedHistogram.h"
#include "IO.h"
#include "Path.h"


const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".txt");


int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Determine bin edges for histograms.", ' ', VERSION);

  // We need a list of image/mask pairs 
  TCLAP::ValueArg<std::string> 
    imageArg("i", 
	     "infile", 
	     "Path to image/mask list.",
	     true,
	     "",
	     "path", 
	     cmd);
  
  // We need a directory for storing the resulting histogram info
  TCLAP::ValueArg<std::string> 
    outArg("o", 
	   "outfile", 
	   "Path to output file",
	   true, 
	   "", 
	   "path", 
	   cmd);
  
  // We need to know how many bins to use
  TCLAP::ValueArg<unsigned int> 
    nBinsArg("b", 
	     "bins", 
	     "Number of bins to use",
	     true, 
	     41, 
	     "unsigned int", 
	     cmd);

  // We need to know how many samples to use
  TCLAP::ValueArg<unsigned int> 
    nSamplesArg("S", 
		"samples", 
		"Number of samples to use from each (0 = all)",
		true, 
		0, 
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
  std::string infilePath( imageArg.getValue() );
  std::string outfilePath( outArg.getValue() );
  unsigned int nBins( nBinsArg.getValue() );
  unsigned int nSamples( nSamplesArg.getValue() );
  //// Commandline parsing is done ////

  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef double PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  typedef unsigned char MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension >  MaskType;
  typedef itk::ImageFileReader< MaskType > MaskReaderType;

  // Setup the random distribution we will need to seed the random iterator with
  std::random_device rd;
  std::uniform_int_distribution<int>
    dist( std::numeric_limits<int>::min(),
	  std::numeric_limits<int>::max() );
  
  // Setup the readers
  ReaderType::Pointer imageReader = ReaderType::New();
  MaskReaderType::Pointer maskReader = MaskReaderType::New();

  // Get the image/mask pairs
  std::vector< StringPair > imageMaskPairList;
  try {
    imageMaskPairList = readPairList( infilePath );
  }
  catch (...) {
    std::cerr << "Could not read image/mask list" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup a filter that ensures the mask is binary.
  typedef itk::ClampImageFilter< MaskType,
				 MaskType > ClampFilterType;
  ClampFilterType::Pointer clampFilter = ClampFilterType::New();
  clampFilter->InPlaceOff(); // Otherwise we get a segmentation fault from the random iterator
  clampFilter->SetBounds(0, 1);
  clampFilter->SetInput( maskReader->GetOutput() );
  
  // Typedefs for the iterators
  typedef itk::ImageRegionConstIteratorWithIndex< MaskType > IteratorType;
  typedef itk::ImageRandomConstIteratorWithIndex< MaskType > RandomIteratorType;

  std::vector<PixelType> samples;
  
  for ( auto imageMaskPair : imageMaskPairList ) {
    std::cout << "Processing " << std::endl
	      << "Image: '" << imageMaskPair.first << "'" << std::endl
	      << "Mask: '" << imageMaskPair.second << "'" << std::endl;
    imageReader->SetFileName( imageMaskPair.first );
    maskReader->SetFileName( imageMaskPair.second );

    try {
      imageReader->UpdateLargestPossibleRegion();
      maskReader->UpdateLargestPossibleRegion();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to Update mask reader." << std::endl
		<< "Image: '" << imageMaskPair.first << "'" << std::endl
		<< "Mask: '" << imageMaskPair.second << "'" << std::endl
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }

    RandomIteratorType
      randomIter( maskReader->GetOutput(),
		  maskReader->GetOutput()->GetLargestPossibleRegion() );
    randomIter.SetNumberOfSamples( nSamples );
    randomIter.ReinitializeSeed( dist(rd) );
    
    IteratorType
      iter( maskReader->GetOutput(),
	    maskReader->GetOutput()->GetRequestedRegion() );

    ImageType::Pointer image = imageReader->GetOutput();
    if ( nSamples == 0 ) {
      // Use all voxels
      for ( iter.GoToBegin(); !iter.IsAtEnd(); ++iter ) {
	if ( iter.Get() ) {
	  samples.push_back( image->GetPixel( iter.GetIndex() ) );
	}
      }
    }
    else {
      unsigned int nSampled = 0;
      while ( nSampled < nSamples ) {
	for ( randomIter.GoToBegin(); !randomIter.IsAtEnd(); ++randomIter ) {
	  if ( randomIter.Get() ) {
	    samples.push_back( image->GetPixel( randomIter.GetIndex() ) );
	    if ( ++nSampled == nSamples ) {
	      break;
	    }
	  }
	}
      }
    }
  }
      
  
  // Setup the outfile
  std::ofstream out( outfilePath );

  // Write a header
  out << "# Features: Intensity\n"
      << "# Scales: 0\n";
  if ( !out.good() ) {
    std::cerr << "Error writing edges header to file." << std::endl
	      << "Out path: " << outfilePath << std::endl;
    return EXIT_FAILURE;
  }  
  
  // Now we find the equalizing edges for the histogram
  std::sort(samples.begin(), samples.end());
  std::vector< PixelType > edges;
  determineEdgesForEqualizedHistogram( samples.begin(),
				       samples.end(),
				       std::back_inserter(edges),
				       nBins );
  writeSequenceAsText( out, edges.begin(), edges.end() );
  out << std::endl;
  if ( !out.good() ) {
    std::cerr << "Error writing to edges to file." << std::endl
	      << "Out path: " << outfilePath << std::endl;
    return EXIT_FAILURE;
  }  
  return EXIT_SUCCESS;
}
     
