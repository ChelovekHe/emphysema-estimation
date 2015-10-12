project( EE_FeatureExtraction )

set( progs
  MaskedImageFilter
  MaskedNormalizedConvolution
  FiniteDifference_HessianFeatures
  FiniteDifference_GradientFeatures)
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )