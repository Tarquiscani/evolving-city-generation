cmake_minimum_required(VERSION 3.10.0)
project (proc_city_gen)


# CMake print any generated command
set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 17)

#add_custom_command(
#  	OUTPUT some_file.txt
#	COMMAND export DISPLAY=:0
#)
# create a target that includes some_file, this gives us a name that we can use later
#add_custom_target(
#  some_target
#  DEPENDS some_file.txt
#)

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif()

add_subdirectory(third_party/glfw)


include_directories(
	src
	third_party
	third_party/glad
	third_party/imgui)


file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"  #TODO: Remove this line after converting all the source files to *.cpp
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui_impl/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image/*.cpp")

#[[
foreach(file ${SOURCES})
    message(${file})
endforeach()
]]#

list(FILTER SOURCES EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/src/__templates/.*$")
list(FILTER SOURCES EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/src/_tests/.*$")


# Log the source files after the filter has been applied (useful to see if it works)
#[[
message("------------------------------------------------")
foreach(file ${SOURCES})
    message(${file})
endforeach()
message("------------------------------------------------")
]]



add_executable(proc_city_gen ${SOURCES})
#add_dependencies(proc_city_gen some_target)


# Copy the "media" directory into the binary folder after building
add_custom_command(TARGET proc_city_gen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/media"
        "$<TARGET_FILE_DIR:proc_city_gen>/media")
        
# Copy the "shaders"" directory into the binary folder after building
add_custom_command(TARGET proc_city_gen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:proc_city_gen>/shaders")



if (MSVC)
	target_link_libraries(proc_city_gen glfw ${GLFW_LIBRARIES})

    # warning level 4 and all warnings as errors
    #target_compile_options(proc_city_gen PRIVATE /W4 /WX)
    target_compile_options(proc_city_gen PRIVATE /W4 )
else()
	target_link_libraries(proc_city_gen glfw3 -lGL -lGLU -lm -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)
    # lots of warnings and all warnings as errors
    #[[target_compile_options(proc_city_gen PRIVATE -Wall -Wextra -pedantic -Werror)]]
endif()

