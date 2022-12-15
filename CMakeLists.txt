#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build .
#cd /home/toby/dev/solar/mtd-bridge/build ; cmake  -DCMAKE_BUILD_TYPE=Debug  ../src

#build and run
#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build . && cd ../src && ../build/src/Solar100PcVatFreeBridge

#to run on a file (assume in dir about build) for VAT end month 8
#./build/src/Solar100PcVatFreeBridge -i Month8\ export.csv -o out.csv -e 8



#to run the tests
#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build . && tests/Solar100PcVatFreeBridge_tst 

cmake_minimum_required(VERSION 3.14...3.23)
project(
  Solar100PcVatFreeBridge
  VERSION 1.0
  LANGUAGES CXX)


# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)
include_directories(libs)

add_subdirectory(src)
add_subdirectory(tests)

#output the binary to a bin directory
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



#
# Add version number into executable
#
#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
#project(cmake-git-versioning-example C)
#include_directories(${CMAKE_BINARY_DIR})
#add_executable(foobar main.c)
find_package(Git)
add_custom_target(version
  ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/include/SolarMtdBridge_version.h.in
                   -D DST=${CMAKE_BINARY_DIR}/SolarMtdBridge_version.h
                   -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                   -P ${CMAKE_SOURCE_DIR}/utility/GenerateVersionHeader.cmake
  )
add_dependencies(Solar100PcVatFreeBridge version)

