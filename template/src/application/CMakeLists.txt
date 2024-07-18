# Target for the simulator
add_executable(main)

target_link_libraries(
	main
	PRIVATE
		compressor-lib
		md5-lib
)

target_sources(
	main
	PRIVATE
		"main.cpp"
)
