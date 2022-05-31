/* librist. Copyright © 2019-2020 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_LOG_PRIVATE_H
#define RIST_LOG_PRIVATE_H

#include "common/attributes.h"
#include "rist-private.h"

RIST_PRIV void rist_log_priv(struct rist_common_ctx *cctx, enum rist_log_level level, const char *format, ...);
RIST_PRIV void rist_log_priv2(struct rist_logging_settings *logging_settings, enum rist_log_level level, const char *format, ...);
RIST_PRIV void rist_log_priv3(enum rist_log_level level, const char *format, ...);
#endif
