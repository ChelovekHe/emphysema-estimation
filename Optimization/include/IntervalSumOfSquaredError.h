#ifndef __IntervalSumOfSquaredError_h
#define __IntervalSumOfSquaredError_h

template< typename TVector, typename TMatrix>
struct IntervalSumOfSquaredError {
  typedef TVector VectorType;
  typedef TMatrix MatrixType;
  typedef double ResultType;

  IntervalSumOfSquaredError()
    : m_I()
  {}
  
  IntervalSumOfSquaredError( const MatrixType& I )
    : m_I( I )
  {
    assert( I.cols() == 2 );
  }
  
  ResultType
  operator()( const VectorType& y) {
    assert( y.size() == m_I.rows() );
    ResultType sae();
    for ( std::size_t i = 0; i < y.size(); ++i ) {
      if ( y(i) < m_I(i,0) ) {
	auto diff = m_I(i,0) - y(i);
	sae += diff*diff;
      }
      else if ( y(i) > m_I(i,1) ) {
	auto diff = y(i) - m_I(i,1);
	sae += diff*diff;
      }
    }    
    return sae;
  }

  ResultType
  operator()( const MatrixType& I, const VectorType& y) {
    assert( y.size() == I.rows() );
    assert( I.cols() == 2 );
    ResultType sae();
    for ( std::size_t i = 0; i < y.size(); ++i ) {
      if ( y(i) < I(i,0) ) {
	auto diff = I(i,0) - y(i);
	sae += diff*diff;
      }
      else if ( y(i) > I(i,1) ) {
	auto diff = y(i) - I(i,1);
	sae += diff*diff;
      }
    }    
    return sae;
  }
  
private:
  MatrixType m_I;
};
#endif
