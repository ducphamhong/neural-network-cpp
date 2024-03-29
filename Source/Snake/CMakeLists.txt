include_directories(
	${ANN_SOURCE_DIR}/Source/ANN/Source
	${ANN_SOURCE_DIR}/Source/ThirdParty/Source
	${ANN_SOURCE_DIR}/Source/Snake/Source
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
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(SDL2 REQUIRED sdl2)
	pkg_check_modules(SDL2Image REQUIRED SDL2_image)
	pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
	pkg_check_modules(SDL2Mixer REQUIRED SDL2_mixer)
	
	include_directories(${sdl2_INCLUDE_DIRS} ${sdl2image_INCLUDE_DIRS} ${sdl2ttf_INCLUDE_DIRS} ${SDLMixer_INCLUDE_DIRS})
endif()

add_executable(Snake WIN32 ${application_source})


# target_precompiled_header(Snake ${ANN_SOURCE_DIR}/Source/Snake/Source/pch.cpp ${application_source})

set_target_properties(Snake PROPERTIES VERSION ${ANN_VERSION})
set_target_properties(Snake PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Linker
target_link_LIBRARIES(Snake ANN ThirdParty)

if (MSVC OR CYGWIN OR MINGW)
	target_link_LIBRARIES(Snake ${sdl_lib_path}/SDL2.lib)
	target_link_LIBRARIES(Snake ${sdl_ttf_lib_path}/SDL2_ttf.lib)
	target_link_LIBRARIES(Snake ${sdl_image_lib_path}/SDL2_image.lib)
	target_link_LIBRARIES(Snake ${sdl_mixer_lib_path}/SDL2_mixer.lib)

	file(GLOB_RECURSE sdl_dll_files ${sdl_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET Snake POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:Snake>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_ttf_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET Snake POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:Snake>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_image_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET Snake POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:Snake>)
	endforeach()

	file(GLOB_RECURSE sdl_dll_files ${sdl_mixer_dll_path})
	foreach(dll ${sdl_dll_files})
		add_custom_command(TARGET Snake POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:Snake>)
	endforeach()
else()
	message(STATUS "Snake LINK ${SDL2_LINK_LIBRARIES}")
	message(STATUS "Snake LINK ${SDL2Image_LINK_LIBRARIES}")
	message(STATUS "Snake LINK ${SDL2TTF_LINK_LIBRARIES}")
	message(STATUS "Snake LINK ${SDL2Mixer_LINK_LIBRARIES}")
	
	target_link_LIBRARIES(Snake ${SDL2_LINK_LIBRARIES} ${SDL2Image_LINK_LIBRARIES} ${SDL2TTF_LINK_LIBRARIES} ${SDL2Mixer_LINK_LIBRARIES})
endif()