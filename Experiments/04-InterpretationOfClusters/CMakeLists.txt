project( EE_InterpretationOfClusters )

set( LIBS
  ${ITK_LIBRARIES}
  IO
  String
  )


set( progs
  ClusterInterpretation
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )