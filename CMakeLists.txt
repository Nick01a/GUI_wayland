
cmake_minimum_required(VERSION 3.5)
project(untitled)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS -std=c++11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

find_package(Wayland REQUIRED) # https://github.com/LunarG/VulkanSamples/blob/master/cmake/FindWayland.cmake
include_directories(${WAYLAND_CLIENT_INCLUDE_DIR} ${WAYLAND_EGL_INCLUDE_DIR})

find_package(EGL REQUIRED) # https://github.com/WebKit/webkit/blob/master/Source/cmake/FindEGL.cmake
include_directories(${EGL_INCLUDE_DIRS})

find_package(Cairo REQUIRED) # https://github.com/adobe/webkit/blob/master/Source/cmake/FindCairo.cmake
include_directories(${CAIRO_INCLUDE_DIRS})

SET(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(Pango REQUIRED) # https://android.googlesource.com/platform/external/webkit/+/android-4.3_r2.2/Source/cmake/FindPango.cmake
include_directories( ${OPENGL_INCLUDE_DIRS} ${Pango_INCLUDE_DIRS} )

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED gobject-2.0)
include_directories(${GLIB_INCLUDE_DIRS})

if(NOT DEFINED PATH_TOWAYLAND_PROTOCOLS)
    SET(PATH_TOWAYLAND_PROTOCOLS /usr/share/wayland-protocols)
endif()
add_custom_command(OUTPUT xdg-shell-client-protocol.c
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE}  private-code ${PATH_TOWAYLAND_PROTOCOLS}/stable/xdg-shell/xdg-shell.xml
        ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.c )
add_custom_command(OUTPUT xdg-shell-client-protocol.h
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE}  client-header ${PATH_TOWAYLAND_PROTOCOLS}/stable/xdg-shell/xdg-shell.xml
        ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h )

add_custom_command(OUTPUT xdg-decoration-unstable-v1.c
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE}  private-code ${PATH_TOWAYLAND_PROTOCOLS}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
        ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-unstable-v1.c )
add_custom_command(OUTPUT xdg-decoration-unstable-v1.h
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE}  client-header ${PATH_TOWAYLAND_PROTOCOLS}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
        ${CMAKE_CURRENT_BINARY_DIR}/xdg-decoration-unstable-v1.h )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

SET(ALL_LIBS ${WAYLAND_CLIENT_LIBRARIES} ${CAIRO_LIBRARIES} ${EGL_LIBRARIES}  ${WAYLAND_EGL_LIBRARIES}
        ${OPENGL_LIBRARIES} ${Pango_LIBRARY} -lpangocairo-1.0  -lpthread  ${GLIB_LDFLAGS} )

add_executable(test main.cpp)
target_link_libraries(test ${ALL_LIBS})
