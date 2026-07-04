# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rimlink_test)

target_sources(app PRIVATE src/main.c)

# Phase 5 health counters (spec 5-S3) build against the real adapter.
target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/health.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/rim_wdt.c
)
target_include_directories(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src)
target_compile_definitions(app PRIVATE CONFIG_APP_LOG_LEVEL=0)
