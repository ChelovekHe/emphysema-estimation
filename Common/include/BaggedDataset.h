#ifndef __BaggedDataset_h
#define __BaggedDataset_h

#include <stdexcept>
#include <type_traits>
#include "Eigen/Dense"

template< size_t BagLabelDim=1, size_t InstanceLabelDim=1 >
class BaggedDataset {
public:
  typedef BaggedDataset< BagLabelDim, InstanceLabelDim > Self;
  // In some cases we MUST have that the matrix is stored in row-major order
  // unfortunately we cannot make row-major matrix with a single column unless
  // we make it Dynamic.
  // For this reason we use row-major for the matrix and col-major for the label
  // vectors when they have dim=1
  typedef Eigen::Matrix< double,
			 Eigen::Dynamic,
			 Eigen::Dynamic,
			 Eigen::RowMajor > MatrixType;

  typedef Eigen::Matrix< double,
   			 Eigen::Dynamic,
			 InstanceLabelDim,
			 InstanceLabelDim == 1 ? Eigen::ColMajor : Eigen::RowMajor > InstanceLabelVectorType;

  typedef Eigen::Matrix< double,
  			 Eigen::Dynamic,
  			 BagLabelDim,
  			 BagLabelDim == 1 ? Eigen::ColMajor : Eigen::RowMajor > BagLabelVectorType;

  typedef Eigen::Matrix< size_t,
  			 Eigen::Dynamic,
  			 1,
  			 Eigen::ColMajor > IndexVectorType;


  BaggedDataset()
    : m_Instances( )
    , m_BagMembershipIndices(  )
    , m_BagLabels(  )
    , m_InstanceLabels(  )
  {}   
  
  BaggedDataset( const MatrixType& instances,
		 const IndexVectorType& bagMembershipIndices,
		 const BagLabelVectorType& bagLabels,
		 const InstanceLabelVectorType& instanceLabels )		 
    : m_Instances( instances)
    , m_BagMembershipIndices( bagMembershipIndices )
    , m_BagLabels( bagLabels )
    , m_InstanceLabels( instanceLabels )
  {
    if ( instances.rows() != instanceLabels.rows() ) {
      throw std::logic_error( "Number of instance labels do not match number of instances" );
    }
    if ( instances.rows() != bagMembershipIndices.rows() ) {
      throw std::logic_error( "Number of bag membership indices do not match number of instances" );
    }
    if ( bagMembershipIndices.maxCoeff() >= bagLabels.rows() ) {
      throw std::logic_error( "Largest bag membership index is larger than the number of bag labels" );
    }    
  }

  size_t NumberOfBags() const {
    return m_BagLabels.rows();
  }
  
  size_t NumberOfInstances() const {
    return m_Instances.rows();
  }

  size_t Dimension() const {
    return m_Instances.cols();
  }

  const IndexVectorType& Indices() const {
    return m_BagMembershipIndices;
  }

  const MatrixType& Instances() const {
    return m_Instances;
  }

  const BagLabelVectorType& BagLabels() const {
    return m_BagLabels;
  }

  const InstanceLabelVectorType& InstanceLabels() const {
    return m_InstanceLabels;
  }

  void InstanceLabels(const InstanceLabelVectorType& instanceLabels) {
    if ( instanceLabels.rows() != m_Instances.rows() ) {
      throw std::logic_error( "Number of instance labels do not match number of instances" );
    }
    m_InstanceLabels = instanceLabels;
  }

  static
  Self
  Random( size_t numberOfBags, size_t bagSize, size_t dimension ) {
    size_t numberOfInstances = numberOfBags * bagSize;
    MatrixType instances = MatrixType::Random( numberOfInstances, dimension );
    IndexVectorType bagMembership = IndexVectorType( numberOfInstances );
    for ( size_t i = 0, index = 0; i < bagMembership.size(); ++i ) {
      if ( i >= (index + 1) * bagSize ) {
	++index;
      }
      bagMembership(i) = index;
    }
    BagLabelVectorType bagLabels = BagLabelVectorType::Random( numberOfBags, BagLabelDim );
    InstanceLabelVectorType instanceLabels = InstanceLabelVectorType::Zero( numberOfInstances, InstanceLabelDim );
    return Self( instances, bagMembership, bagLabels, instanceLabels );
  }  

  
private:
  MatrixType m_Instances;
  IndexVectorType m_BagMembershipIndices;
  BagLabelVectorType m_BagLabels;
  InstanceLabelVectorType m_InstanceLabels;
};

#endif
