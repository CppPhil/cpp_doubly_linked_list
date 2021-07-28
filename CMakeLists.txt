cmake_minimum_required(VERSION 3.15)

enable_testing()

project(doubly_linked_list_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    string(APPEND CMAKE_CXX_FLAGS "/W4")
  endif()

  string(
    APPEND
    CMAKE_CXX_FLAGS
    " /DNOMINMAX /D_UNICODE /DUNICODE /DCPP20"
  )

  string(APPEND CMAKE_CXX_FLAGS_DEBUG " /Od /DDEBUG_MODE")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " /O2 /DNDEBUG /DRELEASE_MODE /D_ITERATOR_DEBUG_LEVEL=1")
else()
  string(
    APPEND
    CMAKE_CXX_FLAGS
    " -Werror -Wall -Wextra -Wstrict-aliasing -Wcast-align -Wmissing-include-dirs -Wpointer-arith -Wunreachable-code -Wunused -Wunused-parameter"
  )

  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g -O0 -DDEBUG_MODE")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " -g -O3 -DNDEBUG -DRELEASE_MODE")
endif()

set(APP_NAME doubly_linked_list_app)

set(
  HEADERS
  include/list.hpp
)

set(
  SOURCES
  src/main.cpp
)

add_executable(${APP_NAME} ${HEADERS} ${SOURCES})

target_include_directories(
  ${APP_NAME} 
  PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
