cmake_minimum_required(VERSION 3.22)
set(BOARD xiao_esp32c3)

find_package(Zephyr)
project(hello_world_blinky)

target_sources(app PRIVATE src/main.c)

include_directories(/home/milan/zephyr/zephyr/include)