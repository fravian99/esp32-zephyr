cmake_minimum_required(VERSION 3.20.0)

#set(BOARD esp32c6_devkitc/esp32c6/hpcore)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp32-zephyr)

target_include_directories(app PRIVATE include)
target_sources(app PRIVATE src/main.c src/internal_temp.c src/rgb_led.c src/ble_utils.c)
