project( EE_Classification )

set( LIBS
  ${CERES_LIBRARIES}
  ${CMAES_LIBRARIES}
  )

set( progs
  ClusterModelTrainerTest
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )