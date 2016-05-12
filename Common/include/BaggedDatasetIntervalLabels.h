#ifndef __BaggedDataset_h
#define __BaggedDataset_h

#include "Eigen/Dense"

template< size_t BagLabelDim=1, size_t InstanceLabelDim=1 >
class BaggedDatasetIntervalBagLabels {
public:
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 Eigen::Dynamic,
			 Eigen::RowMajor > MatrixType;
  
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 InstanceLabelDim,
			 Eigen::RowMajor > InstanceLabelVectorType;

  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 BagLabelDim,
			 Eigen::RowMajor > BagLabelVectorType;

  BaggedDatasetIntervalBagLabels();

  size_t NumberOfBags() const {
    return m_BagLabels.rows();
  }
  
  size_t NumberOfInstances() const {
    return m_Instances.rows();
  }

  size_t Dimension() const {
    return m_Instances.cols();
  }

  const std::vector< size_t >& Indices() const {
    return m_BagMembershipIndices;
  }
  
  
private:
  MatrixType m_Instances;
  InstanceLabelVectorType m_InstanceLabels;
  std::vector< size_t > m_BagMembershipIndices;
  BagLabelVectorType m_BagLabels; 
};
