#ifndef __SimplexRisk_h
#define __SimplexRisk_h

#include <cassert>
#include "Eigen/Dense"

template< typename TLoss, size_t SimplexDim=2>
class SimplexRisk : public ceres::CostFunction {
  typedef TLoss LossType;
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 2,
			 Eigen::RowMajor > KnownLabelVectorType;

  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 1,
			 Eigen::ColMajor > PredictedLabelVectorType;

  SimplexRisk(const KnownLabelVectorType& bagLabels,
	      const ClusterBagMapType& clusterBagMap,
	      LossType loss=LossType())
    : m_BagLabels( bagLabels )
    , m_ClusterBagMap( clusterBagMap )
    , m_Loss( loss )
  {}
  

  /**
     @parameters Array of length 1 containing a pointer to an array of K 
                 parameters.
     @residuals  Array of length N.
     @jacobians  Array of length 1 containing a pointer to an array of N
                 Jacobian vectors of length K.
   */
  bool Evaluate(double const* const* parameters,
		double* residuals,
		double** jacobians) const {
    const size_t N = m_C.rows();
    const size_t K = m_C.cols();
    const size_t D = SimplexDim;
    
    // We have one parameter block with M parameters, where
    //   M = K * D
    // We need to enforce the simplex constraint
    double const* x = parameters[0];
    SimplexMatrixType simplex( K, D );
    for ( size_t i = 0; i < K; ++i ) {
      // We add 1 to all entries to handle the case where the row sum is zero
      double rowSum = D;
      for ( size_t j = 0; j < D; ++j ) {
	simplex(i,j) = 	[j + i*D];
	rowSum += simplex(i,j);
      }
      for ( size_t j = 0; j < D; ++j ) {
	simplex(i,j) = 	(simplex(i,j) + 1) / rowSum;
      }
    }

    auto predicted = m_ClusterBagMap * simplex;
    for ( size_t i = 0; i < N; ++i ) {
      residuals[i] = m_Loss( m_BagLabel.row(i), predicted.row(i) );
    }    

    if ( jacobians != NULL && jacobians[0] != NULL ) {
      // Compute the Jacobian

      // We have only one parameter block.
      double* J = jacobians[0];
      for ( std::size_t i = 0; i < N; ++i ) {
	for ( std::size_t j = 0; j < K; ++j ) {
	  // Jacobian for residual i and parameters j
	  J[i * K + j] = - m_C(i,j);
	}
      }
    }
      
    return true;
  }
private:
  VectorType m_P;
  MatrixType m_C;
  LossType m_Loss;
};

#endif
