project(gource)

SET(GOURCESRC
	fxfont.cpp
	logger.cpp
	resource.cpp
	)

add_library(
	gource
	STATIC
	${GOURCESRC}
)
