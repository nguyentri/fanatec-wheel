/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_LINK_SPI_H_
#define APP_SRC_LINK_SPI_H_

/**
 * @brief Fast-path link glue: SPI peripheral transfers + CS-sense
 * EXTI re-arm/statistics + LINK_READY/error indicators
 * (spec sections 3, 5.2, 5.3, 10).
 */

/** Configure CS EXTI + indicators, verify SPI, release the link thread. */
int link_spi_init(void);

#endif /* APP_SRC_LINK_SPI_H_ */
