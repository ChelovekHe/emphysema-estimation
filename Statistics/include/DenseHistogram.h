#ifndef __DenseHistogram_h
#define __DenseHistogram_h

#include <iterator>
#include <vector>
#include <limits>

#include "IO.h"

template< typename NumType >
class DenseHistogram {
public:
  typedef NumType value_type;
  typedef typename std::vector< value_type >::size_type size_type;

  template< typename T >
  friend std::ostream& operator<<(std::ostream&,
				  const DenseHistogram<T>&);
  
  template< typename InputIt >
  DenseHistogram( InputIt begin, InputIt end )
    : m_Edges( begin, end ), m_Counts( m_Edges.size() + 1 )
  {
    assert( m_Edges.size() > 0 );
    assert( m_Counts.size() > 1 );
  }

  
  // edges should be sorted
  DenseHistogram( std::initializer_list< value_type > edges )
    : m_Edges( edges ), m_Counts( edges.size() + 1 )
  {
    assert( m_Edges.size() > 0 );
    assert( m_Counts.size() > 1 );
  }

  void insert( value_type value ) {
    auto edge = std::lower_bound(m_Edges.begin(), m_Edges.end(), value );
    auto bin = std::distance( m_Edges.begin(), edge );
    assert( bin >= 0 );
    assert( bin < m_Counts.size() );
    ++m_Counts[bin];
  }
  
  // Binary search for the bucket
  // size_type j = 0, k = m_Edges.size();
  // size_type i = k/2;
  // while ( j != k ) {
  //   if ( value < m_Edges[i] ) {
  // 	k = i;
  // 	i = j + (k - j)/2;
  //   }
  //   else {
  // 	j = i;
  // 	i = k - (k - j)/2;
  // 	if (i == k) break;
  //   }
  // }
  // ++m_Counts[i];
  //}

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
  
  // template <class CharT, class Traits>
  // std::basic_ostream<CharT, Traits>&
  // operator<<(std::basic_ostream<CharT, Traits>& os,
  // 	     const DenseHistogram< NumType >& hist ) {

  //}

private:
  std::vector< value_type > m_Edges;
  std::vector< unsigned int > m_Counts;
  size_type m_Center, m_High;  
};

template<typename T>
std::ostream&
operator<<( std::ostream& os, const DenseHistogram< T >& hist ) {
  writeSequenceAsText( os, hist.m_Counts.begin(), hist.m_Counts.end() );
}

#endif
