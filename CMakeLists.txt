cmake_minimum_required(VERSION 2.8)

project( enkiMI )

include_directories ("${PROJECT_SOURCE_DIR}/src")

set( ENKIMI_SRC
     example/example.c
     src/miniz.h
	 src/miniz.c
	 src/enkimi.h
	 src/enkimi.c
	 )
	 
add_executable( example ${ENKIMI_SRC} )
