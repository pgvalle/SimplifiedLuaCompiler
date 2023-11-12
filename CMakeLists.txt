cmake_minimum_required(VERSION 3.22)

project(SimplifiedLuaCompiler)

file(GLOB_RECURSE CPP_FILES source/*.cpp)
add_executable(slc ${CPP_FILES})
