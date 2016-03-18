#ifndef __ClusterModelTrainer3_hxx
#define __ClusterModelTrainer3_hxx

template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainer3< TClusterer, TLabeller >
::train( const VectorType& p,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm )
{
  VectorType weights =
    VectorType::Ones( m_Params.featureSpaceDimension ) * 0.5;
  
  // We want to find a weighting of the feature space, such that the
  // weights are constrained to lie in [0,1].
  std::vector< double > lBounds( weights.size(), 0.0 );
  std::vector< double > uBounds( weights.size(), 1.0 );
  GenoPheno gp( &lBounds.front(), &uBounds.front(), weights.size() );
  
  CMAParameters cmaParams( weights.size(),
			   weights.data(),
			   m_Params.sigma,
			   m_Params.lambda,
			   m_Params.seed,
			   gp );
  cmaParams.set_algo( aCMAES );

  if ( m_Params.maxIterations > 0 ) {
    cmaParams.set_max_iter( m_Params.maxIterations );
  }

  if ( !m_Params.out.empty() ) {
    ++m_Counter;
    std::string tracefile = m_Params.out + "_cmaes_trace_" + std::to_string( m_Counter ) + ".dat";
    std::cout << "Tracing to " << tracefile << std::endl;
    cmaParams.set_fplot( tracefile );
  }
  
  // Define and wrap the objective for cmaes
  std::function< double(const double*, const int&) >
    objective =
    [&p, &instances, &bagLabels, this]
    ( const double* w, const int& N )
    {
      DistanceFunctorType d( w, N );
    
      // Find clustering
      auto clustering =
      this->m_Clusterer.cluster( instances, d, this->m_Params.k );
      
      // Get the actual number of clusters
      const std::size_t k = clustering.centers.rows();

      // Verify we got one cluster index for each instance
      assert( bagLabels.size() == clustering.indices.size() );
      
      // Calculate the mapping from clusters to bags
      MatrixType C = MatrixType::Zero( p.size(), k );
      coOccurenceMatrix( bagLabels.begin(),
			 bagLabels.end(),
			 clustering.indices.begin(),
			 clustering.indices.end(),
			 C );
      C = rowNormalize(C);
  
      // Find labels 
      VectorType labels = VectorType::Zero( k );
      double loss = this->m_Labeller.label( p, C, labels );

      // Store trace if requested
      if ( this->m_Params.trace ) {
	this->m_Trace.clusterings.push_back( clustering );
	this->m_Trace.labellings.push_back( labels );
	this->m_Trace.losses.push_back( loss );
      }
      return loss;
    }; // End of objective
  
  // Run the optimization
  CMASolutions solutions =
    libcmaes::cmaes< GenoPheno >( objective, cmaParams );

  // TODO: Handle the diferent ways that CMAES can terminate
  if ( solutions.run_status() < 0 ) {
    std::cerr << "Error occured while training model." << std::endl
	      << "CMA-ES error code: " << solutions.run_status() << std::endl;
    return std::numeric_limits<double>::infinity();
  }

  // Print the iteration count
  std::cout << "Used " << solutions.niter() << " iterations" << std::endl;
  
  // Now we use the weights we found in the optimization to train a model
  weights = gp.pheno( solutions.best_candidate().get_x_dvec() );
  std::cout << weights << std::endl;
  
  DistanceFunctorType dist( weights.data(), weights.size() );
  cm.weights() = weights;

  // We iterate 10 times to give an idea of how stable the clustering is
  double bestLoss = std::numeric_limits<double>::infinity();  
  for ( int i = 0; i < 10; ++i ) {
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
    std::cout << loss << std::endl;

    if ( loss < bestLoss ) {
      bestLoss = loss;
      cm.setCenters( clustering.centers );
      cm.setLabels( labels );
    }
  }

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
  
  return bestLoss;
}



template< typename TClusterer, typename TLabeller >
double 
ClusterModelTrainer3< TClusterer, TLabeller >
::train( const MatrixType& I,
	 const MatrixType& instances,
	 const IndexVectorType& bagLabels,
	 ModelType& cm )
{
  VectorType weights =
    VectorType::Ones( m_Params.featureSpaceDimension ) * 0.5;
  
  // We want to find a weighting of the feature space, such that the
  // weights are constrained to lie in [0,1].
  std::vector< double > lBounds( weights.size(), 0.0 );
  std::vector< double > uBounds( weights.size(), 1.0 );
  GenoPheno gp( &lBounds.front(), &uBounds.front(), weights.size() );
  
  CMAParameters cmaParams( weights.size(),
			   weights.data(),
			   m_Params.sigma,
			   m_Params.lambda,
			   m_Params.seed,
			   gp );
  cmaParams.set_algo( aCMAES );

  if ( m_Params.maxIterations > 0 ) {
    cmaParams.set_max_iter( m_Params.maxIterations );
  }

  if ( !m_Params.out.empty() ) {
    ++m_Counter;
    std::string tracefile = m_Params.out + "_cmaes_trace_" + std::to_string( m_Counter ) + ".dat";
    std::cout << "Tracing to " << tracefile << std::endl;
    cmaParams.set_fplot( tracefile );
  }
  
  // Define and wrap the objective for cmaes
  std::function< double(const double*, const int&) >
    objective =
    [&I, &instances, &bagLabels, this]
    ( const double* w, const int& N )
    {
      DistanceFunctorType d( w, N );
    
      // Find clustering
      auto clustering =
      this->m_Clusterer.cluster( instances, d, this->m_Params.k );
      
      // Get the actual number of clusters
      const std::size_t k = clustering.centers.rows();

      // Verify we got one cluster index for each instance
      assert( bagLabels.size() == clustering.indices.size() );
      
      // Calculate the mapping from clusters to bags
      MatrixType C = MatrixType::Zero( I.rows(), k );
      coOccurenceMatrix( bagLabels.begin(),
			 bagLabels.end(),
			 clustering.indices.begin(),
			 clustering.indices.end(),
			 C );
      C = rowNormalize(C);
  
      // Find labels 
      VectorType labels = VectorType::Zero( k );
      double loss = this->m_Labeller.label( I, C, labels );

      // Store trace if requested
      if ( this->m_Params.trace ) {
	this->m_Trace.clusterings.push_back( clustering );
	this->m_Trace.labellings.push_back( labels );
	this->m_Trace.losses.push_back( loss );
      }
      return loss;
    }; // End of objective
  
  // Run the optimization
  CMASolutions solutions =
    libcmaes::cmaes< GenoPheno >( objective, cmaParams );

  // TODO: Handle the diferent ways that CMAES can terminate
  if ( solutions.run_status() < 0 ) {
    std::cerr << "Error occured while training model." << std::endl
	      << "CMA-ES error code: " << solutions.run_status() << std::endl;
    return std::numeric_limits<double>::infinity();
  }

  // Print the iteration count
  std::cout << "Used " << solutions.niter() << " iterations" << std::endl;
  
  // Now we use the weights we found in the optimization to train a model
  weights = gp.pheno( solutions.best_candidate().get_x_dvec() );
  std::cout << weights << std::endl;
  
  DistanceFunctorType dist( weights.data(), weights.size() );
  cm.weights() = weights;

  // We iterate 10 times to give an idea of how stable the clustering is
  double bestLoss = std::numeric_limits<double>::infinity();  
  for ( int i = 0; i < 10; ++i ) {
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
    std::cout << loss << std::endl;

    if ( loss < bestLoss ) {
      bestLoss = loss;
      cm.setCenters( clustering.centers );
      cm.setLabels( labels );
    }
  }

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
  
  return bestLoss;
}


#endif
