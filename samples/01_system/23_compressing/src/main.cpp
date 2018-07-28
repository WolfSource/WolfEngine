/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to use fiber
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include <w_timer.h>
#include <lz4/lz4.h>
#include <lz4/lz4frame.h>

//namespaces
using namespace wolf;
using namespace wolf::system;

#define IN_CHUNK_SIZE  (16*1024)

static const LZ4F_preferences_t kPrefs =
{
    { LZ4F_max256KB, LZ4F_blockLinked, LZ4F_noContentChecksum, LZ4F_frame,
        0 /* unknown content size */, 0 /* no dictID */ , LZ4F_noBlockChecksum },
    0,   /* compression level; 0 == default */
    0,   /* autoflush */
    0,   /* favor decompression speed */
    { 0, 0, 0 },  /* reserved, must be set to 0 */
};


/* safe_fwrite() :
 * performs fwrite(), ensure operation success, or immediately exit() */
static void safe_fwrite(void* buf, size_t eltSize, size_t nbElt, FILE* f)
{
    size_t const writtenSize = fwrite(buf, eltSize, nbElt, f);
    size_t const expectedSize = eltSize * nbElt;   /* note : should check for overflow */
    if (writtenSize < expectedSize) {
        if (ferror(f))  /* note : ferror() must follow fwrite */
            printf("Write failed\n");
        else
            printf("Short write\n");
        exit(1);
    }
}

typedef struct
{
    int error;
    unsigned long long size_in;
    unsigned long long size_out;
} compressResult_t;

static compressResult_t
compress_file_internal(FILE* f_in, FILE* f_out,
                       LZ4F_compressionContext_t ctx,
                       void* inBuff, size_t inChunkSize,
                       void* outBuff, size_t outCapacity)
{
    compressResult_t result = { 1, 0, 0 };  /* result for an error */
    unsigned long long count_in = 0, count_out;
    
    assert(f_in != NULL); assert(f_out != NULL);
    assert(ctx != NULL);
    assert(outCapacity >= LZ4F_HEADER_SIZE_MAX);
    assert(outCapacity >= LZ4F_compressBound(inChunkSize, &kPrefs));
    
    /* write frame header */
    {   size_t const headerSize = LZ4F_compressBegin(ctx, outBuff, outCapacity, &kPrefs);
        if (LZ4F_isError(headerSize)) {
            printf("Failed to start compression: error %zu\n", headerSize);
            return result;
        }
        count_out = headerSize;
        printf("Buffer size is %zu bytes, header size %zu bytes\n", outCapacity, headerSize);
        safe_fwrite(outBuff, 1, headerSize, f_out);
    }
    
    /* stream file */
    for (;;) {
        size_t const readSize = fread(inBuff, 1, IN_CHUNK_SIZE, f_in);
        if (readSize == 0) break; /* nothing left to read from input file */
        count_in += readSize;
        
        size_t const compressedSize = LZ4F_compressUpdate(ctx,
                                                          outBuff, outCapacity,
                                                          inBuff, readSize,
                                                          NULL);
        if (LZ4F_isError(compressedSize)) {
            printf("Compression failed: error %zu\n", compressedSize);
            return result;
        }
        
        printf("Writing %zu bytes\n", compressedSize);
        safe_fwrite(outBuff, 1, compressedSize, f_out);
        count_out += compressedSize;
    }
    
    /* flush whatever remains within internal buffers */
    {   size_t const compressedSize = LZ4F_compressEnd(ctx,
                                                       outBuff, outCapacity,
                                                       NULL);
        if (LZ4F_isError(compressedSize)) {
            printf("Failed to end compression: error %zu\n", compressedSize);
            return result;
        }
        
        printf("Writing %zu bytes\n", compressedSize);
        safe_fwrite(outBuff, 1, compressedSize, f_out);
        count_out += compressedSize;
    }
    
    result.size_in = count_in;
    result.size_out = count_out;
    result.error = 0;
    return result;
}

static compressResult_t
compress_file(FILE* f_in, FILE* f_out)
{
    assert(f_in != NULL);
    assert(f_out != NULL);
    
    /* ressource allocation */
    LZ4F_compressionContext_t ctx;
    size_t const ctxCreation = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);
    void* const src = malloc(IN_CHUNK_SIZE);
    size_t const outbufCapacity = LZ4F_compressBound(IN_CHUNK_SIZE, &kPrefs);   /* large enough for any input <= IN_CHUNK_SIZE */
    void* const outbuff = malloc(outbufCapacity);
    
    compressResult_t result = { 1, 0, 0 };  /* == error (default) */
    if (!LZ4F_isError(ctxCreation) && src && outbuff) {
        result = compress_file_internal(f_in, f_out,
                                        ctx,
                                        src, IN_CHUNK_SIZE,
                                        outbuff, outbufCapacity);
    } else {
        printf("error : ressource allocation failed \n");
    }
    
    LZ4F_freeCompressionContext(ctx);   /* supports free on NULL */
    free(src);
    free(outbuff);
    return result;
}

WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"23_compressing";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    logger.initialize(_log_config);
    
    //log to output file
    logger.write(L"Wolf initialized");
    
    logger.write("Compressing using LZ4 version: {}", LZ4_versionNumber());
    
    
    w_timer _timer;
    _timer.start();
    
    //compress a file
    FILE* const _ifile = fopen("/Users/pooyaeimandar/Desktop/H.mp4", "rb");
    FILE* const _ofile = fopen("/Users/pooyaeimandar/Desktop/H_com.mp4", "wb");

    compressResult_t const ret = compress_file(_ifile, _ofile);

    fclose(_ifile);
    fclose(_ofile);
    
    _timer.stop();
    
    logger.write("time is: {}", _timer.get_milliseconds() / 1000.000f);
    
    //release logger
    logger.release();

    return EXIT_SUCCESS;
}
