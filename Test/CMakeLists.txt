project( EE_Test )

set( GTEST_LIBRARIES
  gtest
  gtest_main
  )

set( LIBS
  ${ITK_LIBRARIES}
  ${GTEST_LIBRARIES}
  ${CERES_LIBRARIES}
  )

set( progs
  DenseHistogramTest
  EigenvaluesTest
  HausdorffTest
  WeightedEarthMoversDistanceTest
  DetermineEdgesForEqualizedHistogramTest
  ClusterLabellerTest
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
  add_test( NAME ${prog} COMMAND ${prog} --gtest_repeat=1000 )
endforeach( prog )