project( EE_Statistics )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set( progs 
  DetermineHistogramBinWidtsFromPopulation
  MaskedImageHistogram
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )