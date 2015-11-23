#ifndef __Clustering_h
#define __Clustering_h

template< typename MatrixType, typename VectorType >
struct Clustering {
  Clustering() : centers(), indices(), distances() {}
  Clustering(MatrixType _centers,
	     VectorType _indices,
	     MatrixType _distances )
    : centers(_centers),
      indices(_indices),
      distances(_distances) {}
  
  MatrixType centers;
  VectorType indices;
  MatrixType distances;
};


#endif
