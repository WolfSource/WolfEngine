
# Also see "include/mbedtls/config.h"

CFLAGS	?= -O2
WARNING_CFLAGS ?=  -Wall -Wextra -Wformat=2 -Wno-format-nonliteral
LDFLAGS ?=

# Include ./include for public headers and ./library for private headers.
LOCAL_CFLAGS = $(WARNING_CFLAGS) -I./library -I./include -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS =

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

# if were running on Windows build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
else ifeq ($(shell uname -s),Darwin)
ifeq ($(AR),ar)
APPLE_BUILD ?= 1
endif
endif

ifndef WINDOWS_BUILD
LOCAL_CFLAGS += -fPIC -fpic
endif
SOEXT_TLS=so.13
SOEXT_X509=so.1
SOEXT_CRYPTO=so.6

# Set AR_DASH= (empty string) to use an ar implementation that does not accept
# the - prefix for command line options (e.g. llvm-ar)
AR_DASH ?= -

ARFLAGS = $(AR_DASH)src
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
ARFLAGS = $(AR_DASH)Src
RLFLAGS = -no_warning_for_no_symbols -c
RL ?= ranlib
endif
endif

DLEXT ?= so
ifdef WINDOWS_BUILD
# Windows shared library extension:
DLEXT = dll
else ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
# Mac OS X shared library extension:
DLEXT = dylib
endif
endif

OBJS_CRYPTO= \
	     library/aes.o \
	     library/aesni.o \
	     library/arc4.o \
	     library/aria.o \
	     library/asn1parse.o \
	     library/asn1write.o \
	     library/base64.o \
	     library/bignum.o \
	     library/blowfish.o \
	     library/camellia.o \
	     library/ccm.o \
	     library/chacha20.o \
	     library/chachapoly.o \
	     library/cipher.o \
	     library/cipher_wrap.o \
	     library/cmac.o \
	     library/ctr_drbg.o \
	     library/des.o \
	     library/dhm.o \
	     library/ecdh.o \
	     library/ecdsa.o \
	     library/ecjpake.o \
	     library/ecp.o \
	     library/ecp_curves.o \
	     library/entropy.o \
	     library/entropy_poll.o \
	     library/error.o \
	     library/gcm.o \
	     library/havege.o \
	     library/hkdf.o \
	     library/hmac_drbg.o \
	     library/md.o \
	     library/md2.o \
	     library/md4.o \
	     library/md5.o \
	     library/memory_buffer_alloc.o \
	     library/nist_kw.o \
	     library/oid.o \
	     library/padlock.o \
	     library/pem.o \
	     library/pk.o \
	     library/pk_wrap.o \
	     library/pkcs12.o \
	     library/pkcs5.o \
	     library/pkparse.o \
	     library/pkwrite.o \
	     library/platform.o \
	     library/platform_util.o \
	     library/poly1305.o \
	     library/psa_crypto.o \
	     library/psa_crypto_client.o \
	     library/psa_crypto_driver_wrappers.o \
	     library/psa_crypto_ecp.o \
	     library/psa_crypto_rsa.o \
	     library/psa_crypto_se.o \
	     library/psa_crypto_slot_management.o \
	     library/psa_crypto_storage.o \
	     library/psa_its_file.o \
	     library/ripemd160.o \
	     library/rsa.o \
	     library/rsa_internal.o \
	     library/sha1.o \
	     library/sha256.o \
	     library/sha512.o \
	     library/threading.o \
	     library/timing.o \
	     library/version.o \
	     library/version_features.o \
	     library/xtea.o \
	     # This line is intentionally left blank

.SILENT:

.PHONY: all static clean

all: static

static: libmbedcrypto.a

# crypto
libmbedcrypto.a: $(OBJS_CRYPTO)
	echo "  AR    $@"
	$(AR) $(ARFLAGS) $@ $(OBJS_CRYPTO)
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
	echo "  RL    $@"
	$(RL) $(RLFLAGS) $@
endif
endif

.c.o:
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

clean:
ifndef WINDOWS
	rm -f ./library/*.o libmbed*
else
	if exist ./library/*.o del /Q /F ./library/*.o
	if exist libmbed* del /Q /F libmbed*
endif
