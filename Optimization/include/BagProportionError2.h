#ifndef __BagProportionError2_h
#define __BagProportionError2_h

template< typename TVector, typename >
struct BagProportionError2 {
  typedef TVector VectorType;
  typedef double ResultType;

  BagProportionError2() : m_P() {}
  
  BagProportionError2( const VectorType& p )
    : m_P( p )
  {}
  
  ResultType
  operator()( const VectorType& y) {
    VectorType diff = m_P - y;
    ResultType error = diff.squaredNorm();    
    return error;
  }

  ResultType
  operator()( const VectorType& p, const VectorType& y) {
    VectorType diff = p - y;
    ResultType error = diff.squaredNorm();    
    return error;
  }

  
private:
  VectorType m_P;  
};
  


#endif
