#ifndef __LLPCostFunction_h
#define __LLPCostFunction_h

#include "ceres/ceres.h"
/**
   Cost function implementing
   r_i(x) = p_i - (Cx)_i
   r_{N+1} = sum_i r_i(x)

   See: ClusterLabeller.h
*/
template< typename TVector, typename TMatrix >
class LLPCostFunction : public ceres::CostFunction {
public:
  typedef TVector VectorType;
  typedef TMatrix MatrixType;

  /** 
     @param p \in [0,1]^N is the known bag label proportions
     @param C \in R^{N \times K} is a mapping from cluster labels to bag labels
  */
  LLPCostFunction(VectorType p, MatrixType C)
    : m_P(p), m_C(C)
  {
    assert( p.size() == C.rows() );

    // We have K parameters
    auto* sizes = mutable_parameter_block_sizes();
    sizes->push_back( C.cols() );

    // We have N + 1 residuals.
    set_num_residuals( static_cast<int>(p.size()) + 1 );
  }

  virtual ~LLPCostFunction() {}

  /**
     @parameters Array of length 1 containing a pointer to an array of K 
                 parameters.
     @residuals  Array of length N+1.
     @jacobians  Array of length 1 containing a pointer to an array of N+1 
                 Jacobian vectors of length K.
   */
  bool Evaluate(double const* const* parameters,
		double* residuals,
		double** jacobians) const {
    // We need to calculate p - Cx and sum_i (p - Cx)_i

    // We have one parameter block, with K parameters
    double const* x = parameters[0];
      
    const std::size_t N = m_C.rows();
    const std::size_t K = m_C.cols();
    residuals[N] = 0;
    for ( std::size_t i = 0; i < N; ++i ) {
      // Compute y = (C*x)_i
      double y = 0; 
      for ( std::size_t j = 0; j < K; ++j ) {
	y += m_C(i,j)* x[j]; 
      }
      residuals[i] = m_P(i) - y;
      residuals[N] += m_P(i) - y;
    }

    if ( jacobians != NULL && jacobians[0] != NULL ) {
      // Compute the Jacobian

      // We have only one parameter block.
      double * J = jacobians[0];
      for ( std::size_t i = 0; i < N; ++i ) {
	for ( std::size_t j = 0; j < K; ++j ) {
	  // Jacobian for residual i and parameters j
	  J[i * K + j] = - m_C(i,j);

	  // Add to the derivative of the last term, which is the sum of all the derivatives.
	  if ( i == 0 ) {
	    J[N * K + j] = - m_C(i,j);
	  }
	  else {
	    J[N * K + j] -= m_C(i,j);
	  }
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
