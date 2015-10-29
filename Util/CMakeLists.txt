project( EE_Util )

include_directories( include )

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

set( libs
  IO
  String
  HR2Reader
  )
foreach( lib ${libs} )
  add_library( ${lib} STATIC ${lib}.cxx )
  install( TARGETS ${lib} DESTINATION lib )
endforeach( lib )

set( progs
  ConvertHR2
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${libs} ${ITK_LIBRARIES} ${ZLIB_LIBRARIES})
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )