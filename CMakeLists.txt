# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0
#
# Zephyr module CMake entry point (see zephyr/module.yml).

if(NOT DEFINED ZEPHYR_CURRENT_MODULE_DIR)
  set(ZEPHYR_CURRENT_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  message(STATUS "ZEPHYR_CURRENT_MODULE_DIR not set; defaulting to ${ZEPHYR_CURRENT_MODULE_DIR}")
endif()

# Workspace resources (custom boards / DTS bindings)
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Syscall headers for custom driver classes
zephyr_syscall_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/drivers/blink)

zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(drivers)
add_subdirectory(lib)
