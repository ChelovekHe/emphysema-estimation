project( EE_Optimization )

set( LIBS
  ${CERES_LIBRARIES}
  )

set( progs
  TestClusterLabeller
  TestClusterLabellerSetup
  LLPCostFunctionTest
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )