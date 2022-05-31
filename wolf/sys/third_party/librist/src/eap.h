/* librist. Copyright © 2020 SipRadius LLC. All right reserved.
 * Author: Gijs Peskens <gijs@in2ip.nl>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _EAP_H_
#define _EAP_H_

#include "common/attributes.h"

#include "srp.h"
#include "librist/librist_srp.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//802.1X-2010 Section 11
#define EAPOL_TYPE_EAP 0
#define EAPOL_TYPE_START 1
#define EAPOL_TYPE_LOGOFF 2

RIST_PACKED_STRUCT(eapol_hdr, {
	uint8_t eapversion;
	uint8_t eaptype;
	uint16_t length;
})

//https://tools.ietf.org/html/rfc3748
#define EAP_CODE_REQUEST 1
#define EAP_CODE_RESPONSE 2
#define EAP_CODE_SUCCESS 3
#define EAP_CODE_FAILURE 4

RIST_PACKED_STRUCT(eap_hdr, {
	uint8_t code;
	uint8_t identifier;
	uint16_t length;
})

#define EAPOL_EAP_HDRS_OFFSET sizeof(struct rist_gre_hdr) + sizeof(struct eapol_hdr) + sizeof(struct eap_hdr)

#define EAP_TYPE_IDENTITY 1
#define EAP_TYPE_NOTIFICATION 2
#define EAP_TYPE_NAK 3
#define EAP_TYPE_MD5_CHALLENGE 4

//https://tools.ietf.org/html/draft-ietf-pppext-eap-srp-03
#define EAP_TYPE_SRP_SHA1 19

//requests
#define EAP_SRP_SUBTYPE_CHALLENGE 1
#define EAP_SRP_SUBTYPE_SERVER_KEY 2

//responses
#define EAP_SRP_SUBTYPE_CLIENT_KEY 1
#define EAP_SRP_SYPTYPE_CLIENT_VALIDATOR 2

//either
#define EAP_SRP_SUBTYPE_SERVER_VALIDATOR 3
#define EAP_SRP_SUBTYPE_LWRECHALLENGE 4

RIST_PACKED_STRUCT(eap_srp_hdr, {
	uint8_t type;
	uint8_t subtype;
})

#define EAP_ROLE_AUTHENTICATEE 0
#define EAP_ROLE_AUTHENTICATOR 1

#define EAP_AUTH_STATE_FAILED -1
#define EAP_AUTH_STATE_UNAUTH 0
#define EAP_AUTH_STATE_SUCCESS 1
#define EAP_AUTH_STATE_REAUTH 2

struct eapsrp_ctx
{
	uint_fast8_t role;
	uint8_t last_identifier;
	int authentication_state;
	uint8_t tries;

	uint8_t *last_pkt;
	size_t last_pkt_size;
	uint8_t timeout_retries;
	uint64_t last_timestamp;
	uint64_t last_auth_timestamp;

	char username[256];
	char password[256];

	char *salt;
	size_t salt_len;
	char *verifier;
	size_t verifier_len;
	bool default_2048_ng;
	char *ascii_n;
	char *ascii_g;

	user_verifier_lookup_t lookup_func;
	void *lookup_func_userdata;
	struct SRPSession *srp_session;
	struct SRPUser *srp_user;
	struct SRPVerifier *srp_verifier;
	struct rist_peer *peer;
	char ip_string[46];
	struct rist_logging_settings *logging_settings;

	// authenticator data (single user mode)
	char authenticator_username[256];
	size_t authenticator_len_verifier;
	char *authenticator_bytes_verifier;
	size_t authenticator_len_salt;
	char *authenticator_bytes_salt;
};

#define EAP_LENERR -1
#define EAP_WRONGIDENTIFIER -2
#define EAP_UNEXPECTEDRESPONSE -3
#define EAP_UNEXPECTEDREQUEST -4
#define EAP_SRP_WRONGSUBTYPE -4

RIST_PRIV int eap_process_eapol(struct eapsrp_ctx* ctx, uint8_t pkt[], size_t len);
RIST_PRIV int eap_request_identity(struct eapsrp_ctx *ctx);
RIST_PRIV int eap_start(struct eapsrp_ctx *ctx);
RIST_PRIV void eap_periodic(struct eapsrp_ctx *ctx);
RIST_PRIV bool eap_is_authenticated(struct eapsrp_ctx *ctx);
RIST_PRIV void eap_delete_ctx(struct eapsrp_ctx **in);
RIST_PRIV int eap_clone_ctx(struct eapsrp_ctx *in, struct rist_peer *peer);
RIST_PRIV void eap_set_ip_string(struct eapsrp_ctx *ctx, char ip_string[]);
#endif
