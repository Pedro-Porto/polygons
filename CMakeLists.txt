cmake_minimum_required(VERSION 3.5)
project(polygons_glfw_glad LANGUAGES C CXX)

# c++17
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

# ---------------- GLFW ----------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# ---------------- GLAD local ----------------
# Sua estrutura:
# external/glad/include/glad/glad.h
# external/glad/include/KHR/khrplatform.h
# external/glad/src/glad.c

set(GLAD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/glad/include")
set(GLAD_SOURCE      "${CMAKE_SOURCE_DIR}/external/glad/src/glad.c")

# ---------------- Sources ----------------
file(GLOB SRC_FILES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(app
  ${SRC_FILES}
  ${GLAD_SOURCE}   # compila o glad.c junto
)

target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${GLAD_INCLUDE_DIR}   # headers do glad
)

target_link_libraries(app PRIVATE glfw)

if(UNIX AND NOT APPLE)
  find_package(OpenGL REQUIRED)
  target_link_libraries(app PRIVATE OpenGL::GL dl)
endif()

if(WIN32)
  target_compile_definitions(app PRIVATE NOMINMAX)
endif()

if (MSVC)
  target_compile_options(app PRIVATE /W4 /permissive-)
else()
  target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
endif()
