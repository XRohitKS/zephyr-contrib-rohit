cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(xled_demo)

target_sources(app PRIVATE src/main.c)

# Include the raw C library directly
target_sources(app PRIVATE ../../subsys/xled/xled.c)
zephyr_include_directories(../../include/zephyr)
