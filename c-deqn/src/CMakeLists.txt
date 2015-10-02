cmake_minimum_required (VERSION 2.8)
project (cdeqn C)


find_package(HDF5 COMPONENTS C HL)
include_directories (${HDF5_INCLUDE_DIR})
set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_BUILD_TYPE Release)


message( ${HDF5_LIBRARIES})

add_executable(cdeqn main.c)
target_link_libraries (cdeqn ${LINK_LIBS})
