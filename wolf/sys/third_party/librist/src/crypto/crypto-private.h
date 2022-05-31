/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_CRYPTO_PRIVATE_H
#define RIST_CRYPTO_PRIVATE_H

#include "common/attributes.h"
#include <stdint.h>

RIST_PRIV uint64_t rist_siphash(uint64_t birthtime, uint32_t seq, const char *phrase);
RIST_PRIV uint32_t prand_u32(void);

#endif
