cmake_minimum_required(VERSION 2.8)

project( enkiMI )

#now our enkiMI target is available to everything below
add_subdirectory(src)
option(ENKIMI_ENABLE_EXAMPLE "Enable the enkiMI project example executable target" OFF)
if(ENKIMI_ENABLE_EXAMPLE)
	add_subdirectory(example)
endif()
