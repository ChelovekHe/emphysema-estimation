/* 
   Determine the bin widths of a histogram, such that each bin has the same 
   frequency over the population.

   The population can be estimated by sampling or the entire population can be 
   used.

   Initially we have no idea what intensity ranges to expect. So we first 
   accumulate all the samples, keeping track of min/max. Then we map the 
   intensity range to some fixed intensity range, e.g [0:256), and round to 
   integers.

   From this integer range, we can estimate the cdf and thus the inverse cdf, 
   which is used for equalization.

*/
#include <limits>
#include <string>
#include <vector>
#include <iostream>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageConstIterator.h"
#include "itkImageRandomConstIteratorWithIndex.h"

#include "IO.h"
#include "DateTime.h"
#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".txt");


int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Determine bin widths for histograms.", ' ', VERSION);

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
	      "histogram_info_", 
	      "string", 
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
    nSamplesArg("s", 
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
  std::string outDirPath( outDirArg.getValue() );
  std::string prefix( prefixArg.getValue() );
  unsigned int nBins( nBinsArg.getValue() );
  unsigned int nSamples( nSamplesArg.getValue() );
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


  // Maybe reserve some memory?
  std::vector< PixelType > samples; 
  PixelType min = std::numeric_limits< PixelType >::max();
  PixelType max = std::numeric_limits< PixelType >::lowest();
  
  // Typedefs for the iterators
  typedef itk::ImageRegionConstIterator< MaskImageType > IteratorType;
  typedef itk::ImageRandomConstIteratorWithIndex< MaskImageType >
    RandomIteratorType;
        
  for ( auto imageMaskPair : imageMaskPairList ) {
    imageReader->SetFileName( imageMaskPair.first );
    maskReader->SetFileName( imageMaskPair.second );

    // Force an update of the readers so the iterators will work
    try {
      imageReader->Update();
      maskReader->Update();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to Update readers." << std::endl
		<< "Image: " << imageMaskPair.first << std::endl
		<< "Mask: " << imageMaskPair.second << std::endl
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }

     

    ImageType::Pointer image( imageReader->GetOutput() );

    if ( nSamples == 0 ) {
      IteratorType
	maskIter( maskReader->GetOutput(),
		  maskReader->GetOutput()->GetLargestPossibleRegion() );
      for ( maskIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter ) {
	if ( maskIter.Get() ) {
	  auto sample = image->GetPixel( maskIter.GetIndex() );
	  if ( sample < min ) {
	    min = sample;
	  }
	  else if ( sample > max ) {
	    max = sample;
	  }
	  samples.push_back( sample );
	}
      }
    }
    else {
      RandomIteratorType
	maskIter( maskReader->GetOutput(),
		  maskReader->GetOutput()->GetLargestPossibleRegion() );
	
      maskIter.SetNumberOfSamples( nSamples );
      unsigned int nSampled = 0;
      while ( nSampled < nSamples ) {
	for ( maskIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter ) {
	  if ( maskIter.Get() ) {
	    auto sample = image->GetPixel( maskIter.GetIndex() );
	    if ( sample < min ) {
	      min = sample;
	    }
	    else if ( sample > max ) {
	      max = sample;
	    }
	    samples.push_back( sample );
	    if ( ++nSampled == nSamples ) {
	      break;
	    }
	  }
	}
      }
    }
  }

  // Now we rescale from [min, max] to [0,10*nBins] and discretize to integers
  //
  const unsigned int RANGE_MAX = 10*nBins;
  auto width = max - min;
  std::vector< unsigned int > histogram(RANGE_MAX + 1);
  for ( auto sample : samples ) {
    // This only works if we can trust that
    //   std::ceil((max - min)/(max - min)) == 1
    // see DivisionTest which provides evidence that we can trust it.
    //++histogram.at( std::ceil( ((sample - min)/width) * (RANGE_MAX + 1) )  - 1 );
    auto bin = std::round( ((sample - min)/width) * RANGE_MAX );
    ++histogram.at( static_cast< unsigned int >( bin ) );
  }  
  PixelType massPerBin = samples.size() / nBins;
  std::cout << "Max : " << max << std::endl
	    << "Min : " << min << std::endl
	    << "Width : " << width << std::endl
	    << "MassPerBin : " << massPerBin << std::endl;
  samples.clear();

  std::vector< PixelType > edges(nBins-1);
  unsigned int j = 0;
  unsigned int mass = 0;
  // Now we just find the integer edges and then transform back
  // TODO : Something better 
  for ( unsigned int i = 1; i <= edges.size(); ++i ) {
    while ( j < histogram.size() && mass < i * massPerBin ) {
      mass += histogram[j++];
    }
    edges[i-1] = min + (width * j) / static_cast<PixelType>(histogram.size());
  }


  // Store the edges
  std::string fileName = prefix + timestamp() +  OUT_FILE_TYPE;
  std::string outPath( Path<char>::Join( outDirPath, fileName ) );
  std::ofstream out( outPath );
  writeSequenceAsText< char > ( out, edges.begin(), edges.end() );
  if ( !out.good() ) {
    std::cerr << "Error writing to edges to file." << std::endl
	      << "Out path: " << outPath << std::endl;
    return EXIT_FAILURE;
  }  

  return EXIT_SUCCESS;
}
     
