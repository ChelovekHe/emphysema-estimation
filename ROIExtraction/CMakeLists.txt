project( EE_ROIExtraction )

set( progs 
  GenerateROIs
  SampleROIs
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )