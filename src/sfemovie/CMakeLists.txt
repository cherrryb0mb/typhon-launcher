project(sfemovie)

SET(SFMSRC
	AudioStream.cpp
	Macros.cpp
	Stream.cpp
	Utilities.cpp
	Demuxer.cpp
	Movie.cpp
	SubtitleStream.cpp
	VideoStream.cpp
	Log.cpp
	MovieImpl.cpp
	Timer.cpp
	)
	
ADD_DEFINITIONS ( -D__STDC_CONSTANT_MACROS)
INCLUDE_DIRECTORIES(${sfemovie_SOURCE_DIR})

add_library(
	sfemovie
	STATIC
	${SFMSRC}
)
