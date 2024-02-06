# Target for the expression processing library
add_library(compressor-lib STATIC)

target_sources(
	compressor-lib
	PRIVATE
		"Encoder.h"
		"Encoder.cpp"
		"Decoder.h"
		"Decoder.cpp"
)
