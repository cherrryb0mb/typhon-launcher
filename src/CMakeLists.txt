cmake_minimum_required(VERSION 2.8)
PROJECT (typhon)
SET(SOURCES
	cmdlinehandling.cpp
	confighandling.cpp
	editor.cpp
	emuhandling.cpp
	filehandling.cpp
	fonthandling.cpp
	glhandling.cpp
	glmenu.cpp
	inputhandling.cpp
	mmdhandling.cpp
	networking.cpp
	p3thandling.cpp
	picturehandling.cpp
	rsshandling.cpp
	shaderhandling.cpp
	sound.cpp
	systeminfo.cpp
	TConfig.cpp
	themehandling.cpp
	timerfunc.cpp
	typhon.cpp
	values.cpp
	videoplayerhandling.cpp
	windowhandling.cpp
	xmlconfig.cpp
	)

SET ( SRC_compile ${SOURCES} )
ADD_EXECUTABLE ( ${APPNAME} ${SOURCES} )
TARGET_LINK_LIBRARIES( ${APPNAME} ${EXTRA_LIBS} )

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries(${APPNAME} ${X11_LIBRARIES} ${EXTRA_LIBS})

IF(P3T_FOUND)
INCLUDE_DIRECTORIES( ${P3T_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES( ${APPNAME} ${EXTRA_LIBS} ${P3T_LIBRARY})
ELSE(P3T_FOUND)
MESSAGE(STATUS "system libp3t not found - linking statically!")
INCLUDE_DIRECTORIES(${typhon_SOURCE_DIR}/p3textract)
add_subdirectory(p3textract)
TARGET_LINK_LIBRARIES( ${APPNAME} p3textract ${EXTRA_LIBS} )
ENDIF(P3T_FOUND)

INCLUDE_DIRECTORIES(${typhon_SOURCE_DIR}/gource)
add_subdirectory(gource)
TARGET_LINK_LIBRARIES( ${APPNAME} gource ${EXTRA_LIBS} )

IF ( NOMMD )
ELSE ( NOMMD )
IF(MMD_FOUND)
INCLUDE_DIRECTORIES( ${MMD_INCLUDE_DIR} )
ADD_DEFINITIONS ( -DWITHMMD )
TARGET_LINK_LIBRARIES( ${APPNAME} ${EXTRA_LIBS} ${MMD_LIBRARY} )
IF(BULLET_FOUND)
INCLUDE_DIRECTORIES( ${BULLET_INCLUDE_DIRS})
ENDIF(BULLET_FOUND)
ELSE(MMD_FOUND)
MESSAGE(STATUS "WARNING - libmmd not found - disabling modelviewer support!")
ENDIF(MMD_FOUND)
ENDIF ( NOMMD )

IF ( NOSFM )
ELSE ( NOSFM )
IF(SFEMOVIE_FOUND)
ADD_DEFINITIONS ( -DWITHSFM )
INCLUDE_DIRECTORIES( ${SFEMOVIE_INCLUDE_DIR})
TARGET_LINK_LIBRARIES( ${APPNAME} ${EXTRA_LIBS} ${SFEMOVIE_LIBRARY})
ELSE(SFEMOVIE_FOUND)
IF(FFMPEG_FOUND)
MESSAGE(STATUS "system sfe-movie not found - linking statically!")
ADD_DEFINITIONS ( -DWITHSFM )
add_subdirectory(sfemovie)
INCLUDE_DIRECTORIES(${typhon_SOURCE_DIR}/sfemovie  ${FFMPEG_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES( ${APPNAME} sfemovie ${EXTRA_LIBS} ${FFMPEG_LIBRARIES})
ELSE(FFMPEG_FOUND)
MESSAGE(STATUS "WARNING - ffmpeg not found - disabling sfemovie support!")
ENDIF(FFMPEG_FOUND)
ENDIF(SFEMOVIE_FOUND)
ENDIF ( NOSFM )

IF(ImageMagick_mogrify_FOUND)
IF(ImageMagick_convert_FOUND)
ADD_DEFINITIONS ( -DWITHIM )
ENDIF(ImageMagick_convert_FOUND)
ENDIF(ImageMagick_mogrify_FOUND)

# install binary

INSTALL ( TARGETS ${APPNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )

