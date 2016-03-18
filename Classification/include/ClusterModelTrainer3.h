#ifndef __ClusterModelTrainer3_h
#define __ClusterModelTrainer3_h

#include <vector>

#include "libcmaes/cmaes.h"

#include "MatrixOperations.h"
#include "ClusterModelTrainerParameters.h"
#include "ClusterModel.h"

template< typename TClusterer,
	  typename TLabeller >
class ClusterModelTrainer3
{
public:
  typedef ClusterModelTrainer3< TClusterer, TLabeller > Self;

  typedef TClusterer ClustererType;
  typedef TLabeller LabellerType;
  
  typedef typename TClusterer::ClusteringType ClusteringType;
  typedef typename TClusterer::IndexVectorType IndexVectorType;
  typedef typename TClusterer::MatrixType MatrixType;
  typedef typename TClusterer::VectorType VectorType;
  typedef typename TClusterer::DistanceFunctorType DistanceFunctorType;
  typedef ClusterModel< DistanceFunctorType > ModelType;

  typedef ClusterModelTrainerParameters ParamsType;
  typedef double LossResultType;
  
  /* 
     Struct that holds trace of execution
  */
  struct Trace {
    std::vector< ClusteringType > clusterings;
    std::vector< VectorType > labellings;
    std::vector< LossResultType > losses;
  };

  /*
    Constructor

    @param clusterer  Object that can cluster instances 
    @param labeller   Object that can label a clustering 
    @param params     Parameters for the training 
   */
  ClusterModelTrainer3( ClustererType& clusterer,
			LabellerType& labeller,
			ClusterModelTrainerParameters& params )
    : m_Clusterer( clusterer ),
      m_Labeller( labeller ),
      m_Params( params ),
      m_Trace( ),
      m_Counter( )
  { }
  

  const Trace& getTrace() { return m_Trace; }

  /*
    Train a cluster model.

    @param p  Vector of bag proportions [IN]
    @param instances  Matrix of instances. One instance per row. [IN]
    @param bagLabels  Vector mapping instances to bags in p. [IN]
    @param cm      The cluster model to train  [OUT]

    @return  Training loss
   */
  double
  train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm);

  /*
    Train a cluster model with interval labels

    @param I  Matrix of interval bag proportions [IN]
    @param instances  Matrix of instances. One instance per row. [IN]
    @param bagLabels  Vector mapping instances to bags in p. [IN]
    @param cm      The cluster model to train  [OUT]

    @return  Training loss
   */
  double
  train( const MatrixType& I,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm);

  
private:
  typedef libcmaes::GenoPheno< libcmaes::pwqBoundStrategy > GenoPheno;
  typedef libcmaes::CMAParameters< GenoPheno > CMAParameters;
  typedef libcmaes::CMASolutions CMASolutions;

  ClustererType& m_Clusterer;
  LabellerType& m_Labeller;
  ClusterModelTrainerParameters& m_Params;
  Trace m_Trace;
  int m_Counter; // Counter for generating trace file names that are unique
                 // for the objects life time.
};

#include "ClusterModelTrainer3.hxx"
  
#endif
