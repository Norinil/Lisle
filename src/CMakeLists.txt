cmake_minimum_required(VERSION 2.8)

if (GCC)
	add_definitions(-std=c++0x)
endif ()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/port/${BUILD_SYS})
include_directories(${CMAKE_SOURCE_DIR}/port/${OPERATING_SYS})
include_directories(${CMAKE_SOURCE_DIR}/port/${THREAD_SYS})
include_directories(${CMAKE_SOURCE_DIR}/port)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(LIB_SRCS
	lisle.cpp
	Anondle.cpp
	Barrier.cpp
	Condition.cpp
	Duration.cpp
	Event.cpp
	Semaphore.cpp
	Shorex.cpp
	Strand.cpp
	Thrid.cpp
	list.cpp
	prioqueue.cpp
	self.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/Mutex.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/Retex.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/Once.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/Thread.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/create.cpp
	${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/self.cpp
	${CMAKE_SOURCE_DIR}/port/${CPU_ARCH}/${BUILD_SYS}/Countic.cpp
)

if (THREAD_SYS MATCHES windows)
	set(LIB_SRCS ${LIB_SRCS} ${CMAKE_SOURCE_DIR}/port/${THREAD_SYS}/lockcmpxchg.cpp)
endif ()

if (BUILD_SHARED_LIBS)
	add_library(lisle SHARED ${LIB_SRCS})
	set_target_properties(lisle PROPERTIES VERSION "${LISLE_VERSION_MAJOR}.${LISLE_VERSION_MINOR}.${LISLE_VERSION_PATCH}")
	set_target_properties(lisle PROPERTIES SOVERSION "${LISLE_VERSION_MAJOR}")
else ()
	add_library(lisle STATIC ${LIB_SRCS})
endif ()

install(TARGETS lisle DESTINATION lib)
