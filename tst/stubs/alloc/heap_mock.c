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
#include "heap_mock.h"

int mock_write_heap_init(void *buf_p,
                         size_t size,
                         size_t sizes[HEAP_FIXED_SIZES_MAX],
                         int res)
{
    harness_mock_write("heap_init(buf_p)",
                       buf_p,
                       size);

    harness_mock_write("heap_init(size)",
                       &size,
                       sizeof(size));

    harness_mock_write("heap_init(sizes[HEAP_FIXED_SIZES_MAX])",
                       &sizes[HEAP_FIXED_SIZES_MAX],
                       sizeof(sizes[HEAP_FIXED_SIZES_MAX]));

    harness_mock_write("heap_init(): return (res)",
                       &res,
                       sizeof(res));

    return (0);
}

int __attribute__ ((weak)) STUB(heap_init)(struct heap_t *self_p,
                                           void *buf_p,
                                           size_t size,
                                           size_t sizes[HEAP_FIXED_SIZES_MAX])
{
    int res;

    harness_mock_assert("heap_init(buf_p)",
                        buf_p,
                        size);

    harness_mock_assert("heap_init(size)",
                        &size,
                        sizeof(size));

    harness_mock_assert("heap_init(sizes[HEAP_FIXED_SIZES_MAX])",
                        &sizes[HEAP_FIXED_SIZES_MAX],
                        sizeof(sizes[HEAP_FIXED_SIZES_MAX]));

    harness_mock_read("heap_init(): return (res)",
                      &res,
                      sizeof(res));

    return (res);
}

int mock_write_heap_alloc(size_t size,
                          void *res)
{
    harness_mock_write("heap_alloc(size)",
                       &size,
                       sizeof(size));

    harness_mock_write("heap_alloc(): return (res)",
                       &res,
                       sizeof(res));

    return (0);
}

void *__attribute__ ((weak)) STUB(heap_alloc)(struct heap_t *self_p,
                                              size_t size)
{
    void *res;

    harness_mock_assert("heap_alloc(size)",
                        &size,
                        sizeof(size));

    harness_mock_read("heap_alloc(): return (res)",
                      &res,
                      sizeof(res));

    return (res);
}

int mock_write_heap_free(void *buf_p,
                         int res)
{
    harness_mock_write("heap_free(buf_p)",
                       buf_p,
                       sizeof(buf_p));

    harness_mock_write("heap_free(): return (res)",
                       &res,
                       sizeof(res));

    return (0);
}

int __attribute__ ((weak)) STUB(heap_free)(struct heap_t *self_p,
                                           void *buf_p)
{
    int res;

    harness_mock_assert("heap_free(buf_p)",
                        buf_p,
                        sizeof(*buf_p));

    harness_mock_read("heap_free(): return (res)",
                      &res,
                      sizeof(res));

    return (res);
}

int mock_write_heap_share(const void *buf_p,
                          int count,
                          int res)
{
    harness_mock_write("heap_share(buf_p)",
                       buf_p,
                       sizeof(buf_p));

    harness_mock_write("heap_share(count)",
                       &count,
                       sizeof(count));

    harness_mock_write("heap_share(): return (res)",
                       &res,
                       sizeof(res));

    return (0);
}

int __attribute__ ((weak)) STUB(heap_share)(struct heap_t *self_p,
                                            const void *buf_p,
                                            int count)
{
    int res;

    harness_mock_assert("heap_share(buf_p)",
                        buf_p,
                        sizeof(*buf_p));

    harness_mock_assert("heap_share(count)",
                        &count,
                        sizeof(count));

    harness_mock_read("heap_share(): return (res)",
                      &res,
                      sizeof(res));

    return (res);
}
