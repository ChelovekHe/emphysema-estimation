#ifndef __StreamTraceBase_h
#define __StreamTraceBase_h

#include <ostream>

class StreamTraceBase {
 public:
  StreamTraceBase( std::ostream& out )
    : m_Out( out )
  {}

  template< typename T >
  void Trace( T& o ) {
    m_Out << o;
  }

  
 protected:
  std::ostream& m_Out;

}
