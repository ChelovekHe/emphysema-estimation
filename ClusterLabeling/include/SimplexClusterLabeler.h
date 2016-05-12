#ifndef __SimplexClusterLabeler_h
#define __SimplexClusterLabeler_h

#include <unordered_set>

#include "ceres/ceres.h"

#include "SimplexClusterLabelerParameters.h"

/*
  Find optimal k-simplex labelling of K clusters of N Bags.
  Let 
     x \in {[0,1]^k}^K, with x_{i,j} >= 0, forall i,j and sum_j^k x_{i,j} = 1, forall i
     p \in Y^N
     C \in R^{N \times K}, with C_{i,j} >= 0, forall i,j and sum_j C_{i,j} = 1, forall i.
     R : Y x Y' -> \mathbb{R}_+

  x is an unkown labelling of clusters we want to find
  p is a known labelling of bags and Y the label space
  C maps cluster labels to a bag label space Y'
  R is a risk function

  The reason Y and Y' are not necesarily the same is that Y might be a 
  combination of intervals and categories, while Y' is a simplex that can be 
  used to derive Y-type labels.

  TRisk must define
    double operator()( const BagLabelVectorType& bagLabels, const ClusterLabelVectorType& clusterLabels)
  which calculates the risk when assigning clusterLabels given bagLabels

*/
template< typename TRisk, size_t BagLabelDim=1, InstanceLabelDim=2 >
class SimplexClusterLabeler
{
public:
  typedef TRisk RiskType;
  typedef SimplexClusterLabeler< RiskType > Self;

  typedef BaggedDataset< BagLabelDim, InstanceLabelDim > BaggedDatasetType;
  
  typedef typename BaggedDatasetType::MatrixType MatrixType;
  typedef typename BaggedDatasetType::BagLabelVectorType BagLabelVectorType;
  typedef typename BaggedDatasetType::InstanceLabelVectorType ClusterLabelVectorType;
  

  typedef SimplexClusterLabelerParameters ParameterType;
  
  SimplexClusterLabeler( ParameterType& params )
    : m_Params( params )
  {}
  
  ~SimplexClusterLabeler() {}
    
  /*
    @param bags           Set of bags including their known labels
    @param clusterBagMap  Mapping from cluster labels to bag labels
    @param labeling       Final labeling

    @return   Objective value at best cluster labeling
   */
  double Label( const BaggedDatasetType& bags,
		const MatrixType& clusterBagMap,
		ClusterLabelVectorType& labeling ) {

    if ( clusterBagMap.cols() != labeling.rows() ) {
      throw std::invalid_argument("Size of clusterBagMap and clustering does not match");
    }

    if ( clusterBagMap.rows() != bags.NumberOfBags() ) {
      throw std::invalid_argument( "Size of clusterBagMap and NumberOfBags dows not match" );
    }

    RiskType* risk = new RiskType( bags.BagLabels(), clusterBagMap );

    // We need to make ceres think that we have a 1D array of doubles, when we
    // actually have an NxD matrix representing N D-dimensional simplexes
    // We just pass the matrix as an array and handle the simplex constraint in
    // the risk function
    std::vector< double* > params;
    params.push_back( labeling.data() );

    ceres::Problem problem;
    problem.AddResidualBlock( risk, NULL, params );

    // All variables are bounded to lie in [0,1]
    for ( std::size_t i = 0; i < labeling.size(); ++i ) {
      problem.SetParameterLowerBound( params[0], i, 0);
      problem.SetParameterUpperBound( params[0], i, 1);
    }

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


 private:
  ParameterType m_Params;
  
};

#endif
