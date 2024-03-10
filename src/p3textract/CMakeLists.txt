project(p3textract)

SET(P3TSRC
	gim.cpp
	main.cpp
	p3t.cpp
	vag2wav.cpp
	)

ADD_DEFINITIONS ( -DP3TLIB )

add_library(
	p3textract
	STATIC
	${P3TSRC}
)
