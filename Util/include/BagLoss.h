#ifndef __BagLoss_h
#define __BagLoss_h

#include <vector>

#include "flann/flann.hpp"

/*
  Bag loss functions. See:
  [1] Stolpe, Marco and Morik, Katharina
      Learning from Label Proportions by Optimizing Cluster Model Selection
      2011

*/

/*
  Equation (5) from [1]
*/
template< typename MatrixType >
typename MatrixType::ElementType
bagLoss( const MatrixType& bagProportions,
	 const MatrixType& modelProportions,
	 const MatrixType& sizeProportions ) {
  assert( bagProportions.rows == modelProportions.rows );
  assert( bagProportions.cols == modelProportions.cols );
  assert( bagProportions.rows == sizeProportions.rows );
  assert( sizeProportions.cols >= 1 );

  auto w = bagLossWeight( bagProportions, modelProportions, sizeProportions );
  auto p = bagLossPrior( bagProportions, modelProportions );
  return std::sqrt( w*p );
}


/*
  Equation (6) from [1]
*/
template< typename MatrixType >
typename MatrixType::type
bagLossWeight( const MatrixType& bagProportions,
	       const MatrixType& modelProportions,
	       const MatrixType& sizeProportions ) {
  typedef typename MatrixType::type value_type;  
  const size_t h = bagProportions.rows;
  const size_t l = bagProportions.cols;
  value_type result = 0;
  for ( size_t i = 0; i < h; ++i ) {
    for (size_t j = 0; j < l; ++j ) {
      auto diff = bagProportions[i][j] - modelProportions[i][j];
      auto relativeSize =
	labelProportion( bagProportions, j ) * sizeProportions[i][0];
      result += relativeSize * (diff*diff);
    }
  }
  return result / (h*l);
}

/*
  Equation (7) from [1]
*/
template< typename MatrixType >
typename MatrixType::type
bagLossPrior( const MatrixType& bagProportions,
	      const MatrixType& modelProportions ) {
  typedef typename MatrixType::type value_type;  
  const size_t l = bagProportions.cols;
  value_type result = 0;
  for (size_t j = 0; j < l; ++j ) {
    auto diff = labelProportion( bagProportions, j ) -
      labelProportion( modelProportions, j );
    result += diff*diff;
  }
  return result / l;  
}

/*
  Calculate equation (2) from [1]
  $\eta(\Pi, y_i) = \frac{1}{n}\sum\limits_{i=1}^h |G_i| \cdot \pi_{ij}
  which gives us the proportion of label y_i over the entire sample.
*/
template< typename MatrixType >
typename MatrixType::type
labelProportion( const MatrixType& proportions,
		 const MatrixType& bagProportionalSizes,
		 size_t label ) {
  typedef typename MatrixType::type value_type;
  //  value_type N = 0;
  value_type result = 0;
  for ( size_t i = 0; i < proportions.rows; ++i ) {
    result += proportions[i][label] * bagProportionalSizes[i];
    //    N += bagSizes[i];
  }
  return result;///N;
}


#endif
