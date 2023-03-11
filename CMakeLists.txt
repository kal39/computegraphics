cmake_minimum_required(VERSION 3.20)

project(computegraphics)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/glfw)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(computegraphics ${SRC_FILES})
target_include_directories(computegraphics PRIVATE src)
target_include_directories(computegraphics PRIVATE deps/k_tools)
target_include_directories(computegraphics PRIVATE deps/glfw/include)
target_link_libraries(computegraphics PRIVATE OpenGL::OpenGL glfw)