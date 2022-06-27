/*
       Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
       https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    typedef enum w_ffmpeg_action
    {
        W_ENCODE,
        W_DECODE
    }w_ffmpeg_action;

struct w_ffmpeg_ctx_t;
typedef struct w_ffmpeg_ctx_t* w_ffmpeg_ctx;

typedef struct w_ffmpeg_opt_t {
   int fps;
   int64_t width;
   int64_t height;
   int64_t bitrate;
   int codec_id;
   int preset;
   int crf;
   w_ffmpeg_action type;
   w_ffmpeg_ctx ctx;
}w_ffmpeg_opt_t;

typedef w_ffmpeg_opt_t* w_ffmpeg_opt;

/**
* initialize ffmpeg
* @param p_ffmpeg_opt the ffmpeg options
* @param p_error the error buffer
* @return int the result of encoding the frame
*/
W_EXPORT
   int w_ffmpeg_init(w_ffmpeg_opt p_ffmpeg_opt, char *p_error);

/**
* encode the frame
* @param p_ffmpeg_opt the ffmpeg options
* @param p_data_in the input rgb buffer
* @param p_data_out the output encoded buffer
* @param p_size_out the size of encoded buffer
* @param p_error the error buffer
* @return int the result of encoding the frame
*/
W_EXPORT
   int w_ffmpeg_encode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t *p_data_in, uint8_t **p_data_out, int *p_size_out, char *p_error);

/**
* decode the frame
* @param p_ffmpeg_opt the ffmpeg options
* @param p_data_in the input decode data
* @param p_data_size the size of input decode data
* @param p_data_out the output buffer containing the decoded data
* @param p_size_out the size of decoded buffer
* @param p_error the error buffer
* @return int the result of encoding the frame
*/
W_EXPORT
   int w_ffmpeg_decode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t *p_data_in, int p_data_size, uint8_t **p_data_out,int *p_size_out, char *p_error);

/**
* encode the frame
* @param p_ffmpeg_ctx the ffmpeg parameters
* @param p_error the error buffer
*/
W_EXPORT
   void w_ffmpeg_fini(w_ffmpeg_opt p_ffmpeg_opt);

#ifdef __cplusplus
}
#endif