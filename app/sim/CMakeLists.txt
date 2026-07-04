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

# Base-twin profile (spec 2-S6): YAML -> generated header.
set(TWIN_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/profiles/base_twin.yaml
    CACHE FILEPATH "Base-twin profile YAML")
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/sim_profile.h
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/profiles/gen_profile.py
		${TWIN_PROFILE} ${CMAKE_CURRENT_BINARY_DIR}/generated/sim_profile.h
	DEPENDS ${TWIN_PROFILE} ${CMAKE_CURRENT_SOURCE_DIR}/profiles/gen_profile.py)
add_custom_target(sim_profile_h DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/sim_profile.h)
add_dependencies(app sim_profile_h)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

