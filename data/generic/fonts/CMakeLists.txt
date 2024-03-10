FILE ( GLOB files "*.ttf" )
INSTALL ( FILES ${files}
DESTINATION ${CMAKE_DATA_PATH}/${APPNAME}/generic/fonts/ )
