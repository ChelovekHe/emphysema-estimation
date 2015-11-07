#include <random>
#include <ios>
#include <iostream>
#include <iomanip>
#include "LLP.h"

int main() {
  // Create some bags and label them with proportions
  typedef typename LLP::MatrixType MatrixType;
  typedef typename LLP::VectorType VectorType;
  typedef typename LLP::IndexVectorType IndexVectorType;
  typedef typename LLP::DistanceFunctorType DistanceFunctorType;
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis1(0, 0.1);
  std::uniform_real_distribution<> dis2(0.9, 1);

  const std::size_t rows = 300;
  const std::size_t cols = 2;
  const std::size_t bags = 3;
  const std::size_t k = 2;
  
  MatrixType instances = MatrixType::Zero(rows, cols);
  for ( std::size_t i = 0; i < rows/3; ++i ) {
    double x = dis1(gen);
    double y = dis2(gen);
    instances(i,0) = x;
    instances(i,1) = 1-x;
    instances(rows/3 + i,0) = (x+y)/2;
    instances(rows/3 + i,1) = 1 - (x+y)/2;
    instances(2*(rows/3) + i,0) = y;
    instances(2*(rows/3) + i,1) = 1 - y;
  }

  VectorType p(bags);
  p << 1, 0.5, 0;
  
  IndexVectorType bagIndices(rows);
  for ( std::size_t i = 0; i < rows/3; ++i ) {
    bagIndices[i] = 0;
    bagIndices[rows/3 + i] = 1;
    bagIndices[2*(rows/3) + i] = 2;
  }  

  LLP llp(p, instances, bagIndices );
  DistanceFunctorType dist({{2,1.0}});

  LLP::ResultType result = llp.run(dist, k);

  std::cout << "Error = " << result.error << std::endl
	    << "Bag proportions" << std::endl
	    << p << std::endl
	    << "Estimated proportions" << std::endl
	    << result.C * result.labels << std::endl;
  
  //  	    << "Clusters:" << std::endl;
  // for ( std::size_t i = 0; i < k; ++i ) {
  //   for ( std::size_t j = 0; j < cols; ++j ) {
  //     std::cout << result.clustering.centers(i,j) << " ";
  //   }
  //   std::cout << result.labels(i) << std::endl;
  // }
  
  // std::cout << std::endl
  // 	    << p
  // 	    << std::endl
  // 	    << result.C
  // 	    << std::endl
  // 	    << result.C * result.labels
  // 	    << std::endl;

  // VectorType c1 = result.clustering.centers.row(0);
  // VectorType c2 = result.clustering.centers.row(1);
  // std::cout << "------------------------------------------------------"
  // 	    << std::endl
  // 	    << c1 << std::endl << std::endl
  // 	    << c2 << std::endl << std::endl;
    
  
  // std::cout << "instance\t| bag label\t| cluster label\t| flann dist\t| c1 dist\t| c2 dist" << std::endl;


  // for ( std::size_t i = 0; i < rows; ++i ) {
  //   VectorType x = instances.row(i);
  //   std::cout << std::setprecision(2) << std::fixed
  // 	      << instances(i,0) << " " << instances(i,1) << "\t|\t"
  // 	      << bagIndices[i] << "\t|\t"
  // 	      << result.clustering.indices[i] << "\t|\t"
  // 	      << result.clustering.distances(i,0) << " | " 
  // 	      << dist(c1.data(), x.data(), 2) << " | "
  // 	      << dist(c2.data(), x.data(), 2) << " | "
  // 	      << (c1 - x).norm() << " | "
  // 	      << 1 - (c1 - x).norm() << " | "
  // 	      << (c2 - x).norm() << " | "
  // 	      << 1 - (c2 - x).norm() << " | "
  // 	      << std::endl;
  // }
  return 0;
}
  
