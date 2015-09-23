project( EE_FeatureExtraction )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set( prog NormalizedConvolution )
add_executable( ${prog} ${prog}.cxx )
target_link_libraries( ${prog} ${ITK_LIBRARIES} )

install( TARGETS ${prog} DESTINATION bin )