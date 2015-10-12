project( EE_Statistics )

set( LIBS
  IO
  )

set( progs 
  DetermineHistogramBinWidtsFromPopulation
  MaskedImageHistogram
  )
foreach( prog ${progs} )
  add_executable( ${prog} ${prog}.cxx )
  target_link_libraries( ${prog} ${ITK_LIBRARIES} ${LIBS} )
  install( TARGETS ${prog} DESTINATION bin )
endforeach( prog )