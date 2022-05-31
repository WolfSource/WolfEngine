/* librist. Copyright © 2019-2020 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_UDP_PRIVATE_H
#define RIST_UDP_PRIVATE_H

#include "common/attributes.h"
#include "rist-private.h"

#define SET_BIT(value, pos) (value |= (1U<< pos))
#define UNSET_BIT(value, pos) (value &= (1U << pos))

#define RIST_GRE_PROTOCOL_TYPE_KEEPALIVE 0x88B5
#define RIST_GRE_PROTOCOL_TYPE_REDUCED 0x88B6
#define RIST_GRE_PROTOCOL_TYPE_FULL 0x0800
#define RIST_GRE_PROTOCOL_TYPE_EAPOL 0x888E
#define RIST_GRE_PROTOCOL_REDUCED_SIZE 4

#define RIST_GRE_FLAGS_KEY_SEQ 0x000C
#define RIST_GRE_FLAGS_SEQ     0x0008

#define RIST_PAYLOAD_TYPE_UNKNOWN           0x0
#define RIST_PAYLOAD_TYPE_PING              0x1
#define RIST_PAYLOAD_TYPE_PING_RESP         0x2
#define RIST_PAYLOAD_TYPE_RTCP              0x3
#define RIST_PAYLOAD_TYPE_RTCP_NACK         0x4
#define RIST_PAYLOAD_TYPE_DATA_RAW          0x5
#define RIST_PAYLOAD_TYPE_DATA_OOB          0x6 // Out-of-band data
#define RIST_PAYLOAD_TYPE_DATA_RAW_RTP_EXT  0x7
#define RIST_PAYLOAD_TYPE_EAPOL				0x8

// RTCP constants
#define RTCP_FB_HEADER_SIZE 12

#define PTYPE_SR 200
#define PTYPE_RR 201
#define PTYPE_SDES 202
#define PTYPE_XR 207
#define PTYPE_NACK_CUSTOM  204
#define PTYPE_NACK_BITMASK 205

#define NACK_FMT_BITMASK 1
#define NACK_FMT_RANGE 0
#define NACK_FMT_SEQEXT 1

#define ECHO_REQUEST 2
#define ECHO_RESPONSE 3

#define RTCP_SDES_SIZE 10
#define RTP_MPEGTS_FLAGS 0x80
#define RTCP_SR_FLAGS 0x80
#define RTCP_RR_FULL_FLAGS 0x81
#define RTCP_SDES_FLAGS 0x81
#define RTCP_NACK_RANGE_FLAGS 0x80
#define RTCP_NACK_BITMASK_FLAGS 0x81
#define RTCP_NACK_SEQEXT_FLAGS 0x81
#define RTCP_ECHOEXT_REQ_FLAGS 0x82
#define RTCP_ECHOEXT_RESP_FLAGS 0x83

// RTP Payload types and clocks
// March 1995 (page 9): https://tools.ietf.org/html/draft-ietf-avt-profile-04
// Nov 2019 (page 2): https://www.iana.org/assignments/rtp-parameters/rtp-parameters.xhtml
#define RTP_PTYPE_MPEGTS (0x21)
#define RTP_PTYPE_MPEGTS_CLOCKHZ (90000)
#define RTP_PTYPE_RIST (21)
#define RTP_PTYPE_RIST_CLOCKHZ (UINT16_MAX + 1)

// Maximum offset before the payload that the code can use to put in headers
#define RIST_MAX_PAYLOAD_OFFSET (sizeof(struct rist_gre_key_seq) + sizeof(struct rist_protocol_hdr))

/* Time conversion */
#define SEVENTY_YEARS_OFFSET (2208988800ULL)

/*

+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|0| |0|0|Reserved0|H|RVer | Ver |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Figure 1: GRE header with no options



+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|0| |0|1|Reserved0|H|RVer | Ver |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Sequence Number                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Figure 2: GRE header with sequence number



+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|0| |1|1|Reserved0|H|RVer | Ver |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Key/Nonce                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Figure 5: GRE header with Key/Nonce

The sequence number will become the higher 4byte of AES IV.
So, that on increment - the lower bits (which are zero) get incremented

*/

/*

Reduce overhead GRE payload header (only one supported for now)

+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        source port            |      destination port         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

/*

RTP header format (RFC 3550)
The RTP header is always present on data packets

0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|X|  CC   |M|     PT      |       sequence number         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           timestamp                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           synchronization source (SSRC) identifier            |
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|            contributing source (CSRC) identifiers             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
/*
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|0| Subtype |   PT=APP=204  |            Length             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  SSRC of media source                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         name (ASCII)                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                Timestamp, most significant word               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               Timestamp, least significant word               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                Processing Delay (microseconds)                |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         Padding bytes                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                               ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         Padding bytes                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
/*
RTP header extension format (RFC 3550)

0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      defined by profile       |           length              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        header extension                       |
|                             ....                              |

RIST implementation of header extension format used for null packet
deletion and sequence number extension (VSF TR-06-02, 8.3)

0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|    0x52 (R)   |    0x49 (I)   |          Length=1             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|N|E|Size |0 0 0|T|  NPD bits   |   Sequence Number Extension   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

/*
Data Channel:

[  GRE header  ]
[  Reduced overhead header .]
[  RTP header .]
[  User payload  ]

RTCP Control Channel:

[  GRE header  ]
[  Reduced overhead header. ]
[  RTCP payload .]

*/

RIST_PACKED_STRUCT(rist_gre_keepalive,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
	uint8_t mac_array[6];
	uint8_t capabilities1;
	uint8_t capabilities2;
})

RIST_PACKED_STRUCT(rist_gre_hdr,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
})

RIST_PACKED_STRUCT(rist_gre,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
	uint32_t checksum_reserved1;
})

RIST_PACKED_STRUCT(rist_gre_seq,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
	uint32_t checksum_reserved1;
	uint32_t seq;
})

RIST_PACKED_STRUCT(rist_gre_key_seq_real,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
	uint32_t nonce;
	uint32_t seq;
})

RIST_PACKED_STRUCT(rist_gre_key_seq,{
	uint8_t flags1;
	uint8_t flags2;
	uint16_t prot_type;
	uint32_t checksum_reserved1;
	uint32_t nonce;
	uint32_t seq;
})

RIST_PACKED_STRUCT(rist_rtp_hdr,{
	uint8_t flags;
	uint8_t payload_type;
	uint16_t seq;
	uint32_t ts;
	uint32_t ssrc;
})

RIST_PACKED_STRUCT(rist_rtp_hdr_ext, {
	uint16_t identifier; /* set to 0x5249 */
	uint16_t length; /*shall be set to 1 */
	uint8_t	 flags;
	uint8_t  npd_bits;
	uint16_t seq_ext;
})

RIST_PACKED_STRUCT(rist_protocol_hdr,{
	uint16_t src_port;
	uint16_t dst_port;
	struct rist_rtp_hdr rtp;
})

RIST_PACKED_STRUCT(rist_rtp_nack_record,{
	uint16_t start;
	uint16_t extra;
})

RIST_PACKED_STRUCT(rist_rtcp_hdr,{
	uint8_t flags;
	uint8_t ptype;
	uint16_t len;
	uint32_t ssrc;
})

RIST_PACKED_STRUCT(rist_rtcp_nack_range,{
	uint8_t flags;
	uint8_t ptype;
	uint16_t len;
	uint32_t ssrc_source;
	uint8_t name[4];
})

RIST_PACKED_STRUCT(rist_rtcp_nack_bitmask,{
	uint8_t flags;
	uint8_t ptype;
	uint16_t len;
	uint32_t ssrc_source;
	uint32_t ssrc;
})

RIST_PACKED_STRUCT(rist_rtcp_seqext,{
	uint8_t flags;
	uint8_t ptype;
	uint16_t len;
	uint32_t ssrc;
	uint8_t  name[4];
	uint16_t seq_msb;
	uint16_t reserved0;
})

RIST_PACKED_STRUCT(rist_rtcp_echoext, {
	uint8_t flags;
	uint8_t ptype;
	uint16_t len;
	uint32_t ssrc;
	uint8_t name[4];
	uint32_t ntp_msw;
	uint32_t ntp_lsw;
	uint32_t delay;
})

RIST_PACKED_STRUCT(rist_rtcp_sr_pkt,{
	struct rist_rtcp_hdr rtcp;
	uint32_t ntp_msw;
	uint32_t ntp_lsw;
	uint32_t rtp_ts;
	uint32_t sender_pkts;
	uint32_t sender_bytes;
})

RIST_PACKED_STRUCT(rist_rtcp_rr_pkt,{
	struct rist_rtcp_hdr rtcp;
	uint32_t recv_ssrc;
	uint8_t fraction_lost;
	uint8_t cumulative_pkt_loss_msb;
	uint16_t cumulative_pkt_loss_lshw;
	uint32_t highest_seq;
	uint32_t jitter;
	uint32_t lsr;
	uint32_t dlsr;
})

RIST_PACKED_STRUCT(rist_rtcp_rr_empty_pkt,{
	struct rist_rtcp_hdr rtcp;
})

RIST_PACKED_STRUCT(rist_rtcp_sdes_pkt,{
	struct rist_rtcp_hdr rtcp;
	uint8_t cname;
	uint8_t name_len;
	char udn[1];
})

RIST_PACKED_STRUCT(rist_rtcp_xr_block_hdr, {
	uint8_t type;
	uint8_t reserved;
	uint16_t length;
})
//Receiver reference time report block
RIST_PACKED_STRUCT(rist_rtcp_xr_rrtrb, {
	uint8_t block_type;
	uint8_t reserved;
	uint16_t length;
	uint32_t ntp_msw;
	uint32_t ntp_lsw;
})

RIST_PACKED_STRUCT(rist_rtcp_xr_dlrr, {
	uint8_t block_type;
	uint8_t reserved;
	uint16_t length;
	uint32_t ssrc;
	uint32_t lrr;
	uint32_t delay;
})

/* shared functions in udp.c */
RIST_PRIV void rist_send_nacks(struct rist_flow *f, struct rist_peer *peer);
RIST_PRIV int rist_receiver_send_nacks(struct rist_peer *peer, uint32_t seq_array[], size_t array_len);
RIST_PRIV int rist_receiver_periodic_rtcp(struct rist_peer *peer);
RIST_PRIV void rist_sender_periodic_rtcp(struct rist_peer *peer);
RIST_PRIV int rist_respond_echoreq(struct rist_peer *peer, const uint64_t echo_request_time, uint32_t ssrc);
RIST_PRIV int rist_request_echo(struct rist_peer *peer);
RIST_PRIV int rist_send_common_rtcp(struct rist_peer *p, uint8_t payload_type, uint8_t *payload, size_t payload_len, uint64_t source_time, uint16_t src_port, uint16_t dst_port, uint32_t seq_rtp);
RIST_PRIV void rist_sender_send_data_balanced(struct rist_sender *ctx, struct rist_buffer *buffer);
RIST_PRIV int rist_sender_enqueue(struct rist_sender *ctx, const void *data, size_t len, uint64_t datagram_time, uint16_t src_port, uint16_t dst_port, uint32_t seq_rtp);
RIST_PRIV void rist_clean_sender_enqueue(struct rist_sender *ctx);
RIST_PRIV void rist_retry_enqueue(struct rist_sender *ctx, uint32_t seq, struct rist_peer *peer);
RIST_PRIV ssize_t rist_retry_dequeue(struct rist_sender *ctx);
RIST_PRIV int rist_set_url(struct rist_peer *peer);
RIST_PRIV void rist_create_socket(struct rist_peer *peer);
RIST_PRIV size_t rist_get_sender_retry_queue_size(struct rist_sender *ctx);

RIST_PRIV uint64_t timestampNTP_u64(void);
RIST_PRIV uint64_t timestampNTP_RTC_u64(void);
RIST_PRIV uint32_t timestampRTP_u32(int advanced, uint64_t i_ntp);
RIST_PRIV uint64_t convertRTPtoNTP(uint8_t ptype, uint32_t time_extension, uint32_t i_rtp);
RIST_PRIV uint64_t calculate_rtt_delay(uint64_t request, uint64_t response, uint32_t delay);

static inline uint32_t get_rtp_ts_clock(uint8_t ptype) {
	uint32_t clock = 0;
	switch(ptype) {
		case RTP_PTYPE_MPEGTS:
		case 14: // MPA
		case 25: // CelB
		case 26: // JPEG
		case 28: // nv
		case 31: // H261
		case 32: // MPV
		case 34: // H263
			clock = RTP_PTYPE_MPEGTS_CLOCKHZ;
			break;
		case 0: // PCMU
		case 3: // GSM
		case 4: // G723
		case 5: // DVI4
		case 7: // LPC
		case 8: // PCMA
		case 9: // G722
		case 12: // QCELP
		case 13: // CN
		case 15: // G728
		case 18: // G729
			clock = 8000;
			break;
		case 16: // DVI4
			clock = 11025;
			break;
		case 6: // DVI4
			clock = 16000;
			break;
		case 17: // DVI4
			clock = 22050;
			break;
		case 10: // L16
		case 11: // L16
			clock = 44100;
			break;
		default:
			clock = 0;
			break;
	}
	return clock;
}

#endif
