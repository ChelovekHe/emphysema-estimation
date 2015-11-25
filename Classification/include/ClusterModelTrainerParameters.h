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
  ClusterModelTrainerParameters( std::size_t k,
				 int branching,
				 int maxIterations = 0,
				 std::string out = "",
				 double sigma = -1,
				 int lambda = -1,
				 uint64_t seed = 0 )
    : k( k ),
      branching( branching ),
      maxIterations( maxIterations ),
      out( out ),
      sigma( sigma ),
      lambda( lambda ),
      seed( seed )
  {}

  const std::size_t k;
  const int branching;
  const int maxIterations;
  const std::string out;
  const double sigma;
  const int lambda;
  const uint64_t seed;
};

#endif
