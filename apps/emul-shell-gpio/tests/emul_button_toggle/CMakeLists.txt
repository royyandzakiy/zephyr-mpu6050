# tests/drivers/gpio_button_toggle/CMakeLists.txt

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gpio_test)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_harness.c
)