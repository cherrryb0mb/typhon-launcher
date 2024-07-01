# options:
# -DDEBUG=ON					# enable debug
# -DNOSFM=ON					# disable video support (uses sfe-movie atm)
# -DNOMMD=ON					# disable mmd model support (requires libmmd and bullet)
# -DNOSCROT=ON					# disable screenshot support (requires jpeg and Xutils.h)

# cmake built in:
# -DCMAKE_INSTALL_PREFIX 		# default is -DCMAKE_INSTALL_PREFIX="/usr/local/"
# -DCMAKE_DATA_PATH				# default is ${CMAKE_INSTALL_PREFIX}/share/
# -DCMAKE_INSTALL_DESKTOPDIR	# default is ${CMAKE_INSTALL_PREFIX}/share/applications
# -DCMAKE_INSTALL_ICONDIR 		# default is ${CMAKE_INSTALL_PREFIX}/share/pixmaps


PROJECT (typhon)
SET ( APPNAME typhon )

cmake_minimum_required(VERSION 2.8)

SET ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" )
SET ( CMAKE_INSTALL_BINDIR bin )

find_package(SFML REQUIRED audio graphics window system network)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(TinyXML REQUIRED)
find_package(PNG REQUIRED)
find_package(P3T)
find_package(Threads REQUIRED)
find_package(GLEW REQUIRED)
find_package(JPEG REQUIRED)
find_package(Bullet REQUIRED)
find_package(ImageMagick COMPONENTS convert)
find_package(ImageMagick COMPONENTS mogrify)

IF ( NOT NOSFM )
find_package(sfemovie)
IF(SFEMOVIE_FOUND)
ELSE(SFEMOVIE_FOUND)
find_package(FFmpeg REQUIRED AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE)
ENDIF(SFEMOVIE_FOUND)
ENDIF ( NOT NOSFM )

IF ( NOT NOMMD )
find_package(MMD)
find_package(Bullet)
ENDIF ( NOT NOMMD )

IF ( NOT NOSCROT )
ADD_DEFINITIONS ( -DWITHSCROT )
ENDIF ( NOT NOSCROT )

#IGNORE SET ( CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "installpath prefix" )
SET ( CMAKE_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/" CACHE PATH "datapath prefix" )
SET ( CMAKE_INSTALL_ICONDIR "${CMAKE_INSTALL_PREFIX}/share/pixmaps" CACHE PATH "iconpath prefix" )
SET ( CMAKE_INSTALL_DESKTOPDIR "${CMAKE_INSTALL_PREFIX}/share/applications" CACHE PATH "desktopfilepath prefix")

# one of my machines requires -O0 instead of -O1 and -O2 , else videodisplay is strange - hints?

IF ( DEBUG )
	SET ( CMAKE_CXX_FLAGS "-g -pipe -ggdb")
	ADD_DEFINITIONS ( -DDEBUG )
ENDIF ( DEBUG )

ADD_DEFINITIONS ( -DPKGDATADIR="${CMAKE_DATA_PATH}/${APPNAME}" )

INCLUDE_DIRECTORIES	(
			${FREETYPE_INCLUDE_DIRS} 
			${OPENGL_INCLUDE_DIR}
			${SFML_INCLUDE_DIR}
			${PNG_INCLUDE_DIR}
			${ZLIB_INCLUDE_DIRS}
			${TINYXML_INCLUDE_DIR}
			${GLEW_INCLUDE_DIR}
			${JPEG_INCLUDE_DIR}
			${BULLET_INCLUDE_DIR}
			)

SET(EXTRA_LIBS 
	${OPENGL_LIBRARIES}
	${FREETYPE_LIBRARY}
	${SFML_LIBRARIES}
	${ZLIB_LIBRARIES}
	${PNG_LIBRARIES}
	${TINYXML_LIBRARIES}
	${GLEW_LIBRARY}
	${JPEG_LIBRARY}
	${BULLET_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

SET(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")

add_subdirectory(src)

#install
add_subdirectory(data)
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/LICENSE
	DESTINATION ${CMAKE_DATA_PATH}/${APPNAME}/ )
INSTALL ( FILES ${CMAKE_SOURCE_DIR}/README.md
	DESTINATION ${CMAKE_DATA_PATH}/${APPNAME}/ )


MESSAGE(STATUS "<<< Configuration >>>
Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}
Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}
Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n")
