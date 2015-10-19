#ifndef __HR2Reader_h
#define __HR2Reader_h

#include <string>
#include <vector>

void readHR2(std::string path);

enum struct HR2Tag {
    PixelType,
    Compression,
    Dimension,
    Size,
    Origin,
    Spacing,
    ImageData
};

enum struct HR2PixelType {
  Float
};

enum struct HR2Compression {
  Zlib
};

HR2PixelType stoHR2PT(std::string s);
HR2Compression stoHR2C(std::string s);

struct HR2Header {
  HR2PixelType pixelType;
  HR2Compression compression;
  size_t dimension;
  size_t pixelDataLength;
  std::vector<size_t> size;
  std::vector<size_t> origin;
  std::vector<size_t> spacing;  
};

#endif
