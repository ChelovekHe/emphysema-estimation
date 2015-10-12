cmake_minimum_required( VERSION 2.8.6 FATAL_ERROR )

project( EmphysemaEstimation )

# We want C++11
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

# Help cmake find itk
set( CMAKE_PREFIX_PATH "External/ITK-build" )
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

include_directories( Classification/include )
include_directories( Clustering/include )
include_directories( External/tclap-1.2.1/include )
include_directories( External/flann-1.8.4-src/src/cpp )
include_directories( Statistics/include )
include_directories( Util/include )

include_directories( /usr/include/hdf5/serial )
#include_directories( /usr/include/hdf5/mpi )

add_subdirectory( Clustering )
add_subdirectory( Experiments )
add_subdirectory( External )
add_subdirectory( FeatureExtraction )
add_subdirectory( ROIExtraction )
add_subdirectory( Scripts )
#add_subdirectory( Simulation )
add_subdirectory( Statistics )
add_subdirectory( Util )