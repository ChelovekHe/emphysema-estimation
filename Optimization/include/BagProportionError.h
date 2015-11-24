#ifndef __BagProportionError_h
#define __BagProportionError_h

template< typename TVector, typename >
struct BagProportionError {
  typedef TVector VectorType;
  typedef double ResultType;

  BagProportionError() : m_P(), m_Lambda(1.0) {}
  
  BagProportionError( const VectorType& p, ResultType lambda=1.0 )
    : m_P( p ),
      m_Lambda( lambda )
  {}
  
  ResultType
  operator()( const VectorType& y) {
    VectorType diff = m_P - y;
    ResultType diffSum = diff.sum();
    ResultType error = diff.squaredNorm() + m_Lambda*diffSum*diffSum;
    
    return error;
  }

  ResultType
  operator()( const VectorType& p, const VectorType& y) {
    VectorType diff = p - y;
    ResultType diffSum = diff.sum();
    ResultType error = diff.squaredNorm() + m_Lambda*diffSum*diffSum;
    
    return error;
  }

  
private:
  VectorType m_P;
  ResultType m_Lambda;
  
};
  


#endif
