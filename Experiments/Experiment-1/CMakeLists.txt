project( EE_Experiment-1 )

set( progs 
  Scalability
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )