#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

typedef enum {
  BINARY = 0,
  TEXT,
} BufferType;

typedef enum {
  None,
  MemoryWasNotInitialized,
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

typedef struct AvVideoConfig AvVideoConfig;

typedef struct TcpClient TcpClient;

typedef struct {
  BufferType type_;
  uintptr_t size;
  uint8_t buf[MAX_BUFFER_SIZE];
} Buffer;

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
WError w_media_av_audio_config_init(AvAudioConfig **p_obj,
                                    uint32_t p_channels,
                                    uint32_t p_sample_rate,
                                    int32_t p_sample_fmt);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_audio_config_set(AvAudioConfig *p_obj,
                                   uint32_t p_channels,
                                   uint32_t p_sample_rate,
                                   int32_t p_sample_fmt);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_media_av_audio_config_fini(AvAudioConfig **p_ptr);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_video_config_init(AvVideoConfig **p_obj,
                                    int32_t p_pixel_format,
                                    uint32_t p_width,
                                    uint32_t p_height,
                                    uint32_t p_alignment);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_video_config_set(AvVideoConfig *p_obj,
                                   int32_t p_pixel_format,
                                   uint32_t p_width,
                                   uint32_t p_height,
                                   uint32_t p_alignment);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
int32_t w_media_av_video_config_get_required_buffer_size(AvVideoConfig *p_obj);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_media_av_video_config_fini(AvVideoConfig **p_ptr);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
TcpClient *w_system_socket_tcp_client_new(uint32_t p_connect_number_of_retry,
                                          double p_connect_timeout_in_secs,
                                          const char *p_endpoint_address,
                                          uint32_t p_io_number_of_retry,
                                          double p_io_timeout_in_secs,
                                          uint16_t p_port,
                                          bool p_tls,
                                          WError *p_error);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_system_socket_tcp_client_connect(TcpClient *p_socket,
                                          WError (**p_on_accept_connection)(const char*),
                                          WError (**p_on_message)(const char*, Buffer*, double),
                                          WError (**p_on_close_connection)(const char*));

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_system_socket_tcp_client_free(TcpClient *p_ptr);
