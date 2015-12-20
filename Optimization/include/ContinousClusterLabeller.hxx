#ifndef __ContinousClusterLabeller_hxx
#define __ContinousClusterLabeller_hxx

#include "ContinousClusterLabeller.h"
#include "ceres/ceres.h"

template< template<typename, typename> class TCostFunctor >
double
ContinousClusterLabeller< TCostFunctor >
::label( const VectorType& p,
	 const MatrixType& C,
	 VectorType& x ) const {

  if ( C.cols() != x.size() ) {
    throw std::invalid_argument("Size of C and x must match");
  }

  if ( C.rows() != p.size() ) {
    std::string msg = "Size of p and C must match";
    std::cerr << msg << std::endl
	      << p.size() << " " << C.rows() << std::endl;
    throw std::invalid_argument( msg );
  }

  CostFunctorType* costFunction = new CostFunctorType(p, C);

  // Create the parameter group
  std::vector< double* > params;
  params.push_back(x.data());

  // Setup the problem
  ceres::Problem problem;
  problem.AddResidualBlock( costFunction, NULL, params );
  for ( std::size_t i = 0; i < x.size(); ++i ) {
    problem.SetParameterLowerBound( params[0], i, 0);
    problem.SetParameterUpperBound( params[0], i, 1);
  }

  // Solve the problem
  ceres::Solver::Options options;
  options.max_num_iterations = 150;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if ( summary.IsSolutionUsable() ) {
    return summary.final_cost;
  }

  // Improve on this
  std::cerr << "Solution is not usable!" << std::endl
	    << summary.BriefReport() << std::endl;
  return std::numeric_limits<double>::infinity();  
}

#endif
