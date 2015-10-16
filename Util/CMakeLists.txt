project( EE_Util )

include_directories( include )

set( libs
  IO
  String
  )
foreach( lib ${libs} )
  add_library( ${lib} STATIC ${lib}.cxx )
  install( TARGETS ${lib} DESTINATION lib )
endforeach( lib )

