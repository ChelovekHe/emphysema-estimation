project( EE_IO )
include_directories( include )

set( libs
  IO
  HR2Reader
  )
foreach( lib ${libs} )
  add_library( ${lib} STATIC ${lib}.cxx )
  install( TARGETS ${lib} DESTINATION lib )
endforeach( lib )


find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

set( LIBS
  ${ITK_LIBRARIES}
  ${ZLIB_LIBRARIES}
  String
  HR2Reader
  )

set( progs
  ConvertFromOctave
  ConvertHR2
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )