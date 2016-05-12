#ifndef __CMSTrainerParameters_h
#define __CMSTrainerParameters_h

struct CMSTrainerParameters {
  /*
    Parameters for cluster model training

    Default values makes values be automatically decided

    @param k       	 Number of clusters
    @param maxIterations Maximum number of iterations
    @param out           Path to output files
    @param sigma   	 Initial step size
    @param lambda  	 Initial population size
    @param seed    	 Seed for random generator
  */
  CMSTrainerParameters( std::size_t k = 1,
			int maxIterations = 0,
			std::string out = "",
			double sigma = 0.5,
			int lambda = -1,
			uint64_t seed = 0,
			bool trace = false,
			std::size_t finalNumberOfClusterings=10 )
    : k( k ),
      maxIterations( maxIterations ),
      out( out ),
      sigma( sigma ),
      lambda( lambda ),
      seed( seed ),
      trace( trace ),
      finalNumberOfClusterings( finalNumberOfClusterings )
  {}

  const std::size_t k;
  const int maxIterations;
  const std::string out;
  const double sigma;
  const int lambda;
  const uint64_t seed;
  const bool trace;
  const std::size_t finalNumberOfClusterings;
};

#endif
