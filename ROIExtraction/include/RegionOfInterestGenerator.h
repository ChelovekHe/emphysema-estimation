#ifndef __RegionOfInterestGenerator_h
#define __RegionOfInterestGenerator_h

#include <vector>
#include "itkImageRandomConstIteratorWithIndex.h"

namespace itk {
  template< typename TMask >
  class RegionOfInterestGenerator {
  public:
    typedef TMask MaskType;
    typedef typename MaskType::IndexType IndexType;
    typedef typename MaskType::SizeType SizeType;
    typedef typename MaskType::RegionType RegionType;
  
    RegionOfInterestGenerator(MaskType* mask);

    std::vector< RegionType >
    generate( size_t numberOfROIs, SizeType size );

  protected:
    typedef ImageRandomConstIteratorWithIndex< MaskType > RandomIteratorType;
  
  private:
    MaskType* m_Mask;
  };
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "RegionOfInterestGenerator.hxx"
#endif

#endif
