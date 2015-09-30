#ifndef __DenseHistogram_h
#define __DenseHistogram_h

#include <vector>
#include <limits>

template< typename NumType >
class DenseHistogram {
public:
  typedef NumType value_type;
  typedef typename std::vector< value_type >::size_type size_type;
  
  // edges should be sorted
  DenseHistogram( std::initializer_list< value_type > edges )
    : m_Edges( edges ), m_Counts( edges.size() + 1 )
  {
    assert( edges.size() > 0 );
  }

  // Binary search for the bucket
  void insert( value_type value ) {
    size_type j = 0, k = m_Edges.size();
    size_type i = k/2;
    while ( j != k ) {
      if ( value < m_Edges[i] ) {
	k = i;
	i = j + (k - j)/2;
      }
      else {
	j = i;
	i = k - (k - j)/2;
	if (i == k) break;
      }
    }
    ++m_Counts[i];
  }

  // Linear search
  // void insert( value_type value ) {
  //   size_type i = 0;
  //   while ( i < m_Edges.size() && value >= m_Edges[i]  ) {
  //     ++i;
  //   }
  //   ++m_Counts.at(i);
  // }

  std::vector<unsigned int> getCounts() {
    return m_Counts;
  }
  
private:
  std::vector< value_type > m_Edges;
  std::vector< unsigned int > m_Counts;
  size_type m_Center, m_High;
  

};
#endif
