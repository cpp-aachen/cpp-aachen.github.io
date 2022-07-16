cmake_minimum_required(VERSION 3.10.1)

project(a07 LANGUAGES CXX C ASM)

set(BENCHMARK_ENABLE_TESTING Off)
add_subdirectory(extern/benchmark)

add_subdirectory(extern/vectorclass)

include(CheckLanguage)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(CMAKE_ASM_NASM_COMPILER nasm.exe)
endif()

check_language(ASM_NASM)
if(CMAKE_ASM_NASM_COMPILER)
  enable_language(ASM_NASM)
  set(NASM_FOUND 1)
else()
  message(STATUS "No NAsm support")
endif()

if(MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit pointer
    string(APPEND CMAKE_CXX_FLAGS " /arch:AVX2")
    string(APPEND CMAKE_C_FLAGS " /arch:AVX2")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    string(APPEND CMAKE_CXX_FLAGS " /arch:IA32")
    string(APPEND CMAKE_C_FLAGS " /arch:IA32")
  else()
    message(FATAL_ERROR "Unknown System, only 64 and 64 bit supported")
  endif()
else()
  string(APPEND CMAKE_CXX_FLAGS " -mavx2 -mfma")
  string(APPEND CMAKE_C_FLAGS " -mavx2 -mfma")
endif()

find_package(Boost 1.67)

if (Boost_FOUND)
  message("Boost found")
endif ()

add_executable(simd01 main.cpp)
target_compile_features(simd01 PRIVATE cxx_std_20)

add_executable(simd_alignment simd_alignment.cpp)
target_compile_features(simd_alignment PRIVATE cxx_std_20)
target_link_libraries(simd_alignment PRIVATE Boost::headers)

add_executable(simd_aligned_allocator simd_aligned_allocator.cpp)
target_compile_features(simd_aligned_allocator PRIVATE cxx_std_20)
target_link_libraries(simd_aligned_allocator PRIVATE Boost::headers)

add_executable(simd_asm_01 addnasm01.asm addnasm01_main.cpp)
target_compile_features(simd01 PRIVATE cxx_std_20)
add_executable(simd_if simd_if.cpp)
target_compile_features(simd_if PRIVATE cxx_std_20)
target_link_libraries(simd_if PRIVATE vectorclass)


add_executable(bench_normalize bench_normalize.cpp)
target_link_libraries(bench_normalize benchmark::benchmark_main)

add_executable(bench_vec_normalize bench_vec_normalize.cpp)
target_link_libraries(bench_vec_normalize benchmark::benchmark_main)

add_executable(bench_vec_normalize2 bench_vec_normalize2.cpp)
target_link_libraries(bench_vec_normalize2 benchmark::benchmark_main)

add_executable(simd_vectorclass simd_vectorclass.cpp)
target_compile_features(simd_vectorclass PRIVATE cxx_std_20)
target_link_libraries(simd_vectorclass PRIVATE vectorclass)

add_subdirectory(godbolt)
