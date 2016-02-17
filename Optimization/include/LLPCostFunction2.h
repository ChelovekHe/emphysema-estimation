#ifndef __LLPCostFunction2_h
#define __LLPCostFunction2_h

#include "ceres/ceres.h"
/**
   Cost function implementing
   r_i(x) = p_i - (Cx)_i

   See: ClusterLabeller.h
*/
template< typename TVector, typename TMatrix >
class LLPCostFunction2 : public ceres::CostFunction {
public:
  typedef TVector VectorType;
  typedef TMatrix MatrixType;

  /** 
     @param p \in [0,1]^N is the known bag label proportions
     @param C \in R^{N \times K} is a mapping from cluster labels to bag labels
  */
  LLPCostFunction2(VectorType p, MatrixType C)
    : m_P(p), m_C(C)
  {
    assert( p.size() == C.rows() );

    // We have K parameters
    auto* sizes = mutable_parameter_block_sizes();
    sizes->push_back( C.cols() );

    // We have N residuals.
    set_num_residuals( static_cast<int>(p.size()) );
  }

  virtual ~LLPCostFunction2() {}

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
    // We need to calculate p - Cx

    // We have one parameter block, with K parameters
    double const* x = parameters[0];
      
    const std::size_t N = m_C.rows();
    const std::size_t K = m_C.cols();
    for ( std::size_t i = 0; i < N; ++i ) {
      // Compute y = (C*x)_i
      double y = 0; 
      for ( std::size_t j = 0; j < K; ++j ) {
	y += m_C(i,j)* x[j]; 
      }
      residuals[i] = m_P(i) - y;
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
};

#endif
