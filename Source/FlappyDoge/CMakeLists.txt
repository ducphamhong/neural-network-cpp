include_directories(
	${ANN_SOURCE_DIR}/Source/ANN/Source
	${ANN_SOURCE_DIR}/Source/ThirdParty/Source
	${ANN_SOURCE_DIR}/Source/FlappyDoge/Source
	${ANN_SOURCE_DIR}/Source/SDK/SDL2/include
	${ANN_SOURCE_DIR}/Source/SDK/SDL2_ttf/include
	${ANN_SOURCE_DIR}/Source/SDK/SDL2_image/include
	${ANN_SOURCE_DIR}/Source/SDK/SDL2_mixer/include
)

file(GLOB_RECURSE application_source
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC OR CYGWIN OR MINGW)
	#SDL
	if (CMAKE_CL_64)
		set(sdl_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2/lib/x64")
		set(sdl_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2/lib/x64/*.dll")
		
		set(sdl_ttf_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_ttf/lib/x64")
		set(sdl_ttf_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_ttf/lib/x64/*.dll")
		
		set(sdl_image_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_image/lib/x64")
		set(sdl_image_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_image/lib/x64/*.dll")
		
		set(sdl_mixer_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_mixer/lib/x64")
		set(sdl_mixer_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_mixer/lib/x64/*.dll")
		
	else()
		set(sdl_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2/lib/x86")
		set(sdl_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2/lib/x86/*.dll")
		
		set(sdl_ttf_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_ttf/lib/x86")
		set(sdl_ttf_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_ttf/lib/x86/*.dll")
		
		set(sdl_image_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_image/lib/x64")
		set(sdl_image_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_image/lib/x64/*.dll")
		
		set(sdl_mixer_lib_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_mixer/lib/x64")
		set(sdl_mixer_dll_path "${ANN_SOURCE_DIR}/Source/SDK/SDL2_mixer/lib/x64/*.dll")
	endif()
else()
	include(findpkgconfig)
	pkg_search_module(sdl2 required sdl2)
	pkg_search_module(sdl2image required sdl2_image)
	pkg_search_module(sdl2ttf required sdl2_ttf)
	pkg_search_module(sdl2mixer required sdl2_mixer)
	
	include_directories(${sdl2_include_dirs} ${sdl2image_include_dirs} ${sdl2ttf_include_dirs} ${sdl2mixer_include_dirs})
endif()

add_executable(FlappyDoge WIN32 ${application_source})

set_target_properties(FlappyDoge PROPERTIES VERSION ${ANN_VERSION})
set_target_properties(FlappyDoge PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Linker
target_link_libraries(FlappyDoge ANN ThirdParty)

if (MSVC OR CYGWIN OR MINGW)
	target_link_libraries(FlappyDoge ${sdl_lib_path}/SDL2.lib)
	target_link_libraries(FlappyDoge ${sdl_ttf_lib_path}/SDL2_ttf.lib)
	target_link_libraries(FlappyDoge ${sdl_image_lib_path}/SDL2_image.lib)
	target_link_libraries(FlappyDoge ${sdl_mixer_lib_path}/SDL2_mixer.lib)

	file(GLOB_RECURSE sdl_dll_files ${sdl_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET FlappyDoge POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:FlappyDoge>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_ttf_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET FlappyDoge POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:FlappyDoge>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_image_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET FlappyDoge POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:FlappyDoge>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_mixer_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET FlappyDoge POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:FlappyDoge>)
	endforeach()
else()
	target_link_libraries(FlappyDoge ${sdl2_libraries} ${sdl2image_libraries} ${sdl2ttf_libraries} ${sdl2mixer_libraries})
endif()