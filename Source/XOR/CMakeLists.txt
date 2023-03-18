include_directories(
	${ANN_SOURCE_DIR}/Source/ANN/Source	
	${ANN_SOURCE_DIR}/Source/XOR/Source
)

file(GLOB_RECURSE application_source
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC OR CYGWIN OR MINGW)
	add_executable(XOR ${application_source})
endif()

target_precompiled_header(XOR ${ANN_SOURCE_DIR}/Source/XOR/Source/pch.cpp ${application_source})

# Linker
target_link_libraries(XOR ANN)

set_target_properties(XOR PROPERTIES VERSION ${ANN_VERSION})
set_target_properties(XOR PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")