# Set up project
cmake_minimum_required(VERSION 2.6)
project(PopFilter)
message(STATUS "==== Running cmake ====")

# Source files
set(SRC_FILES
	audio.cpp
	fourier.cpp
	main.cpp
	util/args_parser.cpp
	util/ringbuffer.cpp
	util/unittest.cpp
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Libraries
find_package(Vorbis REQUIRED)
find_package(Threads REQUIRED) # pthread

# Compiler specific configurations
if (MSVC)

else ()
	# -pg -g -O0
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wabi")
endif ()

# Include Directories
include_directories(
	${PROJECT_SOURCE_DIR}
	${OGG_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(
	${PROJECT_NAME}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
	Threads::Threads
)
