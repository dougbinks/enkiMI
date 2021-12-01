cmake_minimum_required(VERSION 2.8)

project( enkiMI )
#for backward compatsake, using capital enkiMI, convention though is usually lowercase
add_library(enkiMI)
target_sources(enkiMI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/miniz.h
        ${CMAKE_CURRENT_SOURCE_DIR}/miniz.c
        ${CMAKE_CURRENT_SOURCE_DIR}/enkimi.h
        ${CMAKE_CURRENT_SOURCE_DIR}/enkimi.c
        )
#if we don't use current source directory, we can't use this project as a submodule
target_include_directories(enkiMI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)


add_library(enkiMI::enkimi ALIAS enkiMI)