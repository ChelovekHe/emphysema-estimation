#ifndef __ClusterModelTrainerNoCMAES_hxx
#define __ClusterModelTrainerNoCMAES_hxx

template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainerNoCMAES< TClusterer, TLabeller >
::train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm )
{
  VectorType weights = VectorType::Ones( m_Params.featureSpaceDimension );    
  DistanceFunctorType dist( weights.data(), weights.size() );
  cm.weights() = weights;

  auto clustering = m_Clusterer.cluster( instances, dist, m_Params.k );
  const std::size_t k = clustering.centers.rows();

  assert( bagLabels.size() == clustering.indices.size() );
  MatrixType C = MatrixType::Zero( p.size(), k );
  coOccurenceMatrix( bagLabels.begin(),
		     bagLabels.end(),
		     clustering.indices.begin(),
		     clustering.indices.end(),
		     C );
  C = rowNormalize(C);

  // Find labels 
  VectorType labels = VectorType::Zero( k );
  double loss = m_Labeller.label( p, C, labels );

  cm.setCenters( clustering.centers );
  cm.setLabels( labels );

  // Store the model if we have an output file
  if ( !m_Params.out.empty() ) {
    std::string modelFile = m_Params.out + "_" + std::to_string( m_Counter ) + ".model";
    std::ofstream o( modelFile );
    if ( o.good() ) {
      o << cm;
    }
    else {
      std::cerr << "Error writing model to " << modelFile << std::endl;
    }
  }
  
  cm.build();
  
  return loss;
}



template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainerNoCMAES< TClusterer, TLabeller >
::train( const MatrixType& I,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm )
{
  VectorType weights = VectorType::Ones( m_Params.featureSpaceDimension );   
  DistanceFunctorType dist( weights.data(), weights.size() );
  cm.weights() = weights;
  double loss = -1;
  for ( size_t i = 0; i < 1000; ++i ) {
    auto clustering = m_Clusterer.cluster( instances, dist, m_Params.k );

    const std::size_t k = clustering.centers.rows();
  
    assert( bagLabels.size() == clustering.indices.size() );
    MatrixType C = MatrixType::Zero( I.rows(), k );
    coOccurenceMatrix( bagLabels.begin(),
		       bagLabels.end(),
		       clustering.indices.begin(),
		       clustering.indices.end(),
		       C );
    C = rowNormalize(C);

    // Find labels 
    VectorType labels = VectorType::Zero( k );
    double loss = m_Labeller.label( I, C, labels );

    cm.setCenters( clustering.centers );
    cm.setLabels( labels );

    // Store the model if we have an output file
    if ( !m_Params.out.empty() ) {
      std::string modelFile = m_Params.out + "_" + std::to_string( i ) + ".model";
      std::ofstream o( modelFile );
      if ( o.good() ) {
	o << cm;
      }
      else {
	std::cerr << "Error writing model to " << modelFile << std::endl;
      }
    }
  }
    
  cm.build();
  
  return loss;
}


#endif
