/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_io.h
    Description      : This class is responsible to managing IO functions
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

typedef struct
{
    void*   buffer;
    size_t  size;
    size_t  bytes_read;
} w_file_input_stream;

typedef w_file_input_stream* w_file_istream;

typedef enum
{
    chromium,
    klomp_avx,
    fast_avx,
    fast_avx512,
    quick_time,
    scalar
} base_64_mode;

typedef enum
{
    //The red, green, and blue components in the image are stored in 3 bytes.
    RGB_PNG = 0,
    //The blue, green, and red components in the image are stored in 3 bytes.
    BGR_PNG,
    //The red, green, blue and alpha components in the image are stored in 4 bytes.
    RGBA_PNG,
    //The blue, green, red and alpha components in the image are stored in 4 bytes.
    BGRA_PNG
} w_png_pixel_format;

typedef enum
{
    //Unknown pixel format
    UNKNOWN_JPEG = -1,
    //The red, green, and blue components in the image are stored in 3 bytes.
    RGB_JPEG = 0,
    //The blue, green, and red components in the image are stored in 3 bytes.
    BGR_JPEG,
    //The red, green, blue and alpha components in the image are stored in 4 bytes.
    RGBX_JPEG,
    //The blue, green, red and alpha components in the image are stored in 4 bytes.
    BGRX_JPEG,
    //The blue, green, red and alpha components in the image are stored in 4 bytes.
    XBGR_JPEG,
    //The alpha, blue, green and red components in the image are stored in 4 bytes.
    XRGB_JPEG,
    //Grayscale pixel format.  Each 1-byte pixel represents a luminance * (brightness) level from 0 to 255.
    GRAY_JPEG,
    //This is the same as JPEG_RGBX, except that when decompressing, the X component is guaranteed to be 0xFF.
    RGBA_JPEG,
    //This is the same as JPEG_BGRX, except that when decompressing, the X component is guaranteed to be 0xFF.
    BGRA_JPEG,
    //This is the same as JPEG_XBGR, except that when decompressing, the X component is guaranteed to be 0xFF.
    ABGR_JPEG,
    //This is the same as JPEG_XRGB, except that when decompressing, the X component is guaranteed to be 0xFF.
    ARGB_JPEG,
    /*
        Unlike RGB, which is an additive color model used
        primarily for display, CMYK (Cyan/Magenta/Yellow/Key) is a subtractive
        color model used primarily for printing.  In the CMYK color model, the
        value of each color component typically corresponds to an amount of cyan,
        magenta, yellow, or black ink that is applied to a white background.  In
        order to convert between CMYK and RGB, it is necessary to use a color
        management system (CMS.)  A CMS will attempt to map colors within the
        printer's gamut to perceptually similar colors in the display's gamut and
        vice versa, but the mapping is typically not 1:1 or reversible, nor can it
        be defined with a simple formula.  Thus, such a conversion is out of scope
        for a codec library.  However, the TurboJPEG API allows for compressing
        CMYK pixels into a YCCK JPEG image (see #TJCS_YCCK) and decompressing YCCK
        JPEG images into CMYK pixels.
     */
    CMYK_JPEG,
} w_jpeg_pixel_format;

/**
 * create a file
 * @param pPath path of file
 * @param pContent content of file
 * @param pBinaryMode open file in binary mode (will be ignored on UNIX)
 * @param pBufferedMode open file in buffered mode
 * @param pNoneBlockMode open file in none block mode
 * @param pMultiThreadedMode allow multiple threads to use the file
 * @param pOpenAppendMode append to existing file
 * @param pIsLargFile is larg file
 * @param pErrorIfFileExists error whether file exists
 * @return file
*/
W_SYSTEM_EXPORT
w_file w_io_file_create(_In_z_  const char* pPath,
                        _In_z_  const char* pContent,
                        _In_    bool pBinaryMode,
                        _In_    bool pBufferedMode,
                        _In_    bool pNoneBlockMode,
                        _In_    bool pMultiThreadedMode,
                        _In_    bool pOpenAppendMode,
                        _In_    bool pIsLargFile,
                        _In_    bool pErrorIfFileExists);

/**
 * create and save content to file
 * @param pPath path of file
 * @param pContent content of file
 * @param pBinaryMode open file in binary mode (will be ignored on UNIX)
 * @param pBufferedMode open file in buffered mode
 * @param pNoneBlockMode open file in none block mode
 * @param pMultiThreadedMode allow multiple threads to use the file
 * @param pOpenAppendMode append to existing file
 * @param pIsLargFile is larg file
 * @param pErrorIfFileExists error whether file exists
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_save(_In_z_  const char* pPath,
                        _In_z_  const char* pContent,
                        _In_    bool pBinaryMode,
                        _In_    bool pBufferedMode,
                        _In_    bool pNoneBlockMode,
                        _In_    bool pMultiThreadedMode,
                        _In_    bool pOpenAppendMode,
                        _In_    bool pIsLargFile,
                        _In_    bool pErrorIfFileExists);

/**
 * is a file
 * @param pPath path of file
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_check_is_file(_In_z_ const char* pPath);

/**
 * get file info
 * @param pPath path of file
 * @return file info
*/
W_SYSTEM_EXPORT
w_file_info w_io_file_get_info_from_path(_In_z_ const char* pPath);

/**
 * get file info
 * @param pFile opened file
 * @return file info
*/
W_SYSTEM_EXPORT
w_file_info w_io_file_get_info(_In_ w_file pFile);

/**
 * get file extension from file path
 * @param pFilePath path to the file
 * @return extension of file
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_extension_from_path(_In_z_ const char* pFilePath);

/**
 * get file extension from file
 * @param pFile path to the file
 * @return extension of file
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_extension(_In_ w_file pFile);

/**
 * get file name from file path
 * @param pFilePath path to the file
 * @return name of file
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_name_from_path(_In_z_ const char* pFilePath);

/**
 * get file name from file
 * @param pFile path to the file
 * @return name of file
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_name(_In_ w_file pFile);

/**
 * pick off base name in filename before dot(without extension) from path
 * @param pPath path of file
 * @return base file name
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_base_name_from_path(_In_z_ const char* pPath);

/**
 * pick off base name in filename before dot(without extension) from file
 * @param pFile path of file
 * @return base file name
*/
W_SYSTEM_EXPORT
const char* w_io_file_get_base_name(_In_ w_file pFile);

/**
 * read the whole file with given path
 * @param pPath path of file
 * @return content of file, size and bytes read
*/
W_SYSTEM_EXPORT
w_file_istream w_io_file_read_full_from_path(_In_z_ const char* pPath);

/**
 * read the specific number of bytes from file with given path
 * @param pPath path of file
 * @param pNBytes number of bytes to read, set zero to read all the file bytes
 * @return content of file, size and bytes read
*/
W_SYSTEM_EXPORT
w_file_istream w_io_file_read_nbytes_from_path(_In_z_ const char* pPath, _In_ size_t pNBytes);

/**
 * read the whole file
 * @param pFile file
 * @return content of file, size and bytes read
*/
W_SYSTEM_EXPORT
w_file_istream w_io_file_read_full(_In_ w_file pFile);

/**
 * read the specific number of bytes from file with given path
 * @param pFile file
 * @param pNBytes number of bytes to read, set zero to read all the file bytes
 * @return content of file, size and bytes read
*/
W_SYSTEM_EXPORT
w_file_istream w_io_file_read_nbytes(_In_ w_file pFile, _In_ size_t pNBytes);

/**
 * delete a file with given path
 * @param pPath path of file
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_delete_from_path(_In_z_ const char* pPath);

/**
 * delete a file
 * @param pFile file which must be deleted
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_delete(_In_ w_file pFile);

/**
 * get current directory in char
 * @return current directory path
*/
W_SYSTEM_EXPORT
char* w_io_dir_get_current(void);

/**
 * check whether this path belongs to a directory
 * @param pPath path of directory
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_dir_check_is_directory(_In_z_ const char* pPath);

/**
 * get parent directory of given path
 * @param pPath path of directory
 * @return parent directory
*/
W_SYSTEM_EXPORT
const char* w_io_dir_get_parent(_In_z_ const char* pPath);

/**
 * create a directory
 * @param pPath path of directory
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_dir_create(_In_z_ const char* pPath);

/**
 * convert utf8 char* to wchar_t* 
 * @param pIn char* inputs,
 * @param pInLen lenght of inputs,
 * @param pOut outputs,
 * @param pOutLen lenght of output,
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_utf8_to_ucs2(
	_In_	char* pIn,
	_In_	size_t* pInLen,
	_Out_	uint16_t* pOut,
	_Out_	size_t* pOutLen);

/**
 * convert wchar_t* to utf8 char*
 * @param pIn wchar_t* inputs,
 * @param pInLen lenght of inputs,
 * @param pOut outputs,
 * @param pOutLen lenght of output,
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_ucs2_to_utf8(
	_In_	uint16_t* pIn,
	_In_	size_t* pInLen,
	_Out_	char* pOut,
	_Out_	size_t* pOutLen);

/**
 * convert hex string to decimal
 * @param pHexStr input string,
 * @return result decimal
*/
W_SYSTEM_EXPORT
long w_io_to_hex(const char* pHexStr);

/**
 * check "pString" string starts with "pStartWith" string
 * @param pString input string,
 * @param pStartWith start with,
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_string_has_start_with(_In_z_ const char* pString, _In_z_ const char* pStartWith);

/**
 * check "pString" string ends with "pEndWith" string
 * @param pString input string
 * @param pEndWith end eith
 * @return result decimal
*/
W_SYSTEM_EXPORT
W_RESULT w_io_string_has_end_with(_In_z_ const char* pString, _In_z_ const char* pEndWith);

/**
 * check "pString" string starts with "pStartWith" string
 * @param pString input string,
 * @param pStartWith start with,
 * @return result
*/
W_SYSTEM_EXPORT
W_RESULT w_io_wstring_has_start_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pStartWith);

/**
 * check "pString" string ends with "pEndWith" string
 * @param pString input string
 * @param pEndWith end eith
 * @return result decimal
*/
W_SYSTEM_EXPORT
W_RESULT w_io_wstring_has_end_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pEndWith);

/**
 * split string and returns all possible splits
 * @param pString input string
 * @param pSplit split strings
 * @param pResults split strings
 * @returns W_SUCCESS when function successed
 * @returns W_FAILURE when function failed
*/
W_SYSTEM_EXPORT
W_RESULT w_io_string_split(_In_z_ char* pString,
                           _In_z_ const char* pSplit,
                           _Out_ w_array* pResults);

/**
 * convert to base64
 * @param pDestinationBuffer destination buffer
 * @param pSourceBuffer source buffer
 * @param pSourceBufferLenght lenght of source buffer
 * @param pEncodeMode typed of encode algorithm
 * @return size of destination buffer
*/
W_SYSTEM_EXPORT
size_t w_io_to_base_64(_Inout_z_ char** pDestinationBuffer,
                       _In_z_ char* pSourceBuffer,
                       _In_z_ size_t pSourceBufferLenght,
                       _In_ base_64_mode pEncodeMode);

/**
 * is file stream contains jpeg data
 * @param pFilePath path to the file
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_is_jpeg(_In_ const char* pFilePath);

/**
 * is file stream contains jpeg data
 * @param pFileStream a file stream
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_stream_is_jpeg(_In_ w_file_istream pFileStream);

/**
 * convert a jpeg stream to pixels format
 * @param pJpegStream a stream jpeg format
 * @param pPixelFormat type of pixel format
 * @param pWidth width of jpeg
 * @param pHeight height of jpeg
 * @param pSubSample subsample of jpeg
 * @param pColorSpace color of jpeg
 * @param pNumberOfPasses number of jpeg passes
 * @param pPixels array of pixels
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_pixels_from_jpeg_stream(_In_z_   const uint8_t* pJpegStream,
                                      _In_     size_t pJpegStreamLen,
                                      _In_     w_jpeg_pixel_format pPixelFormat,
                                      _Out_    int* pWidth,
                                      _Out_    int* pHeight,
                                      _Out_    int* pSubSample,
                                      _Out_    int* pColorSpace,
                                      _Out_    int* pNumberOfPasses,
                                      _Out_    uint8_t** pPixels);

/**
 * read pixels from jpeg file
 * @param pJpegFile a jpeg file path
 * @param pPixelFormat type of pixel format
 * @param pWidth width of jpeg
 * @param pHeight height of jpeg
 * @param pSubSample subsample of jpeg
 * @param pColorSpace color of jpeg
 * @param pNumberOfPasses number of jpeg passes
 * @param pPixels array of pixels
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_pixels_from_jpeg_file(_In_z_   const char* pJpegFile,
                                    _In_     size_t pJpegStreamLen,
                                    _In_     w_jpeg_pixel_format pPixelFormat,
                                    _Out_    int* pWidth,
                                    _Out_    int* pHeight,
                                    _Out_    int* pSubSample,
                                    _Out_    int* pColorSpace,
                                    _Out_    int* pNumberOfPasses,
                                    _Out_    uint8_t** pPixels);

/**
 * is file stream contains png data
 * @param pFileStream a file stream
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_stream_is_png(_In_ w_file_istream pFileStream);

/**
 * read the file and check whether contains png data
 * @param pFilePath a path to the file
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_file_is_png(_In_z_ const char* pFilePath);

/**
 * read pixels from png stream
 * @param pFileStream a file stream
 * @param pPixelFormat pixel format of png
 * @param pWidth width of png
 * @param pHeight height of png
 * @param pColorType color type
 * @param pBitDepth bit depth
 * @param pNumberOfPasses number of passes
 * @param pPixels array of pixels
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_pixels_from_png_stream(_In_   w_file_istream pFileStream,
                                     _In_   w_png_pixel_format pPixelFormat,
                                     _Out_  int* pWidth,
                                     _Out_  int* pHeight,
                                     _Out_  uint8_t* pColorType,
                                     _Out_  uint8_t* pBitDepth,
                                     _Out_  int* pNumberOfPasses,
                                     _Out_  uint8_t** pPixels);

/**
 * read pixels from png file
 * @param pFilePath a file stream
 * @param pPixelFormat pixel format of png
 * @param pWidth width of png
 * @param pHeight height of png
 * @param pColorType color type
 * @param pBitDepth bit depth
 * @param pNumberOfPasses number of passes
 * @param pPixels array of pixels
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT w_io_pixels_from_png_file(_In_   const char* pFilePath,
                                   _In_   w_png_pixel_format pPixelFormat,
                                   _Out_  int* pWidth,
                                   _Out_  int* pHeight,
                                   _Out_  uint8_t* pColorType,
                                   _Out_  uint8_t* pBitDepth,
                                   _Out_  int* pNumberOfPasses,
                                   _Out_  uint8_t** pPixels);

#ifdef __cplusplus
}
#endif

