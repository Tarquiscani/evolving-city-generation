cmake_minimum_required(VERSION 3.10.0)
project (evolving_city_generation)


# CMake print any generated command
set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 17)


find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif()

set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "Force use of high-performance GPU on hybrid systems" FORCE)
add_subdirectory(third_party/glfw)


include_directories(
	src
	third_party
    third_party/earcut.hpp/include
    third_party/flatbuffers/include
	third_party/glad
	third_party/glfw/include
	third_party/glm
	third_party/imgui)


file(GLOB_RECURSE MAIN_SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"  #TODO: Remove this line after converting all the source files to *.cpp
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp"
)

file(GLOB IMGUI_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/*.cpp"   # Only add main .cpp files in the main directory
)

set(SOURCES ${MAIN_SOURCES} ${IMGUI_SOURCES})

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



add_executable(evolving_city_generation ${SOURCES})


# Copy the "media" directory into the binary folder after building
add_custom_command(TARGET evolving_city_generation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/media"
        "$<TARGET_FILE_DIR:evolving_city_generation>/media")
        
# Copy the "shaders"" directory into the binary folder after building
add_custom_command(TARGET evolving_city_generation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:evolving_city_generation>/shaders")



if (MSVC)
	target_link_libraries(evolving_city_generation glfw ${GLFW_LIBRARIES})
    
    # warning level 4 and all warnings as errors
    #target_compile_options(evolving_city_generation PRIVATE /W4 /WX)
    target_compile_options(evolving_city_generation PRIVATE /W4 )
else()
	target_link_libraries(evolving_city_generation glfw ${GLFW_LIBRARIES})
	#target_link_libraries(evolving_city_generation glfw3 -lGL -lGLU -lm -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)
    # lots of warnings and all warnings as errors
    #[[target_compile_options(evolving_city_generation PRIVATE -Wall -Wextra -pedantic -Werror)]]
endif()

