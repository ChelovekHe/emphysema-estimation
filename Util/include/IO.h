#ifndef __IO_h
#define __IO_h

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

typedef std::pair< std::string, std::string > StringPair;

template< typename InputIt >
void
writeSequenceAsText( std::ostream& out,
		     InputIt begin,
		     InputIt end,
		     char sep=',' ) {
  bool first = true;
  while ( begin != end ) {
    if ( first ) {
      first = false;
    }
    else { 
      out << sep;
    }
    out  << *begin++;
  }
}

// template< typename InputIt, typename CharT >
// void
// writeSequenceOfSequenceAsText( std::ostream& out,
// 			       InputIt begin,
// 			       InputIt end,
// 			       CharT outer_sep='\n',
// 			       CharT inner_sep=',' ) {
//   bool first = true;
//   while ( begin != end ) {
//     if ( !first ) {
//       out << outer_sep;
//     }
//     writeSequenceAsText( out, begin->begin(), begin->end() );
//     first = false;
//     ++begin;
//   }
// }


template< typename ElemT, typename CharT, typename OutputIt >
void
readTextSequence( std::istream& is,
		  OutputIt out,
		  CharT sep=',' ) {
  std::basic_string< CharT > s;
  ElemT elem;
  while ( is.good() ) {
    std::getline( is, s, sep );
    std::basic_istringstream< CharT >(s) >> elem;
    *out++ = elem;
  }
}




std::vector< StringPair >
readPairList( std::string inPath, char sep=',' );






#endif
