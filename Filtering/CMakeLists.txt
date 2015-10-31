project( EE_Filtering )

set( LIBS
  ${ITK_LIBRARIES}
  )

set( progs
  TryingHessian3DImageFilter
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )