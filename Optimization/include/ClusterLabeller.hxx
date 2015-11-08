#ifndef __ClusterLabeller_hxx
#define __ClusterLabeller_hxx

#include "ceres/ceres.h"
#include "LLPCostFunction.h"

template< typename TMatrix, typename TVector >
ClusterLabeller< TMatrix, TVector >
::ClusterLabeller( const TVector& p )
  : m_P(p)
{ }


// Ceres works with doubles and expects a vector of double pointers as parameters
// If x is not of type double, then we will get a problem when we make pointers
// to x.
template<>
double
ClusterLabeller<
  Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor >,
  Eigen::Matrix< double, Eigen::Dynamic, 1 >
  >
::findOptimalLabels( const MatrixType& C, VectorType& x ) const {
  // Check that size match
  if ( C.cols() != x.size() ) {
    throw std::invalid_argument("Size of C and x must match");
  }

  if ( C.rows() != m_P.size() ) {
    throw std::invalid_argument("Size of p and C must match" );
  }

  VectorType initialX = x;
  typedef LLPCostFunction<VectorType, MatrixType> CostFunctionType;
  CostFunctionType* costFunction = new CostFunctionType(m_P, C);

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
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if ( summary.IsSolutionUsable() ) {
    return summary.final_cost;
  }

  std::cerr << "Solution is not usable!" << std::endl
	    << summary.BriefReport() << std::endl;
  return std::numeric_limits<double>::infinity();
}

#endif
