#ifndef __Types_h
#define __Types_h

#include "Eigen/Dense"

namespace ee_llp {

  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 Eigen::Dynamic,
			 Eigen::RowMajor > DoubleRowMajorMatrixType;

  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 1 > DoubleColumnVectorType;

  typedef Eigen::Matrix< double,
			 1,
			 Eigen::Dynamic > DoubleRowVectorType;

  typedef Eigen::PermutationMatrix<
    Eigen::Dynamic,
    Eigen::Dynamic > PermutationMatrixType;
  
}

#endif
