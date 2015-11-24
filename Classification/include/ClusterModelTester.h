#ifndef __Tester_h
#define __Tester_h

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

  /**
     Coonstructor

     @param data  Matrix containing the instances in each row. 
    The next to last column is assumed to be the bag proportions.
    The last column is assumed to be the bag membership labels.
   */
  ClusterModelTester( const MatrixType& data )
    : m_Instances( data.leftCols( data.cols() - 2) ),
      m_P( data.col( data.cols() - 2 ) ),
      m_BagIndices( data.rows() )
  {
    VectorType bagIndices = data.col( data.cols() - 1 );
    for ( std::size_t i = 0; i < bagIndices.size(); ++i ) {
      assert( bagIndices(i) >= 0 );
      m_BagIndices[i] = static_cast< std::size_t >( bagIndices(i) );
    }    
  }

  /**
     Apply a model on the data used when constructing the tester

     @param model  A ClusterModel

     @return Aggregated loss over all test instances
   */
  LossResultType
  test( const ModelType& model ) {
    // Predict instances
    VectorType x = model.predict( m_Instances );

    // Aggregate into bags
    VectorType y = VectorType::Zero( m_P.size() );
    VectorType bagCounts = VectorType::Zero( m_P.size() );
    for ( std::size_t i = 0; i < m_BagIndices.size(); ++i ) {
      y(m_BagIndices[i]) += x(i);
      bagCounts(m_BagIndices[i]) += 1;
    }
    y = y.cwiseQuotient( bagCounts );    

    // Calculate loss
    LossFunctorType lossFunction( m_P );
    return lossFunction( y ) / static_cast< LossResultType >(m_P.size());
  }

private:
  typedef std::vector< std::size_t > IndexVectorType;
  
  VectorType m_P;
  MatrixType m_Instances;
  IndexVectorType m_BagIndices;  
};  
#endif
