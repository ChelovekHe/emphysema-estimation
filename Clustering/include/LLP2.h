#ifndef __LLP2_h
#define __LLP2_h

#include <vector>

#include "Eigen/Dense"

#include "MatrixOperations.h"
#include "ClusterLabeller.h"
#include "WeightedEarthMoversDistance.h"
#include "KMeansClusterer.h"

class LLP2 {
public:
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorType;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixType;

  typedef ClusterLabeller<MatrixType, VectorType> LabellerType;

  typedef WeightedEarthMoversDistance< double > DistanceFunctorType;
  typedef KMeansClusterer< DistanceFunctorType > ClustererType;

  typedef std::vector< std::size_t > IndexVectorType;

  /**
     Structure holding the result of running the LLP2 optimization
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
     @param k          Maximum number of clusters to use for clustering
   */
  LLP2( const VectorType& p,
	const MatrixType& instances,
	const IndexVectorType& bagIndices,
	unsigned int k,
	std::size_t nHistograms,
	std::size_t nBins
	);

  ~LLP2() {};

  /**
     Run the label proportion learning and return the error.
     
     @param weights  Weights for the distance function.
     @return         The error measure for the given weights.
  */
  double operator()( const double* weights, const int& N );
  
  /**
     Get the result of the optmization.
     
     @return  Struct holding clustering + error.
   */
  ResultType getResult(  );

private:
  const MatrixType& m_Instances;
  const IndexVectorType& m_BagIndices;
  ClustererType m_Clusterer;
  LabellerType m_Labeller;
  std::size_t m_NumberOfBags;
  unsigned int m_K;
  ResultType m_Result;
  DistanceFunctorType m_Dist;
  double m_Lambda;
};

#include "LLP2.hxx"

#endif
