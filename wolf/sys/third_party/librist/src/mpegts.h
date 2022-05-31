/* librist. Copyright © 2020 SipRadius LLC. All right reserved.
 * Author: Gijs Peskens <gijs@in2ip.nl>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_MPEGTS
#define RIST_MPEGTS
#include "udp-private.h"
#include "common/attributes.h"
#include <stdint.h>
#include <stddef.h>

/*
Needed for nullpacket deletion, values are for null packets:
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Sync Byte    |E|P|T|            PID          |TSC| AF|  CC   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Sync Byte                               0x47 
E: Transport error indicator (TEI)      0
P: Payload unit start indicator (PUSI)  0
T: Transport Priority                   0
PID:                                    0x1ff (null packet pid)
TSC: Transport scambling control        0
AF: Adaptation field control            1 (payload only)
CC: Continuity counter                  0
*/

RIST_PACKED_STRUCT(mpegts_header, {
	uint8_t syncbyte;
	uint16_t flags1;
	uint8_t flags2;
})

RIST_PRIV int suppress_null_packets(const uint8_t payload_in[], uint8_t payload_out[], size_t *payload_len, struct rist_rtp_hdr_ext *header_ext);
RIST_PRIV int expand_null_packets(uint8_t payload[], size_t *payload_len, uint8_t npd_bits);

#endif
