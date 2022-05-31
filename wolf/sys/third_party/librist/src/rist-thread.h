/*
 * Copyright © 2021, VideoLAN and librist authors
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_THREAD_H
#define RIST_THREAD_H
#include "rist-private.h"

RIST_PRIV int rist_thread_create(struct rist_common_ctx *cctx,
                       pthread_t *thread, pthread_attr_t *attr, pthread_start_func_t thread_func, void *thread_arg);

#endif
