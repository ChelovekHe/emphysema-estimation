#include <iostream>

#include "ClusterLabeller.h"
#include "Eigen/Dense"


int main() {
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixType;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorType;
  typedef ClusterLabeller<MatrixType, VectorType> LabellerType;

  VectorType p(5);
  p(0) = 0.3; p(1) = 0.5; p(2) = 0.2; p(3) = 0.8; p(4) = 0.4;
  
  VectorType x = VectorType::Zero(2);
  MatrixType C(5,2);
  C(0,0) = 1/3.0; C(0,1) = 2/3.0;
  C(1,0) = 1/2.0; C(1,1) = 1/2.0;
  C(2,0) = 1/5.0; C(2,1) = 4/5.0;
  C(3,0) = 3/4.0; C(3,1) = 1/4.0;
  C(4,0) = 5/6.0; C(4,1) = 1/6.0;
    
  LabellerType labeller(p);
  
  labeller.findOptimalLabels(C, x);

  std::cout << "Result " << std::endl
	    << x << std::endl << std::endl
	    << p << std::endl << std::endl
	    << C*x << std::endl;

  return 0;
}
    
