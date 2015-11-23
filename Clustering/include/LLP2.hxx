#ifndef __LLP2_hxx
#define __LLP2_hxx

#include <cmath>

#include "LLP2.h"

LLP2
::LLP2( const VectorType& p,
	const MatrixType& instances,
	const IndexVectorType& bagIndices,
	unsigned int k,
	std::size_t nHistograms,
	std::size_t nBins
       )
  : m_Instances( instances ),
    m_BagIndices( bagIndices ),
    m_Clusterer( 2, -1 ),
    m_Labeller( p ),
    m_NumberOfBags( p.size() ),
    m_K( k ),
    m_Result( ),
    m_Dist( std::vector< std::pair< unsigned int, double > >( nHistograms, {nBins, 1} )),
    m_Lambda( std::sqrt( static_cast<double>( p.size() )))
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

  // We must have that the number of features match number of bins in histograms
  if ( instances.cols() != nHistograms*nBins ) {
    throw std::invalid_argument( "instances.cols() != nHistograms*nBins" );
  }

  
}

double
LLP2
::operator()( const double* featureWeights, const int& N ) {

  m_Dist.setWeights( featureWeights, N );

  // Find clusters
  m_Result.clustering = m_Clusterer.cluster( m_Instances, m_Dist, m_K );

  // Find mapping from clusters to bags
  const std::size_t k = m_Result.clustering.centers.rows();

  assert( m_BagIndices.size() == m_Result.clustering.indices.size() );
  MatrixType C = MatrixType::Zero( m_NumberOfBags, k );
  coOccurenceMatrix( m_BagIndices.begin(),
		     m_BagIndices.end(),
		     m_Result.clustering.indices.begin(),
		     m_Result.clustering.indices.end(),
		     C );
  m_Result.C = rowNormalize(C);
  // Find labels 
  m_Result.labels = VectorType::Zero( k );
  m_Result.error = m_Labeller.findOptimalLabels( m_Result.C,
						 m_Result.labels,
						 m_Lambda );

  return m_Result.error;
}

LLP2::ResultType
LLP2
::getResult() {
  return m_Result;
}

#endif
