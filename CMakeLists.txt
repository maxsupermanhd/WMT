cmake_minimum_required(VERSION 3.10)

project(wmt VERSION 1.8.3 DESCRIPTION "Warzone2100 map converting/parsing library built in C++")


option(WMT_BUILD_EXECUTABLE "Build WMT executable" ON)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_COLOR_MAKEFILE   ON)

if(WMT_BUILD_EXECUTABLE)
	add_library(liblog SHARED lib/log/log.cpp)
	target_compile_definitions(liblog PRIVATE LOG_USE_COLOR)
	install(TARGETS liblog
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			COMPONENT library)
endif()

include(GNUInstallDirs)
add_library(libwmt SHARED src/wmt.cpp lib/zip/zip.cpp)
set_target_properties(libwmt PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libwmt PROPERTIES SOVERSION 1)
set_target_properties(libwmt PROPERTIES PUBLIC_HEADER include/wmt.hpp)
if(WMT_BUILD_EXECUTABLE)
	target_link_libraries(libwmt liblog)
	target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/log)
else()
	target_compile_definitions(libwmt PUBLIC _NOLOGBUILD)
endif()
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/zip)
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/json)
target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/ini)
install(TARGETS libwmt
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		COMPONENT library)
configure_file(libwmt.pc.in libwmt.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/libwmt.pc
		DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

if(WMT_BUILD_EXECUTABLE)
	add_executable(wmt src/main.cpp lib/png/TinyPngOut.cpp)
	target_include_directories(libwmt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/png)
	target_link_libraries(wmt libwmt)
	target_link_libraries(wmt liblog)
	install(TARGETS wmt
			RUNTIME
			DESTINATION bin
			COMPONENT applications)
endif()


set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6, libgcc1, libc6")
set(CPACK_PACKAGE_CONTACT "q3.max.2011@yandex.ru")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Maxim Zhuchkov")
set(CPACK_COMPONENTS_ALL library applications)
include(CPack)