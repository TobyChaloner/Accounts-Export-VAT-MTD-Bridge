#used to cross-compile for windows

#Build from src top dir
#cd ../win-bld && cmake -DCMAKE_TOOLCHAIN_FILE=../src/TC-mingw.cmake ../src && cmake --build .


#from cmake/help/book/mastering-cmake/chapter/Cross Compiling With CMake.html

#INSTALL development support
#apt install gcc-mingw-w64
#apt install g++-mingw-w64

# cmake commands in build directory (build-win)
# cmake -DCMAKE_TOOLCHAIN_FILE=../src/TC-mingw.cmake ../src
# assumes directory structure
# mtd-bridge
#   src
#   build-win <current directory>

#building
#cd build-win
#cmake -DCMAKE_TOOLCHAIN_FILE=../src/TC-mingw.cmake ../src
#cmake --build .

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-c++)

#remove need to have various DLL
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc
    /home/alex/mingw-install)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
