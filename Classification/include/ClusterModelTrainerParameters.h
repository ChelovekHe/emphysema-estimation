#ifndef __ClusterModelTrainerParameters_h
#define __ClusterModelTrainerParameters_h

struct ClusterModelTrainerParameters {
  /*
    Parameters for cluster model training

    Default values makes values be automatically decided

    @param k       	 Number of clusters
    @param maxIterations Maximum number of iterations
    @param sigma   	 Initial step size
    @param lambda  	 Initial population size
    @param seed    	 Seed for random generator
  */
  ClusterModelTrainerParameters( const std::size_t k,
				 const int& maxIterations = 0,
				 const double& sigma = -1,
				 const int& lambda = -1,
				 const uint64_t seed = 0 )
    : k( k ),
      maxIterations( maxIterations ),
      sigma( sigma ),
      lambda( lambda ),
      seed( seed )
  {}

  const std::size_t k;
  const int& maxIterations;
  const double& sigma;
  const int& lambda;
  const uint64_t seed;
};

#endif
