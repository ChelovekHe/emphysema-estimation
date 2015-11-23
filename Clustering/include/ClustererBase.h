#ifndef __ClustererBase_h
#define __ClustererBase_h

#include "Clustering.h"

template< typename TMatrix,
	  typename TVector,
	  typename TIndexVector,
	  typename TDistanceFunctor>
class ClustererBase {
public:
  typedef TMatrix MatrixType;
  typedef TVector VectorType;
  typedef TIndexVector IndexVectorType;
  typedef TDistanceFunctor DistanceFunctorType;
  typedef Clustering< TMatrix, TIndexVector > ClusteringType;


  virtual ~ClustererBase() {};

  virtual
  ClusteringType
  cluster( const MatrixType& instances,
	   DistanceFunctorType& dist,
	   std::size_t requestedNumberOfClusters ) = 0;
};


#endif
