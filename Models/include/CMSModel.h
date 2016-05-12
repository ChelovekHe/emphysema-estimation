#ifndef __CMSModel_h
#define __CMSModel_h


/*
  A cluster model should label instances according to 
  a basis given by labelled centers and a distance function

 */

#include <memory>
#include <limits>
#include <istream>
#include <ostream>
#include <ios>

#include "flann/flann.hpp"

template< typename TDistanceFunctor, typename TBaggedDataset >
class CMSModel {
public:
  typedef TDistanceFunctor DistanceFunctorType;
  typedef TBaggedDataset   BaggedDatasetType;
  typedef CMSModel< DistanceFunctorType, BaggedDatasetType > Self;
  typedef std::unique_ptr< Self > Pointer;

  typedef typename BaggedDatasetType::MatrixType MatrixType;
  typedef typename BaggedDatasetType::InstanceLabelVectorType LabelVectorType;
  
  /**
     Factory to simplify testing, where it is easier if we have a model member 
     in the test class.
   */
  static Pointer New( const MatrixType& centroids,
			const LabelVectorType& centroidLabels,
			const std::vector< double >& featureWeights ) {    
    return Pointer( new Self( centroids, centroidLabels, featureWeights ) );
  }
  
  template< typename T, typename T2 >
  friend std::ostream& operator<<(std::ostream& os, const CMSModel<T, T2>& obj);
  
  CMSModel( const MatrixType& centroids,
	    const LabelVectorType& centroidLabels,
	    const std::vector< double >& featureWeights )
    : m_Centroids( centroids )
    , m_Labels( centroidLabels )
    , m_Weights( featureWeights )
    , m_Index()
    , m_IndexIsBuilt( false )
  {
    if ( m_Centroids.rows() != m_Labels.rows() ) {
      throw std::logic_error( "Number of centroid labels do not match number of centroids" );
    }
    // This is just a wrapper around the memory allocated in m_Centroids.
    flann::Matrix< double > flannCentroids( m_Centroids.data(),
					    m_Centroids.rows(),
					    m_Centroids.cols() );  
    DistanceFunctorType dist( m_Weights.data(), m_Weights.size() );   
    m_Index = std::unique_ptr< IndexType >( new IndexType( flannCentroids, IndexParamsType(), dist ) );
  }
  
  ~CMSModel() {
  }


  const MatrixType& Centroids( ) const {
    return m_Centroids;
  } 
  
  const LabelVectorType& Labels( ) const {
    return m_Labels;
  }
  
  const std::vector< double >& Weights() const {
    return m_Weights;
  }

  /** 
      This is expensive if the number of centroids is large
  */ 
  void Build() {
    m_Index->buildIndex();
    m_IndexIsBuilt = true;
  }


  /**
     Predict instances in bags with a 1-NN centroid classifier
   */
  void Predict( BaggedDatasetType& bags ) {
    if ( ! m_IndexIsBuilt ) {
      Build();
    }
    SearchParamsType searchParams( flann::FLANN_CHECKS_UNLIMITED );
    searchParams.use_heap = flann::FLANN_False;
    searchParams.cores = 0; // Use as many as you like

    std::vector< int > indicesBuffer( bags.NumberOfInstances() );
    flann::Matrix< int > indices( indicesBuffer.data(), indicesBuffer.size(), 1 );
    
    std::vector< double > distancesBuffer( bags.NumberOfInstances() );
    flann::Matrix< double > distances( distancesBuffer.data(), distancesBuffer.size(), 1 );

    flann::Matrix< double > instances( const_cast< double* >( bags.Instances().data() ),
				       bags.NumberOfInstances(),
				       bags.Dimension() );

    m_Index->knnSearch( instances, indices, distances, 1, searchParams );
    LabelVectorType instanceLabels( indices.rows, bags.InstanceLabels().cols() );
    for ( std::size_t i = 0; i < indices.rows; ++i ) {
      instanceLabels.row(i) = m_Labels.row( *indices[i] );
    }
    bags.InstanceLabels( instanceLabels );
  } 


  std::ostream& Save( std::ostream& os ) const {
    os << "# number of weights   number of clusters   dimension of label space   dimension of feature space" << std::endl
       << m_Weights.size() << "   " 
       << m_Centroids.rows() << "   "
       << m_Labels.cols() << "   "
       << m_Centroids.cols() << std::endl;
    os.write( reinterpret_cast< const char* >( m_Weights.data() ), sizeof(double)*m_Weights.size() );
    os.write( reinterpret_cast< const char* >( m_Labels.data() ), sizeof(double)*m_Labels.size() );
    os.write( reinterpret_cast< const char* >( m_Centroids.data() ), sizeof(double)*m_Centroids.size() );
    return os;
  }

  static Pointer
  Load( std::istream& is ) {
    char c;
    is >> c;
    if ( c != '#' ) {
      is.setstate( std::ios::failbit );
      throw std::runtime_error( "Missing header" );
    }
    // Skip the line
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::size_t nWeights, nClusters, nLabels, nFeatures;
    is >> nWeights >> nClusters >> nLabels >> nFeatures;
    if ( !is || nWeights > nFeatures ) {
      is.setstate( std::ios::failbit );
      throw std::runtime_error( "Missing header" );
    }
    // We still have a newline we need to read
    is.get();
    
    std::vector< double > weights( nWeights ) ;
    LabelVectorType labels( nClusters, nLabels );
    MatrixType centroids( nClusters, nFeatures );

    double buf;
    char* bufPtr = reinterpret_cast< char* >( &buf );
    for ( std::size_t i = 0; i < nWeights; ++i ) {
      if ( ! is.read( bufPtr, sizeof buf ) ) {
	throw std::runtime_error( "Could not read weights" );
      }
      weights[i] = buf;      
    }
    
    for ( std::size_t i = 0; i < nClusters; ++i ) {
      for ( size_t j = 0; j < nLabels; ++j ) {
	if ( ! is.read( bufPtr, sizeof buf ) ) {
	  throw std::runtime_error( "Could not read labels" );
	}
	labels(i,j) = buf;
      }
    }

    for ( std::size_t i = 0; i < nClusters; ++i ) {
      for ( std::size_t j = 0; j < nFeatures; ++j ) {
	if ( ! is.read( bufPtr, sizeof buf ) ) {
	  throw std::runtime_error( "Could not read centroids" );
	}
    	centroids(i,j) = buf;
      }
    }
    
    return Self::New( centroids, labels, weights );
  }
  

  inline bool operator==(const Self& other) const {
    return
      ( m_Weights.size() == other.m_Weights.size() ) &&
      ( std::equal(m_Weights.begin(), m_Weights.end(), other.m_Weights.begin() ) ) &&
      ( m_Labels == other.m_Labels ) &&
      ( m_Centroids == other.m_Centroids );
  }

  inline bool operator!=( const Self& other ) const {
    return !( *this == other );
  }
    
  
private:
  typedef typename flann::LinearIndexParams IndexParamsType;
  typedef typename flann::SearchParams SearchParamsType;
  typedef typename flann::LinearIndex< DistanceFunctorType > IndexType;
  
  MatrixType m_Centroids;
  LabelVectorType m_Labels;
  std::vector< double > m_Weights;
  std::unique_ptr< IndexType > m_Index;
  bool m_IndexIsBuilt;
};

template< typename T, typename T2 >
std::ostream& operator<<(std::ostream& os, const CMSModel<T,T2>& obj) {
  return obj.Save( os );
}



#endif
