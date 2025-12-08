cmake_minimum_required(VERSION 3.10)

project(minimax LANGUAGES CXX)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL GLEW::GLEW glfw)
