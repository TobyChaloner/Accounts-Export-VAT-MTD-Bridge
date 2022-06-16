#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build .
#cd /home/toby/dev/solar/mtd-bridge/build ; cmake  -DCMAKE_BUILD_TYPE=Debug  ../src

#build and run
#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build . && cd ../src && ../build/prog


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


