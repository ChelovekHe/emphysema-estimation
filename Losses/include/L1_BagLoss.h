#ifndef __IntervalRisk_h
#define __IntervalRisk_h

template< typename TLoss >
struct IntervalRisk { 
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 2,
			 Eigen::RowMajor > KnownLabelVectorType;

  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 1,
			 Eigen::RowMajor > PredictedLabelVectorType;

  double operator()( LossType& loss,
		     const KnownLabelVectorType& knownLabels,
		     const PredictedLabelVectorType& predictedLabels ) {
    assert( knownLabels.rows() == predictedLabels.rows() );
    size_t rows = std::min( knownLabels.rows(), predictedLabels.rows() );
    double risk = 0;
    for ( std::size_t i = 0; i < rows; ++i ) {
      double low = knownLabels(i, 0);
      double high = knownLabels(i, 1);
      double y = predictedLabels(i);
      if ( y < low ) {
	risk += loss( low, y );
      } else if ( y > high ) {
	risk += loss( high, y );
      }
    }
    return risk / rows;
  }
};  


#endif
