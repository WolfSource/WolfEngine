/*
 * Copyright © 2020, VideoLAN and librist authors
 * Copyright © 2019-2020 SipRadius LLC
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _LIBRIST_SRP_H_
#define _LIBRIST_SRP_H_
#include "librist_config.h"
#if HAVE_MBEDTLS
#include "librist.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SRP User lookup function
 *
 *  The SRP User lookup function is called inside the process of authentication
 *  the calling application MUST implement this function if it desires to so run
 *  as a RIST MAIN profile server with SRP authentication enabled.
 *  Userlookup is assumed to have been successful if both verifier params and both
 *  salt params are set by the lookup function.
 *  libRIST will take ownership of all heap allocated data.
 *
 *  @param username IN the username attempting authentication
 *  @param verifier_len OUT len in bytes of the verifier.
 *  @param verifier OUT verifier bytes, MUST be heap allocated.
 *  @param salt_len OUT salt len in bytes of the salt.
 *  @param salt OUT salt bytes, MUST be heap allocated.
 *  @param use_default_2048_bit_n_modulus OUT Use the default 2048 bit modulus, when true N Prime modulus & generator MUST be NULL.
 *  @param n_modulus_ascii OUT N Prime modulus in hex as a zero terminated C string, MUST be heap allocated or NULL.
 *  @param generator_ascii OUT Generator in hex as a zero terminated C string, MUST be heap allocated or NULL.
 *  @param user_data IN pointer to user data.
 *
 **/
typedef void (*user_verifier_lookup_t)(char * username,
									   size_t *verifier_len, char **verifier,
									   size_t *salt_len, char **salt,
									   bool *use_default_2048_bit_n_modulus,
									   char **n_modulus_ascii,
									   char **generator_ascii,
									   void *user_data);

/**
 * @brief Enable SRP authentication
 *
 * This will enable SRP authentication on RIST MAIN profile connections. When
 * running in client mode username and password are mandatory parameters. Else
 * only a lookup function must be provided.
 *
 * @param peer IN Peer object (connection) upon which to enable SRP authentication.
 * @param username IN username MANDATORY in client mode.
 * @param password IN password MANDATORY in client mode.
 * @param lookup_func IN @see *user_verifier_lookup_t MANDATORY in server mode.
 * @param userdata IN optional user data to be supplied to lookup function.
 *
 **/
RIST_API int rist_enable_eap_srp(struct rist_peer *peer, const char *username, const char *password, user_verifier_lookup_t lookup_func, void *userdata);

#ifdef __cplusplus
}
#endif
#endif
#endif
