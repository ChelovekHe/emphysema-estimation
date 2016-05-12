#ifndef __BagBase_h
#define __BagBase_h

#include "Eigen/Dense"
#include <vector>

// A bag should be a vector of instances, where each instance is a vector.

template< typename TInstanceLabel, typename TBagLabel=TInstanceLabel >
class BagBase {
public:
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 Eigen::Dynamic,
			 Eigen::RowMajor > MatrixType;
  typedef TInstanceLabel InstanceLabelType;
  typedef TBagLabel BagLabelType;
  typedef BagBase< MatrixType, InstanceLabelType, BagLabelType > Self;

  BagBase()
    : m_Instances(),
      m_InstanceLabels(),
      m_BagLabel {}

  /**
     \brief Get the label of the bag
  */
  BagLabelType& BagLabel() {
    return m_BagLabel;
  }

  /**
     \brief Get the bag size AKA number of instances in the bag
  */
  std::size_t Size() {
    return m_Instances.rows();
  }
  

  /**
     \brief Get the dimension of the instance space
  */
  std::size_t Dimension() {
    return m_Instances.cols();
  }
  
  /**
     \brief Get instances in the bag

     \return   vector of instances
  */
  MatrixType& Instances() {
    return m_Instances;
  }

  /**
     \brief Get instances in the bag

     \return   vector of instances
  */
  std::vector< InstanceLabelType >& InstanceLabels() {
    return m_InstanceLabels;
  }

  
protected:
  MatrixType m_Instances;
  std::vector< InstanceLabelType > m_InstanceLabels;
  BagLabelType m_BagLabel;
};

#endif
