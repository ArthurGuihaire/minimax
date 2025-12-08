cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(renderer STATIC ${SOURCES})
target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(renderer PRIVATE OpenGL::GL GLEW::GLEW glfw)

