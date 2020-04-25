cmake_minimum_required(VERSION 3.9)

project(wmt VERSION 1.8.1 DESCRIPTION "Warzone2100 map converting/parsing library built in C++")

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_COLOR_MAKEFILE   ON)

include(GNUInstallDirs)
add_library(libwmt SHARED lib/wmt.cpp lib/zip.cpp)
set_target_properties(libwmt PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libwmt PROPERTIES SOVERSION 1)
set_target_properties(libwmt PROPERTIES PUBLIC_HEADER lib/wmt.hpp)
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/.)
install(TARGETS libwmt
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(libwmt.pc.in libwmt.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libwmt.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_library(libpng STATIC lib/TinyPngOut.cpp)
add_library(liblog STATIC lib/log.cpp)

add_executable(wmt src/main.cpp)
target_link_libraries(wmt libwmt)
target_link_libraries(wmt libpng)
target_link_libraries(wmt liblog)
