FILE ( GLOB files "*.wav" )
INSTALL ( FILES ${files}
DESTINATION ${CMAKE_DATA_PATH}/${APPNAME}/generic/sfx/ )
