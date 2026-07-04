# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

# Custom driver class example (reusable template)
add_subdirectory_ifdef(CONFIG_BLINK blink)

# Out-of-tree driver for an existing driver class (reusable template)
add_subdirectory_ifdef(CONFIG_SENSOR example_sensor)

# This directory holds reusable out-of-tree driver-class templates.
# The optional TM1637 display mirror (CONFIG_RIM_TM1637) is a small
# app-level bit-banged driver in app/src/tm1637.c, not a driver class.
