
#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "ImageToEmphysemaFeaturesFilter.h"
#include "Path.h"

const std::string OUT_FILE_TYPE(".nii.gz");

int main( int argc, char* argv[] ) {
  if( argc < 4 ) {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  inpuMaskFile  outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  typedef float PixelType;
  typedef unsigned char MaskPixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::Image< MaskPixelType, Dimension > MaskType;
  typedef itk::VectorImage< PixelType, Dimension >  VectorImageType;
  

  // Setup the reader
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::ImageFileReader< MaskType > MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( argv[2] );

  // Setup the feature filter
  typedef itk::ImageToEmphysemaFeaturesFilter<
    ImageType,
    MaskType,
    VectorImageType > FeatureFilterType;
  FeatureFilterType::Pointer featureFilter = FeatureFilterType::New();
  featureFilter->SetInputImage( reader->GetOutput() );
  featureFilter->SetInputMask( maskReader->GetOutput() );

  
  typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ImageType>
    IndexSelectionType;
  IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
  indexSelectionFilter->SetInput( featureFilter->GetOutput() );
  
  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( indexSelectionFilter->GetOutput() );

  std::vector< std::string > featureNames{
    "intensity", "GradientMagnitude",
      "eig1", "eig2", "eig3",
      "LoG", "Curvature", "Frobenius"
      };
  
  for (unsigned int i = 0; i < featureNames.size(); ++i ) {
    indexSelectionFilter->SetIndex( i );
    std::string outFile = Path::join( argv[3], featureNames[i] + OUT_FILE_TYPE );
    writer->SetFileName( outFile );
    try {
      writer->Update();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to process." << std::endl
  		<< "Image: " << argv[1] << std::endl
		<< "Mask: " << argv[2] << std::endl
  		<< "Out: " << argv[3] << std::endl
  		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
