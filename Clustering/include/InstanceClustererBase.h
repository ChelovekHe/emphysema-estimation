#ifndef __InstanceClustererBase_h
#define __InstanceClustererBase_h

#include "BaggedDataset.h"
#include "InstanceClustering.h"

class InstanceClustererBase {
public:
  /**
     Cluster instances in bags using the given distance 
  */
  static InstanceClustering& Cluster( BaggedDataset& bags );

};
