set(BINARY ${CMAKE_PROJECT_NAME}_test)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest_main)

include(GoogleTest)
gtest_add_tests(TARGET ${CMAKE_PROJECT_NAME}_test)

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib "${CMAKE_THREAD_LIBS_INIT}")

if(${BUILD_FOR_CUDA})
  target_compile_options(${CMAKE_PROJECT_NAME}_lib PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda -fsycl-unnamed-lambda)
  target_link_options(${BINARY} PUBLIC -fsycl -fsycl-targets=nvptx64-nvidia-cuda -fsycl-unnamed-lambda)
endif ()
