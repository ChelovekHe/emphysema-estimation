#ifndef __ClusterModelTester_h
#define __ClusterModelTester_h

#include <vector>

template< typename TModel,
	  typename TLossFunctor >
class ClusterModelTester
{
public:
  typedef TModel ModelType;
  typedef TLossFunctor LossFunctorType;
  typedef typename LossFunctorType::ResultType LossResultType;
  typedef typename ModelType::MatrixType MatrixType;
  typedef typename ModelType::VectorType VectorType;
  typedef typename ModelType::IndexVectorType IndexVectorType;

  /**
     Constructor

   */
  ClusterModelTester(  ) {};

  /**
     Apply a model on the data used when constructing the tester
     
     @param p  Vector of bag proportions
     @param instances  Matrix of instances. One instance per row.
     @param bagLabels  Vector mapping instances to bags in p
     @param model  A ClusterModel

     @return Aggregated loss over all test instances
   */
  LossResultType
  test( const VectorType& p,
	const MatrixType& instances,
	const IndexVectorType& bagLabels,
	const ModelType& model ) {
    // Predict instances
    VectorType instancePrediction = model.predict( instances );

    // Aggregate into bags
    VectorType bagPrediction = VectorType::Zero( p.size() );
    VectorType bagCounts = VectorType::Zero( p.size() );
    for ( std::size_t i = 0; i < bagLabels.size(); ++i ) {
      bagPrediction( bagLabels[i] ) += instancePrediction(i);
      bagCounts( bagLabels[i] ) += 1; 
    }
    bagPrediction = bagPrediction.cwiseQuotient( bagCounts );    

    // Calculate loss
    LossFunctorType lossFunction( p );
    return lossFunction( bagPrediction );
  }

private:

  
  VectorType m_P;
  MatrixType m_Instances;
  IndexVectorType m_BagIndices;  
};  
#endif
