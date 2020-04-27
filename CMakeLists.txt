cmake_minimum_required(VERSION 3.10)

project(wmt VERSION 1.8.1 DESCRIPTION "Warzone2100 map converting/parsing library built in C++")

option(WMT_BUILD_EXECUTABLE "Build WMT executable" ON)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_COLOR_MAKEFILE   ON)

if(WMT_BUILD_EXECUTABLE)
	add_library(liblog SHARED lib/log.cpp)
	target_compile_definitions(liblog PRIVATE LOG_USE_COLOR)
endif()

include(GNUInstallDirs)
add_library(libwmt SHARED lib/wmt.cpp lib/zip.cpp)
if(WMT_BUILD_EXECUTABLE)
	target_link_libraries(libwmt liblog)
else()
	target_compile_definitions(libwmt PRIVATE _NOLOGBUILD)
endif()
set_target_properties(libwmt PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libwmt PROPERTIES SOVERSION 1)
set_target_properties(libwmt PROPERTIES PUBLIC_HEADER lib/wmt.hpp)
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/.)
install(TARGETS libwmt
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(libwmt.pc.in libwmt.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libwmt.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

if(WMT_BUILD_EXECUTABLE)
	add_executable(wmt src/main.cpp lib/TinyPngOut.cpp)
	target_link_libraries(wmt libwmt)
	target_link_libraries(wmt liblog)
endif()
