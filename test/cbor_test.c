/*
 * Copyright (c) 2015 SPUDlib authors.  See LICENSE file.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <check.h>
#include "cn-cbor/cn-cbor.h"
#include "../src/cn-cbor/cn-encoder.h"

Suite * cbor_suite (void);

typedef struct _buffer {
    size_t sz;
    char *ptr;
} buffer;

static bool parse_hex(char *inp, buffer *b)
{
    int len = strlen(inp);
    int i;
    if (len%2 != 0) {
        b->sz = -1;
        b->ptr = NULL;
        return false;
    }
    b->sz  = len / 2;
    b->ptr = malloc(b->sz);
    for (i=0; i<b->sz; i++) {
        sscanf(inp+(2*i), "%02hhx", &b->ptr[i]);
    }
    return true;
}

START_TEST (cbor_parse_test)
{
    cn_cbor_errback err;
    char *tests[] = {
        "00",
        "01",
        "17",
        "1818",
        "6161",
        "8100",
    };
    const cn_cbor *cb;
    buffer b;
    int i;
    unsigned char encoded[1024];
    ssize_t enc_sz;

    for (i=0; i<sizeof(tests)/sizeof(char*); i++) {
        ck_assert(parse_hex(tests[i], &b));
        cb = cn_cbor_decode(b.ptr, b.sz, &err);
        ck_assert_msg(cb != NULL, tests[i]);

        enc_sz = cbor_encoder_write(encoded, 0, sizeof(encoded), cb);
        ck_assert_int_eq(enc_sz, b.sz);
        ck_assert_int_eq(memcmp(b.ptr, encoded, enc_sz), 0);
        free(b.ptr);
    }
}
END_TEST

Suite * cbor_suite (void)
{
    Suite *s = suite_create ("cbor");
    {
        TCase *tc_cbor_parse = tcase_create ("parse");
        tcase_add_test (tc_cbor_parse, cbor_parse_test);

        suite_add_tcase (s, tc_cbor_parse);
    }
    return s;
}
