#ifndef __LLP_h
#define __LLP_h

#include <vector>

#include "Eigen/Dense"

#include "MatrixOperations.h"
#include "ClusterLabeller.h"
#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"

class LLP {
public:
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorType;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixType;

  typedef ClusterLabeller<MatrixType, VectorType> LabellerType;

  typedef WeightedEarthMoversDistance< double > DistanceFunctorType;
  typedef KMeansClusterer< DistanceFunctorType > ClustererType;

  typedef std::vector< std::size_t > IndexVectorType;

  /**
     Structure holding the result of running the LLP optimization
   */
  struct ResultType {
    typename ClustererType::ResultType clustering;
    MatrixType C;
    VectorType labels;
    double error;
  };

  /**
     @param p          Bag label proportions
     @param instances  Instance matrix of dimension |instances| x |features|
     @param bagIndices Bag indices matching each instance to a bag in p
   */
  LLP( const VectorType& p,
       const MatrixType& instances,
       const IndexVectorType& bagIndices );

  ~LLP() {};

  /**
    Run the label proportion learning.
    @param dist  Distance function to use for clustering
    @param k     Maximum number of clusters to use for clustering
   */
  ResultType run( DistanceFunctorType dist, std::size_t k );

private:
  const MatrixType& m_Instances;
  const IndexVectorType& m_BagIndices;
  ClustererType m_Clusterer;
  LabellerType m_Labeller;
  std::size_t m_NumberOfBags;
};

#include "LLP.hxx"

#endif
