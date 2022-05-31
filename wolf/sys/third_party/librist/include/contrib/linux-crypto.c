/* Copyright © 2014-2019, Stephan Mueller <smueller@chronox.de>
 * Copyright © 2020 in2ip B.V.
 * Linux Crypto API, based on libkcapi, relicensed to BSD-2 with author's permission
 * Author: Stephan Mueller <smueller@chronox.de>
 * Modified for librist by: Gijs Peskens <gijs@in2ip.nl>
 * Modified for librist by: Sergio Ammirata <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifdef __linux
#include "linux-crypto.h"

struct linux_crypto {
	int sockfd;
	int cryptofd;
	struct kcapi_handle *handle;
};


int32_t _linux_crypto_send_crypt(struct linux_crypto *ctx,
				struct iovec *iov, uint32_t iovlen,
				uint32_t enc, const uint8_t *iv)
{
	int32_t ret;
	char buffer_static[80] = { 0 };
	char *buffer_p = buffer_static;
	char *buffer_alloc = NULL;

	/* plaintext / ciphertext data */
	struct cmsghdr *header = NULL;
	uint32_t *type = NULL;
	struct msghdr msg;

	/* IV data */
	struct af_alg_iv *alg_iv = NULL;
	uint32_t iv_msg_size = CMSG_SPACE(sizeof(*alg_iv) + 16);


	uint32_t bufferlen = CMSG_SPACE(sizeof(*type)) + 	/* Encryption / Decryption */
		iv_msg_size; 			/* IV */

	memset(&msg, 0, sizeof(msg));

	/* allocate buffer, if static buffer is too small */
	if (RIST_UNLIKELY(bufferlen > sizeof(buffer_static))) {
		buffer_alloc = calloc(1, bufferlen);
		if (!buffer_alloc)
			return -ENOMEM;
		buffer_p = buffer_alloc;
	}

	msg.msg_control = buffer_p;
	msg.msg_controllen = bufferlen;
	msg.msg_iov = iov;
	msg.msg_iovlen = iovlen;

	/* encrypt/decrypt operation */
	header = CMSG_FIRSTHDR(&msg);
	if (RIST_UNLIKELY(!header)) {
		ret = -EFAULT;
		goto out;
	}
	header->cmsg_level = SOL_ALG;
	header->cmsg_type = ALG_SET_OP;
	header->cmsg_len = CMSG_LEN(sizeof(*type));
	type = (void*)CMSG_DATA(header);
	*type = enc;

	/* set IV */
	header = CMSG_NXTHDR(&msg, header);
	if (RIST_UNLIKELY(!header)) {
		ret = -EFAULT;
		goto out;
	}
	header->cmsg_level = SOL_ALG;
	header->cmsg_type = ALG_SET_IV;
	header->cmsg_len = iv_msg_size;
	alg_iv = (void*)CMSG_DATA(header);
	alg_iv->ivlen = 16;
	memcpy(alg_iv->iv, iv, 16);


	ret = (int32_t)sendmsg(ctx->cryptofd, &msg, 0);
	if (RIST_UNLIKELY(ret < 0))
		ret = -errno;

out:
	if (buffer_alloc)
		free(buffer_alloc);
	return ret;
}

int32_t _linux_crypto_read(struct linux_crypto *ctx,
				   uint8_t *out, uint32_t outlen)
{
	int ret;
	int32_t totallen = 0;

	if (RIST_LIKELY(outlen)) {
		do {
			ret = (int32_t)read(ctx->cryptofd, out, outlen);
			if (ret > 0) {
				out += ret;
				outlen -= ret;
				totallen += ret;
			}
		} while ((ret > 0 || errno == EINTR) && outlen);

		if (RIST_UNLIKELY(ret < 0))
			return -errno;
	}

	return totallen;
}

int32_t _linux_crypto_process(struct linux_crypto *ctx,
				  const uint8_t *in, uint32_t inlen,
				  uint8_t *out, uint32_t outlen,
				  int access,const uint8_t *iv, int enc)
{
	RIST_MARK_UNUSED(access);
	int32_t totallen = 0;
	int32_t ret;

	struct iovec iov;

	if (RIST_UNLIKELY(!ctx->cryptofd)) {
		return -1;
	}

	while (inlen && outlen) {
		uint32_t inprocess = inlen;
		uint32_t outprocess = outlen;

		iov.iov_base = (void*)(uintptr_t)in;
		iov.iov_len = inprocess;
		ret = _linux_crypto_send_crypt(ctx, &iov, 1, enc, iv);
		if (RIST_UNLIKELY(0 > ret)) {
			return ret;
		}
		ret = _linux_crypto_read(ctx, out, outprocess);

		if (RIST_UNLIKELY(ret < 0))
			return ret;

		totallen += inprocess;
		in += inprocess;
		inlen -= inprocess;
		out += ret;
		outlen -= ret;
	}

	return totallen;
}

int linux_crypto_init(struct linux_crypto **_ctx) {

	struct linux_crypto *ctx = calloc(1, sizeof(*ctx));
	int ret;
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher", /* this selects the symmetric cipher */
		.salg_name = "ctr(aes)" /* this is the cipher name */
	};
	ctx->sockfd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (ctx->sockfd == -1) {
		fprintf(stderr, "Failed to set up socket!\n");
		free(ctx);
		return -1;
	}
	ret = bind(ctx->sockfd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret == -1) {
		fprintf(stderr, "Failed to bind to socket!\n");
		free(ctx);
		return ret;
	}

	*_ctx = ctx;
	return 0;

}

void linux_crypto_free(struct linux_crypto **ctx) {
	if (!ctx || !*ctx) {
		return;
	}

	if ((*ctx)->cryptofd)
		close((*ctx)->cryptofd);
	if ((*ctx)->sockfd)
		close((*ctx)->sockfd);
	free(*ctx);
	*ctx = NULL;
}

int linux_crypto_set_key(const uint8_t *key, int keylen, struct linux_crypto *ctx) {
	int ret;

	if (ctx->cryptofd) {
		close(ctx->cryptofd);
		ctx->cryptofd = 0;
	}

	ret = setsockopt(ctx->sockfd, SOL_ALG, ALG_SET_KEY, key, keylen);
	if (ret < 0) {
		fprintf(stderr, "Errno is %d\n", -errno);
		fprintf(stderr, "Failed to set key!\n");
		return -1;
	}

	ret = accept(ctx->sockfd, NULL, 0);
	if (ret == -1) {
		fprintf(stderr, "Failed to accept socket!\n");
		return ret;
	}
	ctx->cryptofd = ret;
	return 0;
}

int linux_crypto_decrypt(uint8_t inbuf[], uint8_t outbuf[], int buflen, uint8_t iv[], struct linux_crypto *ctx) {
	return _linux_crypto_process(ctx, inbuf, buflen, outbuf, buflen, 0, iv, 0);
}
int linux_crypto_encrypt(uint8_t inbuf[], uint8_t outbuf[], int buflen, uint8_t iv[], struct linux_crypto *ctx) {
	return _linux_crypto_process(ctx, inbuf, buflen, outbuf, buflen, 1, iv, 0);
}

#endif
