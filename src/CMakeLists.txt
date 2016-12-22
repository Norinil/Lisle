cmake_minimum_required(VERSION 2.8)

if (GCC)
	add_definitions(-std=c++0x)
endif ()

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/port/${BUILD_SYS})
include_directories(${PROJECT_SOURCE_DIR}/port/${OPERATING_SYS})
include_directories(${PROJECT_SOURCE_DIR}/port/${THREAD_SYS})
include_directories(${PROJECT_SOURCE_DIR}/include)

set(LIB_SRCS
	anondle.cpp
	barrier.cpp
	condition.cpp
	duration.cpp
	event.cpp
	semaphore.cpp
	shorex.cpp
	strand.cpp
	thrid.cpp
	list.cpp
	prioqueue.cpp
	self.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/create.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/mutex.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/once.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/retex.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/self.cpp
	${PROJECT_SOURCE_DIR}/port/${THREAD_SYS}/thread.cpp
	${PROJECT_SOURCE_DIR}/port/${CPU_ARCH}/${BUILD_SYS}/countic.cpp
)

file (GLOB INTERN_HDRS *.h)

if (BUILD_SHARED_LIBS)
	add_library(lisle SHARED ${LIB_SRCS})
	set_target_properties(lisle PROPERTIES VERSION "${LISLE_VERSION_MAJOR}.${LISLE_VERSION_MINOR}.${LISLE_VERSION_PATCH}")
	set_target_properties(lisle PROPERTIES SOVERSION "${LISLE_VERSION_MAJOR}")
	if (MSVC)
		add_definitions(-DLISLE_BUILD_SHARED)
	endif()
else ()
	add_library(lisle STATIC ${INTERN_HDRS} ${LIB_HDRS} ${LIB_SRCS})
endif ()

install(TARGETS lisle DESTINATION lib)
