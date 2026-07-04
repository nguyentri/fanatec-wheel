#-------------------------------------------------------------------------------
# Rim firmware (DUT) - Zephyr Rim-Link implementation
#
# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(rim_app LANGUAGES C)

target_sources(app PRIVATE
	src/main.c
	src/link_spi.c
	src/input_svc.c
	src/output_svc.c
	src/diag_svc.c
	src/seg7.c
	src/dpad.c
	src/encoder.c
	src/funky.c
	src/clutch.c
	src/rim_settings.c
	src/led_svc.c
	src/lra_svc.c
	src/power_mgr.c
	src/rim_wdt.c
	src/health.c
)
target_sources_ifdef(CONFIG_RIM_TM1637 app PRIVATE src/tm1637.c)
target_include_directories(app PRIVATE src)

# Session attribution (spec 2-S3): git hash baked into the build.
execute_process(
	COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} rev-parse --short HEAD
	OUTPUT_VARIABLE RIM_GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET)
if(NOT RIM_GIT_HASH)
	set(RIM_GIT_HASH "nogit")
endif()
execute_process(
	COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} status --porcelain
	OUTPUT_VARIABLE RIM_GIT_DIRTY_RAW ERROR_QUIET)
if(RIM_GIT_DIRTY_RAW)
	set(RIM_GIT_HASH "${RIM_GIT_HASH}-dirty")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/session_info.h.in
	${CMAKE_CURRENT_BINARY_DIR}/generated/session_info.h)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

