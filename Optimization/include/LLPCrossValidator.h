#ifndef __LLPCrossValidator_h
#define __LLPCrossValidator_h

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "Types.h"
#include "CrossValidationParams.h"

template< typename TBagTrainer >
class LLPCrossValidator {
public:
  typedef LLPCrossValidator< TBagTrainer > Self;

  typedef TBagTrainer TrainerType;
  typedef typename TrainerType::ModelType ModelType;

  typedef CrossValidationParams ParamsType;
  
  typedef ee_llp::DoubleColumnVectorType VectorType;
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef typename ModelType::IndexVectorType IndexVectorType;
  
  struct ResultType {
    VectorType bagPredictions;
    VectorType instancePredictions;
    std::vector< double > trainingLosses;
  };


  /**
     Run cross validation.

     @param p  Label proportions for bags

     @param instances  Instance data

     @param bagLabels  Label indicating which bag an instance belong to.

     @param trainer  Object that can train an LLP model

     @return  A struct holding the training and test loss for each 
     iteration of the validation
   */
  ResultType
  run( const VectorType& p,
       const MatrixType& instances,
       const IndexVectorType& bagLabels,
       TrainerType& trainer,
       const ParamsType& params );

private:  
  struct SplitType {
    IndexVectorType test, train;
  };


  /**
     Calculate the splits needed for k-fold CV
   */
  std::vector< SplitType >
  split_KFold( const IndexVectorType& indices,
	       const unsigned int k ) const
  {
    assert( k <= indices.size() );

    auto begin = indices.begin();
    auto end = indices.end();
    
    const unsigned int M = indices.size();
    const unsigned int minFoldSize = M / k;
    unsigned int excess = M % k;

    auto testBegin = begin;
    auto testEnd = begin;
  
    std::vector< SplitType > splits( k );
    for ( std::size_t i = 0; i < k; ++i ) {
      auto foldSize = minFoldSize;
      // If the folds are not equal size, we distribute the excess
      // evenly on the first folds
      if ( excess ) {
	++foldSize;
	--excess;
      }
        
      testBegin = testEnd;
      std::advance(testEnd, foldSize);

      // Copy test fold
      splits[i].test.resize( foldSize );
      std::copy( testBegin, testEnd, splits[i].test.begin() );

      // Copy train fold
      splits[i].train.resize( M - std::distance( testBegin, testEnd ) );

      // Copy the part before the test fold
      auto trainBegin = splits[i].train.begin();
      std::copy( begin, testBegin, trainBegin );

      // Copy the part after the test fold
      std::advance( trainBegin, std::distance( begin, testBegin ) );
      std::copy( testEnd, end, trainBegin );
    }
  
    return std::move( splits );
  }

  
  std::vector< SplitType >
  split_LeavePOut( const IndexVectorType& indices,
		   const unsigned int p ) const
  {
    throw std::logic_error( "Cannot call unimplemented function." );
  }


  struct LLPData {
    VectorType p;
    MatrixType instances;
    IndexVectorType bagLabels;
    IndexVectorType instanceIndices; // Index into the original instance matrix
  };

  /**
     Pick the data based on membership testing in indices.

     @param p  Bag proportions.
     @param instances  Instances. Each row is an instance.
     @param bagLabel  A vector mapping each instance to a bag in p.
     @param indices  Sorted vector of bag indices indicating which bags 
     to use. The indices must lie in [0, p.size()).
     @return  Struxt holding the selected p, instances and bagLabels.
   */
  LLPData
  pick( const VectorType& p,
	const MatrixType& instances,
	const IndexVectorType& bagLabels,
	const IndexVectorType& indices )
  {
    LLPData D;

    // We need to relabel the bags such that indices can be used to index p
    std::unordered_map<int, std::size_t > I;
    
    // Find the bag proportions we need
    D.p = VectorType( indices.size() );
    for ( std::size_t i = 0; i < indices.size(); ++i ) {
      D.p(i) = p( indices[i] );
      I[indices[i]] = i;
    }
      
    
    // Find the instances we need
    for ( std::size_t i = 0; i < bagLabels.size(); ++i ) {
      // If the bag label of instance i is in the 
      if ( std::binary_search( indices.begin(), indices.end(), bagLabels[i] ) ) {
	D.instanceIndices.push_back( i ); // This effectively forces it to be a std::vector
      }
    }

    D.instances = MatrixType( D.instanceIndices.size(), instances.cols() );
    D.bagLabels = IndexVectorType( D.instanceIndices.size() );
    for ( std::size_t i = 0; i < D.instanceIndices.size(); ++i ) {
      auto idx = D.instanceIndices[i];
      D.instances.row(i) = instances.row(idx);
      D.bagLabels[i] = I[bagLabels[idx]];
    }    
    
    return D;
  }
};


/**
   @param p         Known label proportions for bags
   @param instances Instances
   @param bagLabels A bag label for each instance. Each bag label should be an 
   index into p.
   @param trainer   The training algorithm
   @param params    Extra parameters
 */
template< typename TBagTrainer >
typename LLPCrossValidator< TBagTrainer >::ResultType
LLPCrossValidator< TBagTrainer >
::run( const VectorType& p,
       const MatrixType& instances,
       const IndexVectorType& bagLabels,
       TrainerType& trainer,
       const ParamsType& params )
{

  // We need to run cross validation on the level of bags
  // So we split the bags into a series of train/test sets
  IndexVectorType bagIndices( p.size() );
  std::iota( bagIndices.begin(), bagIndices.end(), 0 );
  
  if ( params.shuffle ) {
    std::random_shuffle( bagIndices.begin(), bagIndices.end() );
  }
    
  std::vector< SplitType > splits;
  switch ( params.cvType ) {
  case CrossValidationType::LEAVE_P_OUT:
    splits = this->split_LeavePOut( bagIndices, params.p );
    break;

  case CrossValidationType::K_FOLD:
    splits = this->split_KFold( bagIndices, params.k );
    break;
    
  default:
    // Dont know what to do
    std::cerr << "Unknown cross validation type." << std::endl
	      << "CrossValidationType: " << params.cvType << std::endl;
  }
  
  ResultType result;
  result.bagPredictions = - VectorType::Ones( p.size() );
  result.instancePredictions = - VectorType::Ones( instances.rows() );
					 
   
  // Iterate over the splits, train and test
  for ( auto& split : splits ) {
    if ( params.shuffle ) {
      // Make sure that the indices in both test and train are sorted.
      // The indices are bag ids (matching the values in bagLabels )
      std::sort( split.test.begin(), split.test.end() );
      std::sort( split.train.begin(), split.train.end() );
    }
    ModelType model;

    // Pick all the instances that have a bag label that match one in train
    auto trainData = pick( p, instances, bagLabels, split.train );

    // Pick all the instances that have a bag label that match one in test
    auto testData = pick( p, instances, bagLabels, split.test );

    result.trainingLosses.push_back(
      trainer.train( trainData.p,
		     trainData.instances,
		     trainData.bagLabels,
		     model )
    );

    auto bagPredictions = model.predictBags( testData.instances,
					     testData.bagLabels,
					     testData.p.size() );
    
    std::cout << "predicted " << bagPredictions.transpose() << std::endl
	      << "target " << testData.p.transpose() << std::endl;
    
    for ( std::size_t i = 0; i < split.test.size(); ++i ) {
      result.bagPredictions(split.test[i]) = bagPredictions(i);
    }

    // Get the instance predictions
    auto instancePredictions = model.predictInstances( testData.instances );
    for ( std::size_t i = 0; i < testData.instances.rows(); ++i ) {
      result.instancePredictions( testData.instanceIndices[i] ) = instancePredictions(i);
    }
  }
  
  return result;
}



#endif
