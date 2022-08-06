cmake_minimum_required(VERSION 3.5)

project(GPSReader LANGUAGES C CXX)
find_package(PkgConfig REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_CXX_STANDARD 11)
set(LIB_SRCS lib/log/src/log.c lib/encoder.cpp lib/socketHandler.cpp)
set(LIBRARY_NAME libs)
add_library(${LIBRARY_NAME} SHARED ${LIB_SRCS})
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/lib/log/src ${PROJECT_SOURCE_DIR}/lib/)
link_directories("${CMAKE_BINARY_DIR}/lib")

add_executable(GPSReader src/GPSReader.cpp )
add_executable(server  src/server.cpp)
target_link_libraries(GPSReader ${LIBRARY_NAME}  )
target_link_libraries(server ${LIBRARY_NAME} -pthread )

