cmake_minimum_required( VERSION 2.8.6 FATAL_ERROR )

# We want C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DHAVE_GLOG" CACHE STRING "Flags used by the compiler during all build types")

project( EmphysemaEstimation )


# Help cmake find itk
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
include_directories( ClusterLabeling/include )
include_directories( Common/include )
include_directories( Distances/include )
include_directories( FeatureExtraction/include )
include_directories( Filtering/include )
include_directories( IO/include )
include_directories( Losses/include )
include_directories( Models/include )
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


