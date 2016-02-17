#include <iostream>

#include "Types.h"
#include "LLPCostFunction.h"
#include "BagProportionError.h"
#include "ContinousClusterLabeller.h"
#include "ExhaustiveBinaryClusterLabeller.h"
#include "GreedyBinaryClusterLabeller.h"


int main() {
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::DoubleColumnVectorType VectorType;
  
  ContinousClusterLabeller< LLPCostFunction > cLabeller;
  ExhaustiveBinaryClusterLabeller< MatrixType,
				   VectorType,
				   BagProportionError > ebLabeller;

  GreedyBinaryClusterLabeller< MatrixType,
			       VectorType,
			       BagProportionError > gbLabeller;

  std::cout << "Clusterers created" << std::endl;
}
