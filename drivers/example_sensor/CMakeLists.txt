# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_EXAMPLE_SENSOR)
	zephyr_library()
	zephyr_library_sources(example_sensor.c)
endif()
