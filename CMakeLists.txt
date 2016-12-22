################################################################################
# CMake build script for Lisle.
#
option(LISLE_BUILD_TESTS "Build all of lisle's own tests." OFF)

################################################################################
#
# Project-wide settings
project(Lisle)
set(LISLE_VERSION_MAJOR 5)
set(LISLE_VERSION_MINOR 0)
set(LISLE_VERSION_PATCH 2)
cmake_minimum_required(VERSION 2.8)
enable_testing()

include(cmake/internal_utils.cmake)
config_compiler_and_linker()  # Defined in internal_utils.cmake.

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VERSION_MAJOR ${LISLE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${LISLE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${LISLE_VERSION_PATCH})
include(CPack)

find_package(Threads REQUIRED)

set(BUILD_SYS gcc)
if (MSVC)
	set(BUILD_SYS msvc)
endif ()
if (BUILD_SYS MATCHES gcc)
	set(GCC 1)
endif ()

set(CPU_ARCH ${CMAKE_SYSTEM_PROCESSOR})
if (CPU_ARCH MATCHES "^x86*")
	set(CPU_ARCH x86)
endif ()
if (CPU_ARCH MATCHES "^AMD64*")
	set(CPU_ARCH x86)
endif ()

set(OPERATING_SYS ${THREAD_SYS})
if (UNIX)
	include(CheckLibraryExists)
	CHECK_LIBRARY_EXISTS(rt clock_gettime "time.h" HAVE_CLOCK_GETTIME_IN_RT)
elseif (APPLE)
	set(OPERATING_SYS darwin)
endif ()

set(THREAD_SYS)
if (CMAKE_USE_PTHREADS_INIT)
	set(THREAD_SYS posix)
elseif (CMAKE_USE_WIN32_THREADS_INIT)
	set(THREAD_SYS windows)
endif ()

add_subdirectory(include/lisle)
add_subdirectory(src)
add_subdirectory(port/${THREAD_SYS})
if (LISLE_BUILD_TESTS)
	add_subdirectory(sub/test/googletest)
	add_subdirectory(test)
endif()
