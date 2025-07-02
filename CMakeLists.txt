cmake_minimum_required(VERSION 3.11)

project(TUC)

include_directories((include))

# https://stackoverflow.com/questions/38700198/how-to-partially-disabling-cmake-c-c-custom-compiler-checking
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)