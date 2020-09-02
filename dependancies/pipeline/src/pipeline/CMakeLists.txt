cmake_minimum_required(VERSION 3.4.1)

# disable example for now, focus on AnyOpt

testBuilder_add_source(AnyOpt_Tests AnyOpt_Tests.cpp)
testBuilder_add_library(AnyOpt_Tests gtest_main)
testBuilder_build(AnyOpt_Tests EXECUTABLES)
