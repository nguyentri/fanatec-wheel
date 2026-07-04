# Copyright (c) 2026 Fanatec Wheel Project Contributors
# SPDX-License-Identifier: Apache-2.0

# Custom driver class example (reusable template)
add_subdirectory_ifdef(CONFIG_BLINK blink)

# Out-of-tree driver for an existing driver class (reusable template)
add_subdirectory_ifdef(CONFIG_SENSOR example_sensor)

# Phase 3+: TM1637 display driver will be added here (CONFIG_RIM_TM1637)
