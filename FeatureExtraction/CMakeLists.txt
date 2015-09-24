project( EE_FeatureExtraction )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set( progs 
  NormalizedConvolution
  HessianFeatures
  FiniteDifference_GradientFeatures)
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )