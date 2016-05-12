#ifndef __BagConverter_h
#define __BagConverter_h

#include <vector>
#include "Types.h"

template< BagType >
DoubleRowMajorMatrixType
BagsAsDoubleRowMajorMatrix( std::vector< BagType > bags) {
  // Figure out the number of instances
  std::size_t rows = std::accumulate( bags.cbegin(),
				      bags.cend(),
				      0,
				      [](std::size_t i, BagType& bag) {
					return i + bag.Size();
				      } );
  if ( rows == 0 ) {
    return DoubleRowMajorMatrixType(0,0);
  }
     
  // We need to verify that all bags have the same feature space dimensions
  std::size_t cols = bags.front().Dimension();
  bool dimMatch = std::all_off( bags.begin(),
				bags.end(),
				[cols]( const BagType& bag ) {
				  return bag.Dimension() == cols;
				} );
  if ( !dimMatch ) {
    throw std::domain_error( "Dimensions of bags do not match" );
  }
    
  DoubleRowMajorMatrixType M(rows, cols);
  std::size_t row = 0;
  for (auto bagIt = bags.cbegin(); bagIt != bags.cend(); ++bagIt ) {
    M
    for (auto insIt = bagIt.begin(); insIt != bagIt.end(); ++insIt ) {
      M.row(row) = 
    
    
    




template< typename TInstance, typename TLabel >
class BagConverter {
public:
  typedef TInstance InstanceType;
  typedef TLabel BagLabelType;
  typedef BagConverter< InstanceType, BagLabelType > Self;

  typedef typename std::vector< InstanceType >::iterator Iterator;
  
  BagConverter();

  /**
     \brief Get the label of the bag
  */
  const BagLabelType& BagLabel() const {
    return m_BagLabel;
  }

  /**
     \brief Set the label of the bag
  */
  void BagLabel(BagLabelType& label) {
    m_BagLabel = label;
  }


  /**
     \brief Return an iterator over instances in the bag pointing to the first
            instance.

     Bag b;
     *(b.begin()) is the first instance in the bag

   */
  Iterator begin() {
    return m_Instances.begin();
  }


  /**
     \brief Return an iterator over instances in the bag pointing one past the end

   */
  Iterator end() {
    return m_Instances.end();
  }

 
  /**
     \brief Get instances in the bag

     \return   vector of instances
  */
  std::vector<InstanceType>& Instances() {
    return m_Instances;
  }
  
protected:
  std::vector< InstanceType > m_Instances;
  BagLabelType m_BagLabel;
};

#endif



  // /**
  //    \brief Get a matrix representation of the instance features
  //           Each row contains one instance.
  //  */
  // MatrixType& Features() {
  //   return m_Features;
  // }


  // void CalculateBagLabel( BagLabelType (*f)( const std::vector<InstanceLabelType>& ) ) {
  //   m_BagLabel = f( m_InstanceLabels );
  // }


  // typedef typename InstanceType::FeatureType FeatureType;
  // typedef typename InstanceType::LabelType InstanceLabelType;
