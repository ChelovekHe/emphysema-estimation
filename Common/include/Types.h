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


}

#endif
