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

#include "simba.h"

#define SERVER_ADDRESS 0x12345678
#define CLIENT_ADDRESS 0x87654321

#define DS18B20_ID { 0x28, 0x09, 0x1e, 0xa3, 0x05, 0x00, 0x00, 0x42 }

static struct fs_command_t cmd_set_min_max;

static volatile long temp_min = 230000;
static volatile long temp_max = 290000;

static int cmd_set_min_max_cb(int argc,
                              const char *argv[],
                              void *out_p,
                              void *in_p,
                              void *arg_p,
                              void *call_arg_p)
{
    long min, max;

    UNUSED(in_p);

    if (argc != 3) {
        std_fprintf(out_p, OSTR("2 argument required.\r\n"));
        return (1);
    }

    if ((std_strtol(argv[1], &min) == NULL) ||
        (std_strtol(argv[2], &max) == NULL)) {
        std_fprintf(out_p,
                    OSTR("bad min or max value '%s' '%s'\r\n"),
                    argv[1],
                    argv[2]);
        return (1);
    }

    temp_min = 10000 * min;
    temp_max = 10000 * max;
    std_fprintf(out_p,
                OSTR("min set to %ld and max set to %ld\r\n"),
                temp_min / 10000,
                temp_max / 10000);

    return (0);
}

static struct nrf24l01_driver_t nrf24l01;

int main()
{
    struct owi_driver_t owi;
    struct ds18b20_driver_t ds;
    struct owi_device_t devices[4];
    int read_temp;
    long temp, resolution;
    uint8_t state;
    uint8_t id[8] = DS18B20_ID;

    sys_start();

    fs_command_init(&cmd_set_min_max,
                    CSTR("/temp/set_min_max"),
                    cmd_set_min_max_cb,
                    NULL);
    fs_command_register(&cmd_set_min_max);

    nrf24l01_init(&nrf24l01,
                  &spi_device[0],
                  &pin_d10_dev,
                  &pin_d6_dev,
                  &exti_device[1],
                  CLIENT_ADDRESS);
    nrf24l01_start(&nrf24l01);

    /* Initialize temperature sensor. */
    owi_init(&owi, &pin_d5_dev, devices, membersof(devices));
    ds18b20_init(&ds, &owi);

    /* Read temperature periodically. */
    while (1) {
        /* Read temperature. */
        ds18b20_convert(&ds);
        ds18b20_get_temperature(&ds, id, &read_temp);

        temp = read_temp;
        temp = (10000 * (temp >> 4) + 625 * (temp & 0xf));

        /* Update led. */
        if (temp <= temp_min) {
            state = 0x1;
        } else if (temp >= temp_max) {
            state = 0x7;
        } else {
            temp -= temp_min;
            resolution = ((temp_max - temp_min) / 8);
            temp /= resolution;
            state = temp;

            if (state == 0x0) {
                state = 0x1;
            }
        }

        std_printf(FSTR("state = 0x%x\r\n"), (int)state);

        /* Send state to server. */
        nrf24l01_write(&nrf24l01,
                       SERVER_ADDRESS,
                       1,
                       &state,
                       sizeof(state));

        std_printf(FSTR("written state = 0x%x\r\n"), (int)state);
    }

    return (0);
}
