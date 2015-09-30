#ifndef __IO_h
#define __IO_h

#include <fstream>
#include <string>
#include <vector>

#include "String.h"

std::vector< std::pair< std::string, std::string > >
readPairList( std::string inPath ) {
  typedef std::pair< std::string, std::string > StringPair;
  
  std::ifstream is( inPath );
  std::vector< StringPair > pathPairs;
  std::string line;
  while ( is.good() ) {
    std::getline( is, line );
    if ( line.size() == 0 ) continue;
    auto pos = line.find_first_of( ',' );
    if ( pos == std::string::npos ) {
      std::cerr << "Missing ','" << std::endl;
    }
    else {
      pathPairs.emplace_back( std::make_pair( trim( line.substr(0, pos) ),
					      trim( line.substr(pos + 1) )
					      )
			      );
    }
  }
      
  return pathPairs;
}


#endif
