cmake_minimum_required(VERSION 2.8)
enable_testing()

if (GCC)
	add_definitions(-std=c++0x)
endif ()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/port/${BUILD_SYS})
include_directories(${PROJECT_SOURCE_DIR}/port/${THREAD_SYS})
include_directories(${PROJECT_SOURCE_DIR}/port/${OPERATING_SYS})
include_directories(${PROJECT_SOURCE_DIR}/port/${CPU_ARCH}/${BUILD_SYS})

set(TEST_SRCS
	anondle.cpp
	barrier.cpp
	condition.cpp
	create.cpp
	duration.cpp
	event.cpp
	exception.cpp
	handle.cpp
	local.cpp
	mutex.cpp
	once.cpp
	prioqueue.cpp
	retex.cpp
	self.cpp
	semaphore.cpp
	shorex.cpp
	strand.cpp
	strid.cpp
	thread.cpp
	thrid.cpp
)
set(TEST_HDRS
	test.h
)

add_executable(runner runner.cpp ${TEST_HDRS} ${TEST_SRCS})
set(TEST_LIBS gtest lisle)
if (HAVE_CLOCK_GETTIME_IN_RT)
	set(TEST_LIBS ${TEST_LIBS} rt)
endif ()
target_link_libraries(runner ${TEST_LIBS})

add_test(lisle runner)
