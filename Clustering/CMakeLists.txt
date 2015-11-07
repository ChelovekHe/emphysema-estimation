project( EE_Clustering )
include_directories( include )

# set( libs
#   LLP
#   )
# foreach( lib ${libs} )
#   add_library( ${lib} STATIC ${lib}.cxx )
#   target_link_libraries( ${lib} )
#   install( TARGETS ${lib} DESTINATION lib )
# endforeach( lib )

set( LIBS
  ${CERES_LIBRARIES}
  )

set( progs
  LLPTest
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )