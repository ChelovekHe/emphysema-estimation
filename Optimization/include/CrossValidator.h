#ifndef __CrossValidator_h
#define __CrossValidator_h

#include <algorithm>
#include <vector>


#include "Types.h"

enum class CrossValidationType: char { LEAVE_P_OUT, K_FOLD };

std::ostream& operator<<(std::ostream& out, CrossValidationType cvt ) {
  switch ( cvt ) {
  case CrossValidationType::LEAVE_P_OUT:
    out << "Leave-p-out";
    break;

  case CrossValidationType::K_FOLD:
    out << "k-Fold";
    break;

  }
  return out;
}

struct CrossValidationParams {
  CrossValidationParams( CrossValidationType cvType,
			 unsigned int k,
			 bool shuffle )
    : cvType( cvType ),
      k( k ),
      shuffle( shuffle )
  {}

  CrossValidationType cvType; // The kind of CV to perform
  union {
    unsigned int p; // For leave-p-out CV
    unsigned int k; // For k-fold CV
  };
  bool shuffle; // Should the cross validator shuffle the data before
                // partitioning.
};


template< typename TTrainer,
	  typename TTester >
class CrossValidator {
public:
  typedef TTrainer TrainerType;
  typedef TTester TesterType;
  
  typedef typename TrainerType::ModelType ModelType;
  typedef typename TrainerType::LossResultType TrainLossResultType;
  typedef typename TrainerType::ParamsType TrainerParamsType;

  typedef typename TesterType::LossResultType TestLossResultType;

  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef ee_llp::PermutationMatrixType PermutationMatrixType;
  
  struct ResultType {
    std::vector< ModelType > models;
    std::vector< TrainLossResultType > trainingLosses;
    std::vector< TestLossResultType > testLosses;
  };


  /**
     Run cross validation.

     @param trainData  An m x n matrix, where each row is a sample. 
     In each iteration a subset of the rows are used to initialize the 
     trainer and the remaining rows are used for testing.

     @param trainerParams  Parameters passed to the trainer

     @param cvParams  A struct holding parameters for the cross validation
     
     @return  A struct holding the model and the error obtained in each 
     iteration of the cross validation.
   */
  ResultType
  run( MatrixType& trainData,
       const unsigned int featureSpaceDimension,
       const TrainerParamsType& trainerParams,
       const CrossValidationParams& cvParams ) {

    if ( cvParams.shuffle ) {
      PermutationMatrixType P( trainData.rows() );
      P.setIdentity();
      std::random_shuffle( P.indices().data(),
			   P.indices().data() + P.indices().size() );
      trainData = P * trainData;
    }
    
    std::vector< SplitType > splits;
    
    switch ( cvParams.cvType ) {
    case CrossValidationType::LEAVE_P_OUT:
      splits = this->split_LeavePOut( trainData, cvParams.p );
      break;

    case CrossValidationType::K_FOLD:
      splits = this->split_KFold( trainData, cvParams.k );
      break;

    default:
      // Dont know what to do
      std::cerr << "Unknown cross validation type." << std::endl
		<< "CrossValidationType: " << cvParams.cvType << std::endl;
    }

    ResultType result;
   
    // Iterate over the splits, train and test
    for ( const auto& split : splits ) {
      TrainerType trainer( split.train, featureSpaceDimension );
      TesterType tester( split.test );

      result.models.emplace_back( );
      result.trainingLosses.push_back(
	trainer.train( result.models.back(), trainerParams )
      );
      result.testLosses.push_back(
	tester.test( result.models.back() )
      );
    }

    return result;
  }

private:
  struct SplitType {
    SplitType( std::size_t testRows, std::size_t testCols,
	       std::size_t trainRows, std::size_t trainCols )
      : test(testRows, testCols),
	train(trainRows, trainCols)
      {};
    MatrixType test;
    MatrixType train;
  };

  
  /**
     Calculate the splits needed for k-fold CV
   */
  
  std::vector< SplitType >
  split_KFold( const MatrixType& A, const unsigned int k ) {
    assert( k <= A.rows() );
    const unsigned int M = A.rows();
    const unsigned int N = A.cols();
    const unsigned int foldSize = M / k;
    unsigned int excess = M % k;

    std::size_t foldStart = 0, foldEnd = 0;
    std::vector< SplitType > splits;
    splits.reserve( k );

    while ( splits.size() < k ) {
      // We pick [foldStart, foldEnd) as the test data and use the
      // remaing as training data
      foldStart = foldEnd;
      foldEnd = foldStart + foldSize;

      // If the folds are not equal size, we distribute the excess
      // evenly on the first folds
      if ( excess ) {
	++foldEnd;
	--excess;
      }

      auto testM = foldEnd - foldStart;
      auto trainM = M - testM;
      // Create a new split object
      splits.emplace_back( testM, N, trainM, N );

      auto* testBegin = A.data() + foldStart*N;
      auto* testEnd = A.data() + foldEnd*N;
      auto* begin = A.data();
      auto* end = A.data() + M*N;
      auto* testDst = splits.back().test.data();
      // Copy the test data
      std::copy( testBegin, testEnd, testDst );

      // Copy the train data
      // Copy the part before the test fold
      auto* trainDst = splits.back().train.data();
      std::copy( begin, testBegin, trainDst );
      
      // Copy the part after the test fold
      trainDst += foldStart*N;
      std::copy( testEnd, end, trainDst );
            
      foldStart = foldEnd;
    }
    return std::move(splits);
  }

  
  std::vector< SplitType >
  split_LeavePOut( const MatrixType& data, const unsigned int p ) {
    throw std::logic_error( "Cannot call unimplemented function." );
  }
    
  //   assert( p <= data.size() );
    
  //   std::vector< std::size_t > I( data.size() );
  //   typedef typename std::vector< std::size_t >::iterator IterType;
    
  //   std::iota( I.begin(), I.end(), 0 );
  //   foreach_combination( I.begin(),
  // 			 I.begin() + p,
  // 			 I.end(),
  // 			 [&data,&splits](IterType begin, IterType end) {
			   
  //   );
    
  //   // We make a list of all the indices and permute them
  //   // lexicographically untill we have tried them all.
  //   std::vector< std::size_t > I( data.size() );
  //   std::iota( I.begin(), I.end(), 0 );

  //   std::vector< SplitType > splits;
  //   do {
  //     // Take the first P
  //   } while ( std::next_permutation( I.begin(), I.end() ) )
    
    
    

  //   for ( std::size_t fold = 0; fold < k; ++fold ) {
  //     splits.test = MatrixType( size, data.cols() );
  //     splits.train = MatrixType( data.rows() - size, data.cols() );

  //     // We pick [foldStart, foldEnd) as the test data and
  //     // use the remaing as training data      
  //     std::size_t foldEnd = foldStart + size;
  //     if ( excess ) {
  // 	++foldEnd;
  // 	--excess;
  //     }
      
  //     std::size_t iTest = 0, iTrain = 0;
  //     for ( std::size_t i = 0; i < data.rows(); ++i ) {
  // 	if ( i >= foldStart && i < foldEnd ) {
  // 	  // Test row
  // 	  splits.test.row(iTest++) = data.row(i);
  // 	}
  // 	else {
  // 	  splits.train.row(iTrain++) = data.row(i);
  // 	}
  //     }

  //     foldStart = foldEnd;      
  //   }
  //   return splits;
  // }
    
};


//#include "CrossValidator.hxx"

#endif
