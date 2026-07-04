#-------------------------------------------------------------------------------
# Base-side simulator - Phase 1 (spec section 8)
#
# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sim_app LANGUAGES C)

target_sources(app PRIVATE
	src/main.c
	../src/seg7.c
)
target_include_directories(app PRIVATE ../src)
