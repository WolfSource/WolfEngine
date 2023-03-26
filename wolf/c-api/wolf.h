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
  CharCastError,
  IntCastError,
  InvalidParameter,
  MediaAudioHostWasNotFound,
  MediaAudioStreamConfigNotFound,
  MediaAudioSupportedInputConfigNotFound,
  MediaAvCodecOpenFailed,
  MediaAvFormatContextAllocFailed,
  MediaCodecNotFound,
  MediaDecodeReceivePacketFailed,
  MediaDecodeSendFrameFailed,
  MediaEncodeReceivePacketFailed,
  MediaEncodeSendFrameFailed,
  MediaImageFillFailed,
  MediaInitParserFailed,
  MediaInvalidAvConfig,
  MediaInvalidAvSetOption,
  MediaInvalidSwsContext,
  MediaInvalidVideoFrameSize,
  MediaParserFailed,
  MediaStreamInfoNotFound,
  MediaSwsScaleFailed,
  MediaUrlInvalid,
  MemoryWasNotInitialized,
  OutOfMemory,
  SizeCastError,
  StreamGrpcAddrressParseFailed,
  StreamGrpcChannelError,
  StreamGrpcInvalidTlsConfig,
  StreamGrpcMissingTlsConfig,
  StreamGrpcServerBuildFailed,
  StreamGrpcServerInvalidUri,
  StreamGrpcTlsCrtNotFound,
  StreamGrpcTlsKeyNotFound,
  SystemLZ4InvalidDestinationSize,
  SystemLZ4InvalidSourceSize,
  SystemLZMACompressionFailed,
  SystemLZMAInvalidDestinationSize,
  SystemLZMAInvalidSourceSize,
  SystemPollFailed,
  SystemPollRegisterFailed,
  SystemSocketAddressParseFailed,
  SystemSocketBindFailed,
  SystemSocketConnectFailed,
  SystemSocketConnectTimeout,
  SystemSocketLoadLocalAddressFailed,
  SystemSocketSSLAcceptFailed,
  SystemSocketSSLBuilderFailed,
  SystemSocketSSLConnectFailed,
  SystemSocketSSLHandshakeTimeout,
  SystemSocketUtf8Error,
  SystemTCPSocketReadFailed,
  SystemTCPSocketWriteFailed,
  SystemUDPSocketReadFailed,
  SystemUDPSocketWriteFailed,
  Unknown,
} WError;

typedef struct AvConfig AvConfig;

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
WError w_media_av_config_audio_new(AvConfig **p_obj,
                                   uint32_t p_channels,
                                   uint32_t p_sample_rate,
                                   int32_t p_sample_fmt);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_config_video_new(AvConfig **p_obj,
                                   int32_t p_pixel_format,
                                   uint32_t p_width,
                                   uint32_t p_height,
                                   uint32_t p_alignment);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_audio_config_set(AvConfig *p_obj,
                                   uint32_t p_channels,
                                   uint32_t p_sample_rate,
                                   int32_t p_sample_fmt);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_media_av_audio_config_free(AvConfig **p_ptr);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
WError w_media_av_video_config_set(AvConfig *p_obj,
                                   int32_t p_pixel_format,
                                   uint32_t p_width,
                                   uint32_t p_height,
                                   uint32_t p_alignment);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
int32_t w_media_av_video_config_get_required_buffer_size(AvConfig *p_obj);

/**
 * # Safety
 *
 * unsafe function for C ABI
 */
void w_media_av_video_config_free(AvConfig **p_ptr);

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
