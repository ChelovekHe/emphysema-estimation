project( EE_Util )

include_directories( include )

set( libs
  IO
  )
foreach( lib ${libs} )
  add_library( ${lib} STATIC ${lib}.cxx )
  install( TARGETS ${lib} DESTINATION lib )
endforeach( lib )

