# Target for the main
add_executable(main)

target_link_libraries(
	main
	PRIVATE
		compressor-lib
)

target_sources(
	main
	PRIVATE
		"compress.cpp"
)