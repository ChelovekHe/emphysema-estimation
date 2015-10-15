project( EE_Test )

set( GTEST_LIBRARIES
  gtest
  gtest_main
  )

set( LIBS
  ${ITK_LIBRARIES}
  ${GTEST_LIBRARIES}
  )

set( progs
  DenseHistogramTest
  EigenvaluesTest
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
  add_test( ${prog} ${prog} )
endforeach( prog )