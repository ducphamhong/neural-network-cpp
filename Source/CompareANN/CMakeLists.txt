include_directories(
	${ANN_SOURCE_DIR}/Source/ANN/Source	
	${ANN_SOURCE_DIR}/Source/CompareANN/Source
)

file(GLOB_RECURSE application_source
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(CompareANN ${application_source})

target_precompiled_header(CompareANN ${ANN_SOURCE_DIR}/Source/CompareANN/Source/pch.cpp ${application_source})

# Linker
target_link_libraries(CompareANN ANN)

set_target_properties(CompareANN PROPERTIES VERSION ${ANN_VERSION})
set_target_properties(CompareANN PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")