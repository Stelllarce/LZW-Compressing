# Target for the expression processing library
add_library(compressor-lib STATIC)

target_link_libraries(
	compressor-lib
	PRIVATE
		md5-lib
)

target_sources(
	compressor-lib
	PRIVATE
		"Encoder.h"
		"Encoder.cpp"
		"Decoder.h"
		"Decoder.cpp"
		"Archiver.h"
		"Archiver.cpp"
		"Interface.h"
		"Interface.cpp"
		"Path.h"
		"Path.cpp"
		"Hasher.h"
		"Hasher.cpp"
)
