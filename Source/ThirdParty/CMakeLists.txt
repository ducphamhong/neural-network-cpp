include_directories(
	${ANN_SOURCE_DIR}/Source/ThirdParty/Source
	${ANN_SOURCE_DIR}/Source/ThirdParty/Source/zlib
)

#libpng
file(GLOB_RECURSE libpng_source 
	./Source/libpng/**.cpp 
	./Source/libpng/**.hpp 
	./Source/libpng/**.c 
	./Source/libpng/**.h)
setup_project_group("${libpng_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#json
file(GLOB_RECURSE json_source 
	./Source/json/**.cpp 
	./Source/json/**.hpp 
	./Source/json/**.c 
	./Source/json/**.h)
setup_project_group("${json_source}" ${CMAKE_CURRENT_SOURCE_DIR})

#zlib
file(GLOB_RECURSE zlib_source 
	./Source/zlib/**.cpp 
	./Source/zlib/**.hpp 
	./Source/zlib/**.c 
	./Source/zlib/**.h)
setup_project_group("${zlib_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(ThirdParty STATIC	
	${json_source}
	${libpng_source}
	${zlib_source}
)

set_target_properties(ThirdParty PROPERTIES VERSION ${ANN_VERSION})