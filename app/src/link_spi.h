/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_LINK_SPI_H_
#define APP_SRC_LINK_SPI_H_

/**
 * @brief Fast-path link glue: SPI peripheral driver + NSS/CS re-arm.
 *
 * Owns the 33-byte transceive, the CS-rising EXTI resync, the
 * armed/staging TX double buffer, and the RX hand-off queue
 * (spec sections 3, 5.2, 5.3). No logging, allocation, or blocking
 * calls are permitted in this path by construction.
 */

/** Initialize SPI1 slave, arm the identity frame, raise LINK_READY. */
int link_spi_init(void);

#endif /* APP_SRC_LINK_SPI_H_ */
