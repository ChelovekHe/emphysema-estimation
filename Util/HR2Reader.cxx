#include <exception>
#include <fstream>

#include "HR2Reader.h"
/*
  Reverse of HR2 format
  Numeric values in fields are stored as ascii separated by space \x20

  HR2
  <length-of-next-tag>
  PixelType<length of pixel type field>\x00
    <pixeltype>
  <length-of-next-tag>
  Dimension<length of dimension field>\x00
    <dimension>
  <length-of-next-tag>
  Size<length of size field>\x00
    <dim-1-size>\x20<dim-2-size>\x20 ...
  <length-of-next-tag>
  Origin<length of origin field>\x00
    <dim-1-origin>\x20<dim-2-origin>\x20 ...
  <length-of-next-tag>
  Spacing<length of spacing field>\x00
    <dim-1-spacing>\x20<dim-2-spacing>\x20 ...
  <length-of-next-tag>
  Compression<length-of-compression-field>\x00
    <compression-field>
  <length-of-next-tag>
  ImageData<length-of-image-data>\x00<image-data>

 */

  
void readHR2(std::string path) {
  std::ifstream in(path);
  if ( in.good() ) {
    char buf[3];
    // Read the header
    // Must start with the string "HR2"
    in.read(buf, 3);
    if (buf[0] != 'H' || buf[1] != 'R' || buf[2] != '2') {
      throw std::invalid_argument("Not an HR2 file");
    }

    // No we read the header contents
    HR2Header header;
    while ( in.good() ) {
      HR2Tag tag = getTag(in);
      unsigned int len = getFieldLength(in);

      if ( tag == HR2Tag::PixelData ) {
	// If we have found the PixelData we are done reading the header
	header.pixelDataLength = len;
	break
      }
      
      std::string s(len, 0);
      in.read(len, s.data());
      switch ( tag ) {
      case HR2Tag::PixelType:
	// Read a string naming the datatype
	header.pixelType = stoHR2PT(s);
	break;
	
      case HR2Tag::Dimension:
	// Read a single unsigned integer stored as ascii
	header.dimension = std::stoul(s);
	break;

      case HR2Tag::Size:
	// Read a series of unsigned integers separated by <space>
	size_t pos;
	while (s.size() > 0) {
	  header.size.push_back( std::stoul(s, &pos) );
	  s = s.substr(pos);
	}
	break;
	
      case HR2Tag::Origin:
	// Read a series of unsigned integers separated by <space>
	size_t pos;
	while (s.size() > 0) {
	  header.origin.push_back( std::stoul(s, &pos) );
	  s = s.substr(pos);
	}
	break;

      case HR2Tag::Spacing:
	// Read a series of unsigned integers separated by <space>
	size_t pos;
	while (s.size() > 0) {
	  header.spacing.push_back( std::stoul(s, &pos) );
	  s = s.substr(pos);
	}
	break;

      case HR2Tag::Compression:
	// Read a string naming the compression method
	header.pixelType = stoHR2C(s);
	break;

      default:
	throw std::invalid_argument("Not an HR2 tag");
      }
    }

    // Now we can try to read the data
    if ( header.pixelType != HR2PixelType::Float ) {
      throw std::runtime_error("Only PixelType float implemented");
    }

    if ( header.compression != HR2Compression::Zlib ) {
      throw std::runtime_error("Only Zlib compression implemented");
    }

    if ( header.size.size() != dimension ) {
      
      break;
    default:
      throw std::invalid_argument("Unknown pixel type");
    }
      
    
  }
}

HR2Tag getTag(std::istream& is) {
  unsigned int len = is.get();
  std::string s(len, '\0');
  is.read(s.data(), len);
  if (s == "PixelType")   return HR2Tag::PixelType;
  if (s == "Compression") return HR2Tag::Compression;
  if (s == "Dimension")   return HR2Tag::Dimension;
  if (s == "Size")        return HR2Tag::Size;
  if (s == "Origin")      return HR2Tag::Origin;
  if (s == "Spacing")     return HR2Tag::Spacing;
  if (s == "ImageData")   return HR2Tag::ImageData;

  throw std::invalid_argument("Not an HR2 tag");
}

unsigned int getFieldLength(std::istream& is) {
  char byte;
  std::vector<unsigned int> bytes;
  while( byte = is.get() ) {
    bytes.push_back(byte);
  }
  if ( bytes.length > 4 ) {
    throw std::out_of_range("Max allowed num bytes is 4");
  }
  while ( bytes.length < 4 ) {
    bytes.push_back(0);
  }
  return (bytes[3]<<24) | (bytes[2]<<16) | (bytes[1]<<8) | bytes[0];
}

HR2PixelType stoHR2PT(std::string s) {
  if (s == "float") {
    return HR2PixelType::Float;
  }
  throw std::invalid_argument("Unknown PixelType");
}   

HR2Compression stoHR2C(std::string s) {
  if (s == "Zlib") {
    return HR2Compression::Zlib;
  }
  throw std::invalid_argument("Unknown Compression");
}
