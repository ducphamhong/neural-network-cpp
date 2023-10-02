cmake_minimum_required(VERSION 3.1)

# cmake config
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# cmake library
include(CMake/CMakeVisualStudioSourceGroup.cmake)
include(CMake/CMakeVisualStudioPCH.cmake)

# for SDL2
if (MSVC OR CYGWIN OR MINGW)
else()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
endif()

# this line has to appear before 'PROJECT' in order to be able to disable incremental linking
set(MSVC_INCREMENTAL_DEFAULT ON)

# begin project
project(ANN)
file(STRINGS "Version.txt" ANN_VERSION)
message(STATUS "- App version ${ANN_VERSION}")

# output
set(LIBRARY_OUTPUT_PATH ${ANN_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}/Libs CACHE PATH "Single output directory for building all libraries.")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ANN_SOURCE_DIR}/Bin CACHE PATH "Single output directory for building executables.")

# debug/release
if (NOT CMAKE_BUILD_TYPE)
	# set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "- System: ${CMAKE_SYSTEM_NAME} - BuildType: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
endif()

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "Use shared libraries" OFF)

option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)

option(USE_OPENMP "Use openmp for multithread optimize" ON)

option(BUILD_EXAMPLE "Build example application" ON)


if (CMAKE_SYSTEM_NAME STREQUAL Linux)
	set(BUILD_LINUX ON)
	add_definitions(-DLINUX)
endif()

# unicode library
add_definitions(-DUNICODE -D_UNICODE)

# visual studio
if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
	if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		#We statically link to reduce dependancies
		foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
			if (${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif (${flag_var} MATCHES "/MD")
			if (${flag_var} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
			endif (${flag_var} MATCHES "/MDd")
		endforeach()
	endif()	
	
	if (CMAKE_CL_64)
		add_definitions(-D_WIN64)
	endif()

	set(CMAKE_EXE_LINKER_FLAGS	"${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif()

subdirs (Source/ANN)
subdirs (Source/ThirdParty)
subdirs (Source/XOR)
subdirs (Source/DigitClassification)
subdirs (Source/FlappyDoge)
subdirs (Source/Snake)
subdirs (Source/CompareANN)