#ifndef __NearestNeighbourTrainer_h
#define __NearestNeighbourTrainer_h

#include <vector>

#include "libcmaes/cmaes.h"

#include "MatrixOperations.h"
#include "NearestNeighbourModel.h"
#include "NearestNeighbourTrainerParameters.h"

template< typename TDistanceFunctor,
	  template< typename,typename > class TErrorFunctor >
class NearestNeighbourTrainer
{
public:
  typedef NearestNeighbourTrainer Self;

  typedef TDistanceFunctor DistanceFunctorType;
  typedef NearestNeighbourModel< DistanceFunctorType > ModelType; 
  typedef typename ModelType::MatrixType MatrixType;
  typedef typename ModelType::VectorType VectorType;
  typedef typename ModelType::IndexVectorType IndexVectorType;
  typedef NearestNeighbourTrainerParameters TrainerParams;
  typedef TErrorFunctor< VectorType, MatrixType > ErrorFunctorType;

  
  /*
    Constructor

   */
  NearestNeighbourTrainer( TrainerParams& params )
    : m_Params( params )
  {}
  

  /*
    Train a nearest neighbour model.

    @param p          Vector of bag proportions [IN]
    @param instances  Matrix of instances. One instance per row. [IN]
    @param bagLabels  Vector mapping instances to bags in p. [IN]
    @param model      The nearest neighbour model to train  [OUT]

    @return Bag prediction error
   */
  double
  train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& model );
  
private:
  TrainerParams& m_Params;
};


template< typename TDistanceFunctor,
	  template< typename, typename > class TErrorFunctor >
double 
NearestNeighbourTrainer< TDistanceFunctor, TErrorFunctor >
::train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& model )
{
  // TODO: This function should also optimize the weights like the cluster model
  
  // Label each instance with the proportion of the corresponding bag
  VectorType labels = VectorType::Zero( instances.rows() );
  for ( std::size_t i = 0; i < bagLabels.size(); ++i ) {
    labels(i) = p( bagLabels[i] );
  }
  
  model.setInstances( instances );  
  model.setLabels( labels );
  model.weights() = VectorType::Ones( m_Params.featureSpaceDimension ) * 0.5;
  model.setK( m_Params.k );
  model.build();

  // Find the bag training error
  VectorType predictions = model.predictBags( instances, bagLabels, p.size() );
  ErrorFunctorType error;
  return error( p, predictions );
}
  
#endif
