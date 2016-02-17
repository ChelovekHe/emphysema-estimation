/* 
   Convolve a masked image with a Gaussian using different techniques

   1. Convolution -> Mask

   2. Mask -> Convolution -> Mask

   3. NormalizedConvolution -> Mask

 */
#include <string>
#include <vector>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "NormalizedGaussianConvolutionImageFilter.h"
#include "itkMaskImageFilter.h"

#include "Path.h"

const std::string VERSION("0.1");
const std::string OUT_FILE_TYPE(".nii.gz");

int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Perform convolution using various techniques.", ' ', VERSION);

  // We need a single image
  TCLAP::ValueArg<std::string> 
    imageArg("i", 
	     "image", 
	     "Path to image (T)",
	     true,
	     "",
	     "path", 
	     cmd);

  // We need a single certainty mask
  TCLAP::ValueArg<std::string> 
    certaintyArg("c", 
		 "certainty", 
		 "Path to certainty mask (c). Must match image dimensions.",
		 true,
		 "",
		 "path", 
		 cmd);
  

  // We need scales for the Gauss applicability function
  TCLAP::MultiArg<double> 
    scalesArg("s", 
	      "scale", 
	      "Scales for the Gauss applicability function",
	      true, 
	      "double", 
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
	      "normconv_", 
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
  std::string certaintyPath( certaintyArg.getValue() );
  std::vector<double> scales( scalesArg.getValue() );
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

  ReaderType::Pointer certaintyReader = ReaderType::New();
  certaintyReader->SetFileName( certaintyPath );
  
  // Setup the convolution filters 
  typedef itk::NormalizedGaussianConvolutionImageFilter< ImageType > NormGaussianFilterType;
  typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType > GaussianFilterType;
  typedef itk::MaskImageFilter< ImageType, ImageType, ImageType >  MaskFilterType;  
 
  // Normalized convolution
  NormGaussianFilterType::Pointer blur1 = NormGaussianFilterType::New();
  MaskFilterType::Pointer mask1 = MaskFilterType::New();
  blur1->SetInputImage( imageReader->GetOutput() );
  blur1->SetInputCertainty( certaintyReader->GetOutput() );  
  mask1->SetInput1( blur1->GetOutput() );
  mask1->SetInput2( certaintyReader->GetOutput() );

  
  // // Convolution -> Mask
  // GaussianFilterType::Pointer blur2 = GaussianFilterType::New();
  // MaskFilterType::Pointer mask2 = MaskFilterType::New();
  // blur2->SetInput( imageReader->GetOutput() );
  // mask2->SetInput1( blur2->GetOutput() );
  // mask2->SetInput2( certaintyReader->GetOutput() );


  // // Mask -> Convolution -> Mask
  // GaussianFilterType::Pointer blur3 = GaussianFilterType::New();
  // MaskFilterType::Pointer maskIn = MaskFilterType::New();
  // MaskFilterType::Pointer mask3 = MaskFilterType::New();
  // maskIn->SetInput1( imageReader->GetOutput() );
  // maskIn->SetInput2( certaintyReader->GetOutput() );
  // blur3->SetInput( maskIn->GetOutput() );
  // mask3->SetInput1( blur3->GetOutput() );
  // mask3->SetInput2( certaintyReader->GetOutput() );
  
  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer1 =  WriterType::New();
  //  WriterType::Pointer writer2 =  WriterType::New();
  //  WriterType::Pointer writer3 =  WriterType::New();

  writer1->SetInput( mask1->GetOutput() );
  //  writer2->SetInput( mask2->GetOutput() );
  //  writer3->SetInput( mask3->GetOutput() );

  // Base file name for output images
  std::string baseFileName = Path::join( outDirPath, prefix );
  
  // Do the convolution for each scale
  for (auto scale : scales ) {
    std::cout << "Processing scale " << scale << std::endl;
    blur1->SetSigma(scale);
    //    blur2->SetSigma(scale);
    //    blur3->SetSigma(scale);
    
    // Create a filename
    std::string out1 = baseFileName + "_blur1_scale_" + std::to_string( scale ) +  OUT_FILE_TYPE;
    //    std::string out2 = baseFileName + "_blur2_scale_" + std::to_string( scale ) +  OUT_FILE_TYPE;
    //    std::string out3 = baseFileName + "_blur3_scale_" + std::to_string( scale ) +  OUT_FILE_TYPE;
    writer1->SetFileName( out1 );
    //    writer2->SetFileName( out2 );
    //    writer3->SetFileName( out3 );

    try {
      blur1->Update();
      //      blur2->Update();
      //      blur3->Update();
      
      writer1->Update();
      //      writer2->Update();
      //      writer3->Update();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to process." << std::endl
		<< "Image: " << imagePath << std::endl
		<< "Certainty: " << certaintyPath << std::endl
		<< "Scale: " << scale << std::endl
		<< "Base file name: " << baseFileName << std::endl
		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
