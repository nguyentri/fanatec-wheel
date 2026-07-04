# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_logic_test)

target_sources(app PRIVATE
	src/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/seg7.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/dpad.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/encoder.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/funky.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/clutch.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/led_svc.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/lra_svc.c
)
target_include_directories(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src
)

# Service sources reuse the app log module; tests pin it off.
target_compile_definitions(app PRIVATE CONFIG_APP_LOG_LEVEL=0)
