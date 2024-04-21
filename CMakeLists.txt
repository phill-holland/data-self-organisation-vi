cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_COMPILER clang++)

project(data-self-organisation LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(LLVM_ENABLE_THREADS TRUE)
set(LLVM_PARALLEL_COMPILE_JOBS, 8)
set(LLVM_PARALLEL_LINK_JOBS, 8)

enable_testing()

find_package(IntelDPCPP REQUIRED)
find_package(Threads REQUIRED)

include_directories("/usr/include")
include_directories("src/include")

add_subdirectory(src)
add_subdirectory(test)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
"${PROJECT_SOURCE_DIR}/src/source/*.cpp" 
)

