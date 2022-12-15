#cd /home/toby/dev/solar/mtd-bridge/build ;  cmake --build . && cd ../src && ../build/src/prog -i data/vatentriesExp.csv



set(BINARY ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)
set(SOURCES ${SOURCES})
add_executable(${BINARY} ${SOURCES})

set(SRC_LIB_ROOT ${CMAKE_PROJECT_NAME})
add_library(${SRC_LIB_ROOT}_lib STATIC ${SOURCES})

#Pick up the version number read from Git
include_directories(${CMAKE_BINARY_DIR})


