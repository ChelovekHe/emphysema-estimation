project( EE_LLP )

set( LIBS
  ${ITK_LIBRARIES}
  ${CERES_LIBRARIES}
  ${CMAES_LIBRARIES}
  IO
  String
  )


set( progs 
  LLP
  LLP3
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )