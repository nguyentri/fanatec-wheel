# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

zephyr_library_named(rimlink)
zephyr_library_sources(
	rimlink_crc8.c
	rimlink_frame.c
	rimlink_link.c
)
zephyr_library_sources_ifdef(CONFIG_RIMLINK_CAPTURE rimlink_capture.c)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)
