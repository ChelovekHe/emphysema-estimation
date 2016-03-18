#ifndef __IntervalSumOfAbsoluteError_h
#define __IntervalSumOfAbsoluteError_h

template< typename TVector, typename TMatrix>
struct IntervalSumOfAbsoluteError {
  typedef TVector VectorType;
  typedef TMatrix MatrixType;
  typedef double ResultType;

  IntervalSumOfAbsoluteError()
    : m_I()
  {}
  
  IntervalSumOfAbsoluteError( const MatrixType& I )
    : m_I( I )
  {
    assert( I.cols() == 2 );
  }
  
  ResultType
  operator()( const VectorType& y) {
    assert( y.size() == m_I.rows() );
    ResultType sae = 0;
    for ( std::size_t i = 0; i < y.size(); ++i ) {
      if ( y(i) < m_I(i,0) ) {
	sae += m_I(i,0) - y(i);
      }
      else if ( y(i) > m_I(i,1) ) {
	sae += y(i) - m_I(i,1);
      }
    }    
    return sae;
  }

  ResultType
  operator()( const MatrixType& I, const VectorType& y) {
    assert( y.size() == I.rows() );
    assert( I.cols() == 2 );
    ResultType sae = 0;
    for ( std::size_t i = 0; i < y.size(); ++i ) {
      if ( y(i) < I(i,0) ) {
	sae += I(i,0) - y(i);
      }
      else if ( y(i) > I(i,1) ) {
	sae += y(i) - I(i,1);
      }
    }    
    return sae;
  }
  
private:
  MatrixType m_I;
};
  


#endif
