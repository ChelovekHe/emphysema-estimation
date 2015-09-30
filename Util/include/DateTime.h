#ifndef __DateTime_h
#define __DateTime_h

#include <string>
#include <ctime>

std::string timestamp() {
  return std::to_string( std::time( nullptr ) );
}

#endif
