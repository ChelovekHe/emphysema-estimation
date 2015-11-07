#ifndef __LLP_hxx
#define __LLP_hxx

#include "LLP.h"

LLP
::LLP( const VectorType& p,
       const MatrixType& instances,
       const IndexVectorType& bagIndices
       )
  : m_Instances( instances ),
    m_BagIndices( bagIndices ),
    m_Clusterer( 2, -1 ),
    m_Labeller( p ),
    m_NumberOfBags( p.size() )
{
  
  // We must have that bagIndices can be used to index into p
  auto maxI = std::max_element( bagIndices.cbegin(), bagIndices.cend()); 
  if( maxI != bagIndices.end() && *maxI >= p.size() ) {
    throw std::invalid_argument( "Largest bag index is to large" );
  }
  
  // We must have that each instance has a bag label
  if ( instances.rows() != bagIndices.size() ) {
    throw std::invalid_argument( "There must exactly one bag index for each instance" );
  }
}

LLP::ResultType
LLP
::run( DistanceFunctorType dist, const std::size_t maxK )
{
  ResultType result;

  // Find clusters
  result.clustering = m_Clusterer.cluster( m_Instances, dist, maxK );

  // Find mapping from clusters to bags
  const std::size_t k = result.clustering.centers.rows();


  assert( m_BagIndices.size() == result.clustering.indices.size() );
  MatrixType C = MatrixType::Zero( m_NumberOfBags, k );
  coOccurenceMatrix( m_BagIndices.begin(),
		     m_BagIndices.end(),
		     result.clustering.indices.begin(),
		     result.clustering.indices.end(),
		     C );
  result.C = rowNormalize(C);
  // Find labels 
  result.labels = VectorType::Zero( k );
  result.error = m_Labeller.findOptimalLabels( result.C, result.labels );

  return result;
}

#endif
