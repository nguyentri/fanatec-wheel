#-------------------------------------------------------------------------------
# Rim firmware (DUT) - Phase 1 Zephyr Rim-Link implementation
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
)
target_sources_ifdef(CONFIG_RIM_TM1637 app PRIVATE src/tm1637.c)
target_include_directories(app PRIVATE src)
