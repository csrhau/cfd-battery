cmake_minimum_required (VERSION 2.8)
project (ccilk C)

find_package(HDF5 COMPONENTS C HL)
include_directories (${HDF5_INCLUDE_DIR})
set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})

message( ${HDF5_LIBRARIES})

add_executable(ccilk main.c)
target_link_libraries (ccilk ${LINK_LIBS})
