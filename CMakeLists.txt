cmake_minimum_required(VERSION 3.20)
project(polygons_glfw_glad LANGUAGES CXX)

# c++17
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)

set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_API "gl=3.3" CACHE STRING "" FORCE)
set(GLAD_GENERATOR "c" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(glad)


file(GLOB SRC_FILES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(app ${SRC_FILES})

target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(app PRIVATE glad glfw)


if(UNIX AND NOT APPLE)
  find_package(OpenGL REQUIRED)
  target_link_libraries(app PRIVATE OpenGL::GL)
endif()

if(WIN32)
  target_compile_definitions(app PRIVATE NOMINMAX)
endif()

if (MSVC)
  target_compile_options(app PRIVATE /W4 /permissive-)
else()
  target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
endif()
