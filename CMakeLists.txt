cmake_minimum_required( VERSION 2.8.6 FATAL_ERROR )

project( EmphysemaEstimation )

# We want C++11
set( CMAKE_CXX_FLAGS "-std=c++11" )

# Help cmake find itk
set( ITK_DIR "External/ITK-build" )
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

include_directories( External/tclap-1.2.1/include )
include_directories( Util/include )

add_subdirectory( Clustering )
add_subdirectory( Experiments )
add_subdirectory( External )
add_subdirectory( FeatureExtraction )
add_subdirectory( ROIExtraction )
add_subdirectory( Scripts )
#add_subdirectory( Simulation )
add_subdirectory( Statistics )
add_subdirectory( Util )