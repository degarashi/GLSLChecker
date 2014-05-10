cmake_minimum_required(VERSION 2.8)
project(glslhl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(Qt5 COMPONENTS Widgets)
find_package(Boost REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS}
					${Boost_INCLUDE_DIRS})

set(SRCS ../syntaxhighlighter.cpp)
add_library(glslhl SHARED ${SRCS})
install(TARGETS glslhl LIBRARY DESTINATION lib)

set(HEADERS ../syntaxhighlighter.h)
install(FILES ${HEADERS} DESTINATION include)

