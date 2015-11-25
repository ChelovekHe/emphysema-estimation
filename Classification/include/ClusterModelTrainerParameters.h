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
  ClusterModelTrainerParameters( std::size_t featureSpaceDimension,
				 std::size_t k,
				 int maxIterations = 0,
				 std::string out = "",
				 double sigma = -1,
				 int lambda = -1,
				 uint64_t seed = 0,
				 bool trace = false )
    : featureSpaceDimension( featureSpaceDimension ),
      k( k ),
      maxIterations( maxIterations ),
      out( out ),
      sigma( sigma ),
      lambda( lambda ),
      seed( seed ),
      trace( trace )
  {}

  const std::size_t featureSpaceDimension;
  const std::size_t k;
  const int maxIterations;
  const std::string out;
  const double sigma;
  const int lambda;
  const uint64_t seed;
  const bool trace;
};

#endif
