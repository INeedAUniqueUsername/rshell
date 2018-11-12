CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(Boost 1.53.00 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_SUBDIRECTORY(googletest)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(
	rshell
	src/main.cpp
	src/program.cpp
	src/operation.cpp
)

ADD_EXECUTABLE(
	test
	src/test.cpp
	src/program.cpp
	src/operation.cpp
)

TARGET_LINK_LIBRARIES(test gtest)
TARGET_COMPILE_DEFINITIONS(test PRIVATE gtest_disable_pthreads=ON)
