/* 
   Generate cubic ROIs at random such that the center of the ROIs lies inside
   the mask.
*/
#include <string>
#include <vector>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageRandomConstIteratorWithIndex.h"

#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".txt");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Generate cubic ROIs.", ' ', VERSION);

  // We need a single mask
  TCLAP::ValueArg<std::string> 
    maskArg("m", 
	    "mask", 
	    "Path to mask.",
	    true,
	    "",
	    "path", 
	    cmd);
  
  // We need a directory for storing the ROI info
  TCLAP::ValueArg<std::string> 
    outDirArg("o", 
	      "outdir", 
	      "Path to output directory",
	      true, 
	      "", 
	      "path", 
	      cmd);
  
  // We can use a prefix to generate filename
  TCLAP::ValueArg<std::string> 
    prefixArg("p", 
	      "prefix", 
	      "Prefix to use for output filename",
	      false, 
	      "", 
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
  std::string maskPath( maskArg.getValue() );
  std::string outDirPath( outDirArg.getValue() );
  std::string prefix( prefixArg.getValue() );
  unsigned int nROIs( nROIsArg.getValue() );
  unsigned int roiSize( roiSizeArg.getValue() );

  //// Commandline parsing is done ////


  // Some common values/types that are always used.
  const unsigned int Dimension = 3;

  // TODO: Need to consider which pixeltype to use
  typedef unsigned char MaskPixelType;
  typedef itk::Image< MaskPixelType, Dimension >  MaskImageType;
  typedef itk::ImageFileReader< MaskImageType > MaskReaderType;
    
  // Setup the reader
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( maskPath );

  try {
    maskReader->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to Update mask reader." << std::endl
	      << "Mask: " << maskPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
 
  // Setup the region. We know the size but start index is set in the loop
  assert( roiSize > 0 );
  MaskImageType::SizeType size;
  size.Fill( roiSize );
  MaskImageType::RegionType roi;
  roi.SetSize( size );


  // Setup the random iterator that walks the mask image
  typedef itk::ImageRandomConstIteratorWithIndex< MaskImageType >
    RandomIteratorType;
  RandomIteratorType
    maskIterator( maskReader->GetOutput(),
		  maskReader->GetOutput()->GetLargestPossibleRegion() );
  
  // We want one sample for each ROI
  maskIterator.SetNumberOfSamples( nROIs );

  // We need to store the ROI start index
  std::vector< MaskImageType::IndexType > start;
  start.reserve( nROIs );

  // We need the image size so we can test that all ROIs are inside the image
  auto imageSize =
    maskReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  // Run the ROI sampling loop  
  for ( unsigned int nROI = 0; nROI < nROIs;  ) {
    for ( maskIterator.GoToBegin();
	  !maskIterator.IsAtEnd() && nROI < nROIs;
	  ++maskIterator ) {
      if ( maskIterator.Get() == 1 ) {
	// We try to sample this location. The location is the center of the ROI
	// so we need to offset it by half the size
	MaskImageType::IndexType center = maskIterator.GetIndex();
	center[0] -= size[0]/2;
	center[1] -= size[1]/2;
	center[2] -= size[2]/2;
	if ( center[0] + size[0] < imageSize[0] &&
	     center[1] + size[1] < imageSize[1] &&
	     center[2] + size[2] < imageSize[2] ) {
	  // Now we are happy and choose this as a sample.
	  start.push_back( center );
	  ++nROI;
	}
      }
    }
  }


  // Write the ROI info
  std::string infoFile = Path::join( outDirPath,
				     prefix + "ROIInfo" + OUT_FILE_TYPE );
  std::ofstream out( infoFile );
  if ( out.good() ) {
    out << "ROI, start, size" << std::endl;
  }
  for ( unsigned int i = 0; i < start.size(); ++i ) {
    if ( out.good() ) {
      out << i << ", " << start[i] << ", " << size << std::endl;
    }
  }
  if ( !out.good() ) {
    std::cerr << "Error writing ROI info file" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
