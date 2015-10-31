/* 
*/
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "tclap/CmdLine.h"

#include "itkClampImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToHistogramFilter.h"
#include "itkMaskedImageToHistogramFilter.h"
#include "itkJoinImageFilter.h"
#include "itkHistogramToIntensityImageFilter.h"
#include "itkHistogramToProbabilityImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkClampImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkRGBPixel.h"

const std::string VERSION("0.1");


int main(int argc, char *argv[]) {
  // Commandline parsing
  TCLAP::CmdLine cmd("Calculate joint histogram for two images.", ' ', VERSION);

  TCLAP::ValueArg<std::string> 
    image1Arg("i", 
	     "image-1", 
	     "Path to first image.",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<std::string> 
    image2Arg("I", 
	     "image-2", 
	     "Path to second image.",
	     true,
	     "",
	     "path", 
	     cmd);

  TCLAP::ValueArg<std::string> 
    maskArg("m", 
	    "mask", 
	    "Path to mask image.",
	    false,
	    "",
	    "path", 
	    cmd);

  
  TCLAP::ValueArg<std::string> 
    outArg("o", 
	   "out", 
	   "Output filename",
	   true, 
	   "", 
	   "path", 
	   cmd);
  

  TCLAP::ValueArg<unsigned int> 
    nBinsArg("b", 
	     "bins", 
	     "Number of bins to use",
	     false, 
	     255, 
	     "unsigned int", 
	     cmd);

  TCLAP::ValueArg<double> 
    minValueArg("l", 
		"lower-bound", 
		"Smallest value to include in histogram. Determined automatically if not set.",
		false, 
		NAN, 
		"real number", 
		cmd);

  TCLAP::ValueArg<double> 
    maxValueArg("u",
		"upper-bound", 
		"Largest value to include in histogram. Determined automatically if not set.",
		false, 
		NAN, 
		"real number", 
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
  const std::string maskPath( maskArg.getValue() );
  const std::string outPath( outArg.getValue() );
  const unsigned int nBins( nBinsArg.getValue() );
  const double minValue( minValueArg.getValue() );
  const double maxValue( maxValueArg.getValue() );
  //// Commandline parsing is done ////

  const bool useMask = ! maskPath.empty();
  
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
  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( image1Path );
  
  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( image2Path );

  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName( maskPath );

  // Hack to get join filter to accept different origin/spacing setting
  try {
    reader1->Update();
    reader2->Update();
    if ( useMask ) {
      maskReader->Update();
    }
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Error updating readers." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  
  reader2->GetOutput()->SetOrigin( reader1->GetOutput()->GetOrigin() );
  reader2->GetOutput()->SetSpacing( reader1->GetOutput()->GetSpacing() );
  if ( useMask ) {
    maskReader->GetOutput()->SetOrigin( reader1->GetOutput()->GetOrigin() );
    maskReader->GetOutput()->SetSpacing( reader1->GetOutput()->GetSpacing() );
  }
  
  // Setup the join filter
  typedef itk::JoinImageFilter< ImageType, ImageType > JoinFilterType;
  JoinFilterType::Pointer joinFilter = JoinFilterType::New();
  joinFilter->SetInput1( reader1->GetOutput() );
  joinFilter->SetInput2( reader2->GetOutput() );


  // Setup up the clamp filter
  typedef itk::ClampImageFilter< MaskImageType, MaskImageType> ClampFilterType;
  ClampFilterType::Pointer clampFilter = ClampFilterType::New();
  clampFilter->SetInput( maskReader->GetOutput() );
  clampFilter->SetBounds(0,1);
  

  // Setup the histogram filter
  typedef typename JoinFilterType::OutputImageType JoinImageType;
  typedef itk::Statistics::ImageToHistogramFilter< JoinImageType > HistogramFilterType;
  typedef typename HistogramFilterType::HistogramSizeType HistogramSizeType;
  typedef itk::Statistics::MaskedImageToHistogramFilter<
    JoinImageType,
    MaskImageType > MaskedHistogramFilterType;
  typedef HistogramFilterType::HistogramMeasurementVectorType
    HistogramMeasurementVectorType;
  
  typedef typename HistogramFilterType::HistogramType HistogramType;
  HistogramType::Pointer histogram;

  HistogramFilterType::Pointer histogramFilter;  
  if ( useMask ) {
    MaskedHistogramFilterType::Pointer maskedHistogramFilter = MaskedHistogramFilterType::New();    
    maskedHistogramFilter->SetMaskImage( clampFilter->GetOutput() );
    maskedHistogramFilter->SetMaskValue( 1 );
    histogramFilter = maskedHistogramFilter;
  }
  else {
    histogramFilter = HistogramFilterType::New();
  }
  
  histogramFilter->SetInput( joinFilter->GetOutput() );
  HistogramSizeType histogramSize(2);
  histogramSize.Fill( nBins );
  histogramFilter->SetHistogramSize( histogramSize );
  if ( std::isnan(minValue) || std::isnan(maxValue) ) {
    std::cout << "Setting min/max values of histogram automatically"
	      << std::endl;
    histogramFilter->SetAutoMinimumMaximum( true );
  }
  else {
    HistogramMeasurementVectorType binMinimum( 2 );
    HistogramMeasurementVectorType binMaximum( 2 );
    binMinimum.Fill( minValue );
    binMaximum.Fill( maxValue );
    histogramFilter->SetHistogramBinMinimum( binMinimum );
    histogramFilter->SetHistogramBinMaximum( binMaximum );
  }
    
  try {
    histogramFilter->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Error updating masked histogram filter." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  histogram = histogramFilter->GetOutput();

  typedef typename HistogramType::ConstIterator ConstIteratorType;
  std::ofstream out( outPath );
  // Writer a header
  out << "BinCenters ";
  size_t i = 0;
  for ( ConstIteratorType iter = histogram->Begin();
	iter != histogram->End();
	++iter ) {
    out << '"' << iter.GetMeasurementVector()[0] << '"';
    if ( ++i == nBins ) {
      out << '\n';
      break;
    }
    else {
      out << ' ';
    }
  }
  
  i = 0;
  for ( ConstIteratorType iter = histogram->Begin();
	iter != histogram->End();
	++iter ) {
    if ( i == 0 ) {
      out << '"' << iter.GetMeasurementVector()[1] << "\" ";
    }
    out << iter.GetFrequency();
    if ( ++i == nBins ) {
      out << '\n';
      i = 0;
    }
    else {
      out << ' ';
    }
  }

  return EXIT_SUCCESS;
}     
