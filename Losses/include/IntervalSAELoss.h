#ifndef __IntervalSAELoss_h
#define __IntervalSAELoss_h


template< typename TLabel, typename TVector >
struct IntervalSAELoss {
  typedef IntervalLabel< TNumeric > LabelType;
  typedef TVector VectorType;
  
  IntervalSAELoss( const std::vector< LabelType >& knownIntervalLabels )
    : m_I( knownIntervalLabels )
  {}
  
  double operator()( const VectorType& predictedLabels) {
    assert( m_I.size() == predictedLabels.size() );
    double sae = 0;
    for ( std::size_t i = 0; i < m_I.size(); ++i ) {
      sae += m_I[i].L1( predictedLabels(i) );
    }    
    return sae;
  }
  
private:
  MatrixType m_I;
};
  


#endif
