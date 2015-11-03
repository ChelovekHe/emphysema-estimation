#ifndef __ROIReader_hxx
#define __ROIReader_hxx

#include "ROIReader.h"

template< typename TRegion >
std::vector< typename ROIReader< TRegion >::RegionType >
ROIReader< TRegion >
::read( std::string path ) {
  std::vector< RegionType > rois;
  read( path, std::back_inserter(rois) );
  return rois;
}  
  
template< typename TRegion >
template< typename OutputIter >
void
ROIReader< TRegion >
::read( std::string path, OutputIter it ) {
  std::ifstream is(path);
  read( is, it );
}

template< typename TRegion >
template< typename OutputIter >
void
ROIReader< TRegion >
::read( std::istream& is, OutputIter it ) {
  const std::streamsize count = std::numeric_limits<std::streamsize>::max();
  // Discard header by ignoring the first line
  is.ignore(count , '\n' );
  while ( is.good() ) {
    IndexType start;
    SizeType size;
    is.ignore( count, '[' );
    is >> start[0]; is.ignore( count, ',' );
    is >> start[1]; is.ignore( count, ',' );
    is >> start[2]; is.ignore( count, '[' );
    is >> size[0];  is.ignore( count, ',' );
    is >> size[1];  is.ignore( count, ',' );
    is >> size[2];  is.ignore( count, '\n' );
    if ( is.good() ) {
      *it++ = RegionType( start, size );
    }
  }
}

#endif
