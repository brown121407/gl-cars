cmake_minimum_required(VERSION 3.23)
project(proj1)

set(CMAKE_CXX_STANDARD 20)

add_executable(proj1 main.cpp)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_link_libraries(proj1 glfw glm::glm GLEW::GLEW)