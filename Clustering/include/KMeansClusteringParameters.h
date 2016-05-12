#ifndef __KMeansClusteringParameters_h
#define __KMeansClusteringParameters_h

#include "flann/flann.hpp"

struct KMeansClusteringParameters {
  KMeansClusteringParameters( int k=1,
			      int branching=2,
			      int iterations=25,
			      float cbIndex=0.2,
			      flann::flann_centers_init_t centersInit=flann::FLANN_CENTERS_KMEANSPP )
  : k( k )
  , branching( branching )
  , iterations( iterations )
  , cbIndex( cbIndex )
  , centersInit( centersInit )
  {}
    
  int k;
  int branching;
  int iterations;
  float cbIndex;
  flann::flann_centers_init_t centersInit;
};

#endif
