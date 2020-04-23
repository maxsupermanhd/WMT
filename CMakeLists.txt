cmake_minimum_required(VERSION 3.9)

project(wmt VERSION 1.8.1 DESCRIPTION "Warzone2100 map converting/parsing library built in C++")

include(GNUInstallDirs)
add_library(wmt SHARED lib/wmt.cpp)
set_target_properties(wmt PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(wmt PROPERTIES SOVERSION 1)
set_target_properties(wmt PROPERTIES PUBLIC_HEADER lib/wmt.hpp)
target_include_directories(wmt PRIVATE lib/.)
install(TARGETS wmt
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(wmt.pc.in wmt.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/wmt.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
