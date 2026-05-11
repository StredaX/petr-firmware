cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(pet_tracker_firmware VERSION 0.1.0 LANGUAGES C)

target_include_directories(app PRIVATE src)

target_sources(app PRIVATE
    src/main.c
    src/app/app_config.c
    src/app/app_state.c
    src/drivers/buzzer.c
    src/drivers/vibration_motor.c
    src/platform/diagnostics.c
)
