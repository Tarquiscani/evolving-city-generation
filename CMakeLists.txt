cmake_minimum_required(VERSION 3.10.0)
project(evolving_city_generation)


# CMake print any generated command
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 17)



####
#   Define source files.
####

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



####
#   Define targets.
####

option(MULTIPLE_CONFIGURATIONS "Build multiple executable with different graphics qualities." OFF)

if (MULTIPLE_CONFIGURATIONS)
    add_executable(demo_windowed_low_quality ${SOURCES})
    add_executable(demo_windowed_medium_quality ${SOURCES})
    add_executable(demo_windowed_high_quality ${SOURCES})
    add_executable(demo_fullscreen_low_quality ${SOURCES})
    add_executable(demo_fullscreen_medium_quality ${SOURCES})
    add_executable(demo_fullscreen_high_quality ${SOURCES})
    
    set(LOW_QUALITY_BINARIES    demo_windowed_low_quality    demo_fullscreen_low_quality   )
    set(MEDIUM_QUALITY_BINARIES demo_windowed_medium_quality demo_fullscreen_medium_quality)
    set(HIGH_QUALITY_BINARIES   demo_windowed_high_quality   demo_fullscreen_high_quality  )

    set(WINDOWED_BINARIES   demo_windowed_low_quality   demo_windowed_medium_quality   demo_windowed_high_quality  )
    set(FULLSCREEN_BINARIES demo_fullscreen_low_quality demo_fullscreen_medium_quality demo_fullscreen_high_quality)
    
    set(A_BINARY demo_windowed_low_quality)
    set(BINARIES ${WINDOWED_BINARIES} ${FULLSCREEN_BINARIES})
    
    foreach (TARGET ${FULLSCREEN_BINARIES})
        target_compile_definitions(${TARGET} PRIVATE CMAKE_FULLSCREEN=true)  
    endforeach (TARGET)

    foreach (TARGET ${LOW_QUALITY_BINARIES})
        target_compile_definitions(${TARGET} PRIVATE CMAKE_LOW_QUALITY=true)  
    endforeach (TARGET)

    foreach (TARGET ${MEDIUM_QUALITY_BINARIES})
        target_compile_definitions(${TARGET} PRIVATE CMAKE_MEDIUM_QUALITY=true)  
    endforeach (TARGET)

    foreach (TARGET ${HIGH_QUALITY_BINARIES})
        target_compile_definitions(${TARGET} PRIVATE CMAKE_HIGH_QUALITY=true)  
    endforeach (TARGET)

else ()
    add_executable(evolving_city_generation ${SOURCES})
    
    set(A_BINARY evolving_city_generation)
    set(BINARIES evolving_city_generation)
    
    target_compile_definitions(evolving_city_generation PRIVATE CMAKE_HIGH_QUALITY=true)  
endif ()

   

####
#   Link third-party libraries.
####

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif ()

# Setting all options of the GLFW subdirectory CMakeLists
set(GLFW_USE_HYBRID_HPG ON  CACHE BOOL "Force use of high-performance GPU on hybrid systems" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "Generate installation target" FORCE)
set(GLFW_VULKAN_STATIC  OFF CACHE BOOL "Assume the Vulkan loader is linked with the application" FORCE)

add_subdirectory(third_party/glfw)

foreach (TARGET ${BINARIES})
    target_link_libraries(${TARGET} glfw ${GLFW_LIBRARIES})
    set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE OFF)
endforeach (TARGET)



####
#   Copy resources in the executable folder.
####

foreach (TARGET ${BINARIES})
    # Copy the "media" directory into the binary folder after building
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/media"
            "$<TARGET_FILE_DIR:${TARGET}>/media")
        
    # Copy the "shaders" directory into the binary folder after building
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:${TARGET}>/shaders")

    if (MULTIPLE_CONFIGURATIONS)
        # Copy the "README.txt" directory into the binary folder after building
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${PROJECT_SOURCE_DIR}/media/README.txt"
                "$<TARGET_FILE_DIR:${TARGET}>/")
    endif ()
endforeach (TARGET)

       
        
####
#   Compiler-specific directives.
####

if (MSVC)
    foreach (TARGET ${BINARIES})
        # warning level 4 and all warnings as errors
        #target_compile_options(evolving_city_generation PRIVATE /W4 /WX)
        target_compile_options(${TARGET} PRIVATE /W4 )
        # Tell MSVC to use main instead of WinMain and hide the console.

        if (MULTIPLE_CONFIGURATIONS)
            set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
        endif ()
    endforeach (TARGET)

else ()
    foreach (TARGET ${BINARIES})
        #target_link_libraries(evolving_city_generation glfw3 -lGL -lGLU -lm -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)
        # lots of warnings and all warnings as errors
        #[[target_compile_options(evolving_city_generation PRIVATE -Wall -Wextra -pedantic -Werror -no-pie)]]
        target_link_options(${TARGET} PRIVATE -no-pie)
    endforeach (TARGET)
endif ()

