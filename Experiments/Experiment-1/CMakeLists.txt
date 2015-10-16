project( EE_Experiment-1 )

set( LIBS
  ${ITK_LIBRARIES}
  IO
  String
  )


set( progs 
  Scalability
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )