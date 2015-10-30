/* 
*/
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "tclap/CmdLine.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToHistogramFilter.h"
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
    minValueArg("m", 
		"min", 
		"Smallest value to include in histogram. Determined automatically if not set.",
		false, 
		NAN, 
		"real number", 
		cmd);

  TCLAP::ValueArg<double> 
    maxValueArg("M", 
		"max", 
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
  const std::string outPath( outArg.getValue() );
  const unsigned int nBins( nBinsArg.getValue() );
  const double minValue( minValueArg.getValue() );
  const double maxValue( maxValueArg.getValue() );
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

  // Hack to get join filter to accept different origin/spacing setting
  try {
    reader1->Update();
    reader2->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Error updating readers." << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }
  reader2->GetOutput()->SetOrigin( reader1->GetOutput()->GetOrigin() );
  reader2->GetOutput()->SetSpacing( reader1->GetOutput()->GetSpacing() );

  
  // Setup the join filter
  typedef itk::JoinImageFilter< ImageType, ImageType > JoinFilterType;
  JoinFilterType::Pointer joinFilter = JoinFilterType::New();
  joinFilter->SetInput1( reader1->GetOutput() );
  joinFilter->SetInput2( reader2->GetOutput() );
  

  // Setup the histogram filter
  typedef typename JoinFilterType::OutputImageType JoinImageType;
  typedef itk::Statistics::ImageToHistogramFilter< JoinImageType > HistogramFilterType;
  typedef typename HistogramFilterType::HistogramSizeType HistogramSizeType;
  HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();
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
    typedef HistogramFilterType::HistogramMeasurementVectorType
      HistogramMeasurementVectorType;
    HistogramMeasurementVectorType binMinimum( 2 );
    HistogramMeasurementVectorType binMaximum( 2 );
    binMinimum.Fill( minValue );
    binMaximum.Fill( maxValue );
    histogramFilter->SetHistogramBinMinimum( binMinimum );
    histogramFilter->SetHistogramBinMaximum( binMaximum );
  }


  // Setup the histogram to image filter
  typedef typename HistogramFilterType::HistogramType HistogramType;
  typedef itk::Image<float, 2> FrequencyImageType;

  typedef itk::HistogramToIntensityImageFilter<
    HistogramType,
    FrequencyImageType
    >
    HistogramToImageFilterType;  
  HistogramToImageFilterType::Pointer histogramToImageFilter =
    HistogramToImageFilterType::New();
  histogramToImageFilter->SetInput( histogramFilter->GetOutput() );


  typedef FrequencyImageType OutputImageType;
  //  typedef typename HistogramToImageFilterType::OutputImageType
  //FrequencyImageType;
  // typedef unsigned char OutputPixelType;
  // typedef itk::RGBPixel<OutputPixelType>    RGBPixelType;
  // typedef itk::Image<RGBPixelType, 2>  OutputImageType;
  // typedef itk::ScalarToRGBColormapImageFilter<
  //   FrequencyImageType,
  //   OutputImageType > RGBFilterType;
  // RGBFilterType::Pointer rescaleFilter = RGBFilterType::New();
  // rescaleFilter->SetInput( histogramToImageFilter->GetOutput() );
  // rescaleFilter->SetColormap( RGBFilterType::Hot );
  //rescaleFilter->SetBounds( 0, 255 );
  // rescaleFilter->SetOutputMinimum( 0 );
  // rescaleFilter->SetOutputMaximum(  );
  
  
  // Setup the writer
  typedef itk::ImageFileWriter< OutputImageType > ImageWriter;
  ImageWriter::Pointer writer = ImageWriter::New();
  writer->SetFileName( outPath );
  writer->SetInput( histogramToImageFilter->GetOutput() );

  try {
    writer->Update();
  }
  catch ( itk::ExceptionObject &e ) {
    std::cerr << "Failed to process." << std::endl
	      << "image1Path: " <<  image1Path << std::endl
      	      << "image1Path: " <<  image2Path << std::endl
	      << "outPath: " << outPath << std::endl
	      << "ExceptionObject: " << e << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
     
