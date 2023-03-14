#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

typedef enum {
  None,
  InvalidParameter,
  OutOfMemory,
  IntCastError,
  SizeCastError,
  MediaInvalidVideoFrameSize,
  MediaInvalidAvConfig,
  MediaImageFillFailed,
  MediaInvalidSwsContext,
  MediaSwsScaleFailed,
  MediaInvalidAvSetOption,
  SystemSocketAddressParseFailed,
  SystemPollFailed,
  SystemSocketConnectFailed,
  SystemSocketBindFailed,
  SystemSocketSSLBuilderFailed,
  SystemSocketSSLAcceptFailed,
  SystemSocketSSLConnectFailed,
  SystemTCPSocketReadFailed,
  SystemTCPSocketWriteFailed,
  SystemUDPSocketReadFailed,
  SystemUDPSocketWriteFailed,
  SystemSocketConnectTimeout,
  SystemSocketSSLHandshakeTimeout,
  SystemSocketLoadLocalAddressFailed,
  SystemPollRegisterFailed,
  SystemSocketUtf8Error,
  Unknown,
} WError;

typedef struct AvAudioConfig AvAudioConfig;

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_version(int32_t *p_major, int32_t *p_minor, int32_t *p_patch);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
AvAudioConfig *w_media_av_audio_config_new(uint32_t p_channels,
                                           uint32_t p_sample_rate,
                                           int32_t p_sample_fmt,
                                           WError *p_error);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_media_av_audio_config_free(AvAudioConfig *p_ptr);
