include_directories(
	${ANN_SOURCE_DIR}/Source/ANN/Source
)

file(GLOB_RECURSE application_source
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ANN STATIC
	${application_source}
)

target_precompiled_header(ANN ${ANN_SOURCE_DIR}/Source/ANN/Source/pch.cpp ${application_source})

set_target_properties(ANN PROPERTIES VERSION ${ANN_VERSION})
set_target_properties(ANN PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")