project( EE_Filtering )

set( LIBS
  ${ITK_LIBRARIES}
  )

set( progs
  TryingHessian3DImageFilter
  BinarizeImage
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )