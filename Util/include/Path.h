#ifndef __Path_h
#define __Path_h

template<typename CharType>
class Path {
public:
  typedef std::basic_string<CharType> StringType;
  static StringType Join(StringType p1, StringType p2) {
    CharType sep = '/';
    auto p1_pos = p1.find_last_not_of( sep );
    p1_pos = p1_pos == StringType::npos
      ? 0           // Erase entire string
      : p1_pos + 1; // Preserve the last character
    p1.erase( p1_pos );
    p1.append( 1, sep );
    auto p2_pos = p2.find_first_not_of( sep );
    if ( p2_pos == StringType::npos ) {
      return p1;
    }
    p1.append( p2.substr( p2_pos ) );
    return p1;
  }
};

#endif
