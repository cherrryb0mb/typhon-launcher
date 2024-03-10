FILE ( GLOB files "*.sfx" )
INSTALL ( FILES ${files}
DESTINATION ${CMAKE_DATA_PATH}/${APPNAME}/generic/backshader/ )
