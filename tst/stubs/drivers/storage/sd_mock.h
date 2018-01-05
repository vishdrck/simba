/**
 * @section License
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018, Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Simba project.
 */

#ifndef __SD_MOCK_H__
#define __SD_MOCK_H__

#include "simba.h"

int mock_write_sd_init(struct spi_driver_t *spi_p,
                       int res);

int mock_write_sd_start(int res);

int mock_write_sd_stop(int res);

int mock_write_sd_read_cid(struct sd_cid_t* cid_p,
                           ssize_t res);

int mock_write_sd_read_csd(union sd_csd_t* csd_p,
                           ssize_t res);

int mock_write_sd_erase_blocks(uint32_t start_block,
                               uint32_t end_block,
                               int res);

int mock_write_sd_read_block(void *dst_p,
                             uint32_t src_block,
                             ssize_t res);

int mock_write_sd_write_block(uint32_t dst_block,
                              const void *src_p,
                              ssize_t res);

#endif
