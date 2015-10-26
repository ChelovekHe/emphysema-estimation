
#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

#include "EigenvalueFeaturesFunctor.h"
#include "Hessian3DImageFilter.h"
#include "Path.h"

const std::string OUT_FILE_TYPE(".nii.gz");

int main( int argc, char* argv[] ) {
  if( argc < 3 ) {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }

  typedef float PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::VectorImage< PixelType, Dimension >  VectorImageType;

  // Setup the reader
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  // Setup the Hesian filter
  typedef itk::Hessian3DImageFilter< ImageType, VectorImageType >
    HessianFilterType;
  HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
  hessianFilter->SetInput( reader->GetOutput() );

  // Setup the eigenvalue feature filter
  typedef EigenvalueFeaturesFunctor< PixelType > FunctorType;
  typedef itk::UnaryFunctorImageFilter< VectorImageType,
					VectorImageType,
					FunctorType >
    EigenvalueFilterType;

  EigenvalueFilterType::Pointer eigenvalueFilter = EigenvalueFilterType::New();
  FunctorType eigenvalueFeatures;
  eigenvalueFilter->SetInput( hessianFilter->GetOutput() );
  eigenvalueFilter->SetFunctor( eigenvalueFeatures );

  
  typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ImageType>
    IndexSelectionType;
  IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
  indexSelectionFilter->SetInput( eigenvalueFilter->GetOutput() );
  
  // Setup the writer
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetInput( indexSelectionFilter->GetOutput() );

  std::vector< std::string > featureNames{
    "eig1", "eig2", "eig3", "LoG", "Curvature", "Frobenius"
      };
  
  for (unsigned int i = 0; i < featureNames.size(); ++i ) {
    indexSelectionFilter->SetIndex( i );
    std::string outFile = Path::join( argv[2], featureNames[i] + OUT_FILE_TYPE );
    writer->SetFileName( outFile );
    try {
      writer->Update();
    }
    catch ( itk::ExceptionObject &e ) {
      std::cerr << "Failed to process." << std::endl
  		<< "Image: " << argv[1] << std::endl
  		<< "Out: " << argv[2] << std::endl
  		<< "ExceptionObject: " << e << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
