# librist. Copyright (c) 2020 SipRadius LLC. All right reserved.
# Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
# SPDX-License-Identifier: BSD-2-Clause

if not builtin_mbedtls
    mbedcrypto_lib = dependency('mbedcrypto', required: false)
    if not mbedcrypto_lib.found()
        mbedcrypto_lib = cc.find_library('mbedcrypto', required: required_library, has_headers: ['mbedtls/aes.h'])
        if not mbedcrypto_lib.found()
            builtin_mbedtls = true
        endif
    endif
    if mbedcrypto_lib.found()
        mbedcrypto_lib_found = true
    endif
endif
if builtin_mbedtls and not mbedcrypto_lib_found
    message('Using builtin mbedtls code')
    mbedcrypto_lib_includes = include_directories('library','include')
    add_project_arguments(['-D_FILE_OFFSET_BITS=64'], language: 'c')
    mbedcrypto_lib = declare_dependency(
            sources: [
                    'library/aes.c',
                    'library/aesni.c',
                    'library/arc4.c',
                    'library/aria.c',
                    'library/asn1parse.c',
                    'library/asn1write.c',
                    'library/base64.c',
                    'library/bignum.c',
                    'library/blowfish.c',
                    'library/camellia.c',
                    'library/ccm.c',
                    'library/chacha20.c',
                    'library/chachapoly.c',
                    'library/cipher.c',
                    'library/cipher_wrap.c',
                    'library/cmac.c',
                    'library/ctr_drbg.c',
                    'library/des.c',
                    'library/dhm.c',
                    'library/ecdh.c',
                    'library/ecdsa.c',
                    'library/ecjpake.c',
                    'library/ecp.c',
                    'library/ecp_curves.c',
                    'library/entropy.c',
                    'library/entropy_poll.c',
                    'library/error.c',
                    'library/gcm.c',
                    'library/havege.c',
                    'library/hkdf.c',
                    'library/hmac_drbg.c',
                    'library/md.c',
                    'library/md2.c',
                    'library/md4.c',
                    'library/md5.c',
                    'library/memory_buffer_alloc.c',
                    'library/nist_kw.c',
                    'library/oid.c',
                    'library/padlock.c',
                    'library/pem.c',
                    'library/pk.c',
                    'library/pk_wrap.c',
                    'library/pkcs12.c',
                    'library/pkcs5.c',
                    'library/pkparse.c',
                    'library/pkwrite.c',
                    'library/platform.c',
                    'library/platform_util.c',
                    'library/poly1305.c',
                    'library/psa_crypto.c',
                    'library/psa_crypto_client.c',
                    'library/psa_crypto_driver_wrappers.c',
                    'library/psa_crypto_ecp.c',
                    'library/psa_crypto_rsa.c',
                    'library/psa_crypto_se.c',
                    'library/psa_crypto_slot_management.c',
                    'library/psa_crypto_storage.c',
                    'library/psa_its_file.c',
                    'library/ripemd160.c',
                    'library/rsa.c',
                    'library/rsa_internal.c',
                    'library/sha1.c',
                    'library/sha256.c',
                    'library/sha512.c',
                    'library/threading.c',
                    'library/timing.c',
                    'library/version.c',
                    'library/version_features.c',
                    'library/xtea.c',],
            include_directories: mbedcrypto_lib_includes
        )
    inc += mbedcrypto_lib_includes
    mbedcrypto_lib_found = true
    # We could compile libmbedcrypto.a using make on contrib/mbedtls and copy to the build folder instead
    #my_mbedcrypto_inc = mbedcrypto_lib_includes
    #my_mbedcrypto_lib = static_library('mbedcrypto')
    #mbedcrypto_lib = declare_dependency(link_with : my_mbedcrypto_lib, include_directories : my_mbedcrypto_inc)
endif
if mbedcrypto_lib_found
    deps += mbedcrypto_lib
    platform_files += 'contrib/srp.c'
    platform_files += 'src/eap.c'
endif