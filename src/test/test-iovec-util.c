/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "iovec-util.h"
#include "memory-util.h"
#include "tests.h"

TEST(iovec_memcmp) {
        struct iovec iov1 = CONST_IOVEC_MAKE_STRING("abcdef"), iov2 = IOVEC_MAKE_STRING("bcdefg"), empty = {};

        struct iovec iov1_truncated = iov1;
        iov1_truncated.iov_len /= 2;

        ASSERT_EQ(iovec_memcmp(NULL, NULL), 0);
        ASSERT_EQ(iovec_memcmp(&iov1, &iov1), 0);
        ASSERT_EQ(iovec_memcmp(&iov2, &iov2), 0);
        ASSERT_EQ(iovec_memcmp(&empty, &empty), 0);
        ASSERT_EQ(iovec_memcmp(&iov1_truncated, &iov1_truncated), 0);
        ASSERT_EQ(iovec_memcmp(&empty, NULL), 0);
        ASSERT_EQ(iovec_memcmp(NULL, &empty), 0);
        ASSERT_LT(iovec_memcmp(&iov1, &iov2), 0);
        ASSERT_GT(iovec_memcmp(&iov2, &iov1), 0);
        ASSERT_GT(iovec_memcmp(&iov1, &empty), 0);
        ASSERT_LT(iovec_memcmp(&empty, &iov1), 0);
        ASSERT_GT(iovec_memcmp(&iov2, &empty), 0);
        ASSERT_LT(iovec_memcmp(&empty, &iov2), 0);
        ASSERT_GT(iovec_memcmp(&iov1_truncated, &empty), 0);
        ASSERT_LT(iovec_memcmp(&empty, &iov1_truncated), 0);
        ASSERT_GT(iovec_memcmp(&iov1, &iov1_truncated), 0);
        ASSERT_LT(iovec_memcmp(&iov1_truncated, &iov1), 0);
        ASSERT_GT(iovec_memcmp(&iov2, &iov1_truncated), 0);
        ASSERT_LT(iovec_memcmp(&iov1_truncated, &iov2), 0);

        _cleanup_(iovec_done) struct iovec copy = {};

        ASSERT_NOT_NULL(iovec_memdup(&iov1, &copy));
        ASSERT_EQ(iovec_memcmp(&iov1, &copy), 0);
}

TEST(iovec_set_and_valid) {
        struct iovec empty = {},
                filled = CONST_IOVEC_MAKE_STRING("waldo"),
                half = { .iov_base = (char*) "piff", .iov_len = 0 },
                invalid = { .iov_base = NULL, .iov_len = 47 };

        ASSERT_FALSE(iovec_is_set(NULL));
        ASSERT_FALSE(iovec_is_set(&empty));
        ASSERT_TRUE(iovec_is_set(&filled));
        ASSERT_FALSE(iovec_is_set(&half));
        ASSERT_FALSE(iovec_is_set(&invalid));

        ASSERT_TRUE(iovec_is_valid(NULL));
        ASSERT_TRUE(iovec_is_valid(&empty));
        ASSERT_TRUE(iovec_is_valid(&filled));
        ASSERT_TRUE(iovec_is_valid(&half));
        ASSERT_FALSE(iovec_is_valid(&invalid));
}

TEST(iovec_append) {
        _cleanup_(iovec_done) struct iovec iov = {};

        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("")), &iov);
        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("waldo")), &iov);
        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("quux")), &iov);
        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("")), &iov);
        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("p")), &iov);
        ASSERT_PTR_EQ(iovec_append(&iov, &IOVEC_MAKE_STRING("")), &iov);

        ASSERT_EQ(iovec_memcmp(&iov, &IOVEC_MAKE_STRING("waldoquuxp")), 0);
}

TEST(iovec_make_byte) {
        struct iovec x = IOVEC_MAKE_BYTE('x');

        ASSERT_EQ(x.iov_len, 1U);
        ASSERT_EQ(memcmp_nn(x.iov_base, x.iov_len, "x", 1), 0);
}

DEFINE_TEST_MAIN(LOG_INFO);
