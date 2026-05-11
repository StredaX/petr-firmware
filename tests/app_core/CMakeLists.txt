cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_core_tests LANGUAGES C)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_compile_definitions(app PRIVATE
    CONFIG_PET_TRACKER_FIRMWARE_VERSION="test-0.1.0"
)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/app_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/app/app_state.c
    src/main.c
)
