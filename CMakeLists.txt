cmake_minimum_required( VERSION 2.8.6 FATAL_ERROR )

project( EmphysemaEstimation )

# We want C++11
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

# Help cmake find itk
set( CMAKE_PREFIX_PATH "External/ITK-build" )
find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

find_package( Ceres REQUIRED PATHS "External/" )
include_directories( ${CERES_INCLUDE_DIRS} )


# Set variable for libcmaes
set( CMAES_LIBRARIES ${CMAKE_SOURCE_DIR}/External/lib/libcmaes.so.0 )
include_directories( External/include )

enable_testing( )

include_directories( Classification/include )
include_directories( Clustering/include )
include_directories( Common/include )
include_directories( Distances/include )
include_directories( External/eigen-eigen-c58038c56923 )
include_directories( External/flann-1.8.4-src/src/cpp )
include_directories( External/googletest-release-1.7.0/include )
include_directories( External/tclap-1.2.1/include )
include_directories( FeatureExtraction/include )
include_directories( Filtering/include )
include_directories( IO/include )
include_directories( Numerics/include )
include_directories( Optimization/include )
include_directories( ROIExtraction/include )
include_directories( Statistics/include )
include_directories( Util/include )


include_directories( /usr/include/hdf5/serial )
#include_directories( /usr/include/hdf5/mpi )

add_subdirectory( Classification )
add_subdirectory( Clustering )
add_subdirectory( Experiments )
add_subdirectory( External )
add_subdirectory( FeatureExtraction )
add_subdirectory( Filtering )
add_subdirectory( IO )
add_subdirectory( Optimization )
add_subdirectory( ROIExtraction )
add_subdirectory( Scripts )
#add_subdirectory( Simulation )
add_subdirectory( Statistics )
add_subdirectory( Util )
add_subdirectory( Test )