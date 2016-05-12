#ifndef __Bag_h
#define __Bag_h

#include <vector>
#include <algorithm>
#include "Types.h"

template< typename TInstanceLabel, typename TBagLabel=TInstanceLabel >
class Bag {
public:
  typedef TInstanceLabel InstanceLabelType;
  typedef TBagLabel BagLabelType;
  typedef Bag< InstanceLabelType, BagLabelType > Self;
  
  typedef ee_llp::DoubleRowMajorMatrixType MatrixType;
  typedef std::vector< InstanceLabelType > LabelVectorType;

  Bag()
    : m_Instances{},
      m_InstanceLabels{},
      m_BagLabel{}
  {}

  Bag( MatrixType& instances, LabelVectorType& instanceLabels )
    : m_Instances( instances ),
      m_InstanceLabels( instanceLabels ),
      m_BagLabel{}
  {}
  
  BagLabelType BagLabel() {
    TBagLabel x{};
    std::accumulate( m_InstanceLabels.cbegin(), m_InstanceLabels.cend(), x );
    return x / m_InstanceLabels.size();
  }
    
  MatrixType& Instances() {
    return m_Instances;
  }

  LabelVectorType& InstanceLabels() {
    return m_InstanceLabels;
  }

protected::
  MatrixType m_Instances;
  std::vector< InstanceLabelType > m_InstanceLabels;
  BagLabelType m_BagLabel;
};

#endif
