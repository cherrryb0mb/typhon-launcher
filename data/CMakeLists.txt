ADD_SUBDIRECTORY ( themedata )
ADD_SUBDIRECTORY ( themes )
ADD_SUBDIRECTORY ( generic )
INSTALL ( FILES themedata/typhon/typhon.png DESTINATION ${CMAKE_INSTALL_ICONDIR}/ )
INSTALL ( FILES typhon.desktop	DESTINATION ${CMAKE_INSTALL_DESKTOPDIR}/ )


