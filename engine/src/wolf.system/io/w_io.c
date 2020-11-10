#include "w_io.h"
#include <apr-1/apr_general.h>
#include <apr-1/apr_strings.h>
#include <apr-1/apr_file_info.h>
#include <apr-1/apr_file_io.h>
#include "log/w_log.h"

#if !defined(W_PLATFORM_ANDROID) && !defined(W_PLATFORM_IOS)

#include <turbojpeg.h>
#include <png.h>

#include "base64/fastavx512bwbase64.h"
#include "base64/fastavxbase64.h"
#include "base64/klompavxbase64.h"
#include "base64/quicktimebase64.h"

#endif

#ifndef W_PLATFORM_IOS
#include "base64/chromiumbase64.h"
#include "base64/scalarbase64.h"
#endif

#ifdef W_PLATFORM_UNIX
#include "base64/linuxbase64.h"
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#elif defined(W_PLATFORM_WIN)
#include <shlwapi.h>
#include <io.h>
#endif

#ifdef W_PLATFORM_OSX
#include <Foundation/NSString.h>
#include <Foundation/NSBundle.h>
#endif

#define PNG_BYTES_TO_CHECK  4
#define PNG_PAGING_SIZE     8

struct png_context {
    void* data;
    int len;
    int pos;
};

w_file w_io_file_create(
    _Inout_ w_mem_pool pMemPool,
    _In_z_  const char* pPath,
    _In_z_  const char* pContent,
    _In_    bool pBinaryMode,
    _In_    bool pBufferedMode,
    _In_    bool pNoneBlockMode,
    _In_    bool pMultiThreadedMode,
    _In_    bool pOpenAppendMode,
    _In_    bool pIsLargFile,
    _In_    bool pErrorIfFileExists)
{
    const char* _trace_info = "w_io_file_create";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing fast extend memory pool. trace info: %s", _trace_info);
        return NULL;
    }

    apr_status_t _ret = APR_SUCCESS;
    apr_file_t* _file = NULL;
    apr_size_t  _buffer_len = 0;
    apr_int32_t _flags = 0;

    _flags =
        APR_FOPEN_CREATE | // create file if not there
        APR_FOPEN_WRITE;   // open for writing

    if (pBinaryMode)
    {
        _flags |= APR_FOPEN_BINARY; // binary mode (ignored on UNIX)
    }
    if (pBufferedMode)
    {
        _flags |= APR_FOPEN_BUFFERED; // buffered mode
    }
    if (pNoneBlockMode)
    {
        _flags |= APR_FOPEN_NONBLOCK; // none blocked mode
    }
    if (pMultiThreadedMode)
    {
        _flags |= APR_FOPEN_XTHREAD; // allow multiple threads to use file
    }
    if (pOpenAppendMode)
    {
        _flags |= APR_FOPEN_APPEND; // binary mode (ignored on UNIX)
    }
    if (pIsLargFile)
    {
        _flags |= APR_FOPEN_LARGEFILE; // binary mode (ignored on UNIX)
    }
    if (pErrorIfFileExists)
    {
        _flags |= APR_FOPEN_EXCL; // binary mode (ignored on UNIX)
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        _ret = apr_file_open(
            &_file, // new file handle
            pPath, // file name
            _flags,
            APR_OS_DEFAULT | 0, // permissions
            _pool // memory pool to use
        );
        if (_ret == W_SUCCESS)
        {
            _buffer_len = strlen(pContent);
            _ret = apr_file_write(_file, pContent, &_buffer_len);
        }
    }

    return _file;
}

W_RESULT w_io_file_save(
    _In_z_  const char* pPath,
    _In_z_  const char* pContent,
    _In_    bool pBinaryMode,
    _In_    bool pBufferedMode,
    _In_    bool pNoneBlockMode,
    _In_    bool pMultiThreadedMode,
    _In_    bool pOpenAppendMode,
    _In_    bool pIsLargFile,
    _In_    bool pErrorIfFileExists)
{
    const char* _trace_info = "w_io_file_save";

    w_mem_pool _mem_pool = NULL;
    w_mem_pool_init(&_mem_pool);
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "could not create fast extend memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_status_t _ret = APR_SUCCESS;
    apr_file_t* _file = NULL;
    apr_size_t  _buffer_len = 0;
    apr_int32_t _flags = 0;

    _flags = APR_FOPEN_CREATE | // create file if not there
        APR_FOPEN_WRITE;   // open for writing

    if (pBinaryMode)
    {
        _flags |= APR_FOPEN_BINARY; // binary mode (ignored on UNIX)
    }
    if (pBufferedMode)
    {
        _flags |= APR_FOPEN_BUFFERED; // buffered mode
    }
    if (pNoneBlockMode)
    {
        _flags |= APR_FOPEN_NONBLOCK; // none blocked mode
    }
    if (pMultiThreadedMode)
    {
        _flags |= APR_FOPEN_XTHREAD; // allow multiple threads to use file
    }
    if (pOpenAppendMode)
    {
        _flags |= APR_FOPEN_APPEND; // binary mode (ignored on UNIX)
    }
    if (pIsLargFile)
    {
        _flags |= APR_FOPEN_LARGEFILE; // binary mode (ignored on UNIX)
    }
    if (pErrorIfFileExists)
    {
        _flags |= APR_FOPEN_EXCL; // binary mode (ignored on UNIX)
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(_mem_pool);
    if (!_pool)
    {
        return W_FAILURE;
    }

    _ret = apr_file_open(
        &_file, // new file handle
        pPath, // file name
        _flags,
        APR_OS_DEFAULT | 0, // permissions
        _pool // memory pool to use
    );
    if (_ret != APR_SUCCESS)
    {
        goto out;
    }
    _buffer_len = strlen(pContent);
    _ret = apr_file_write(_file, pContent, &_buffer_len);

out:
    if (_file)
    {
        apr_file_close(_file);
    }
    //terminate memory pool
    w_mem_pool_fini(&_mem_pool);

    return _ret;
}

W_RESULT w_io_file_check_is_file(_In_z_ const char* pPath)
{
    const int _F_OK = 0;
    if (access(pPath, _F_OK) != -1)
    {
        return W_SUCCESS;
    }
    return W_FAILURE;

    /*const char* _trace_info = "w_io_file_check_is_file";

    w_mem_pool _mem_pool = NULL;
    w_mem_pool_init(&_mem_pool);
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "could not create fast extend memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_status_t _ret = W_FAILURE;
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(_mem_pool);
    if (_pool)
    {
        apr_file_t* _file = NULL;
        _ret = apr_file_open(
            &_file,
            pPath,
            APR_READ,
            APR_OS_DEFAULT,
            _pool);
        if (_ret == APR_SUCCESS)
        {
            apr_file_close(_file);
        }
    }
    w_mem_pool_fini(&_mem_pool);*/

    // return _ret;
}

w_file_info w_io_file_get_info_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_file_get_info_from_path";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing fast extend memory pool. trace info: %s", _trace_info);
        return NULL;
    }

    if (pMemPool)
    {
        apr_finfo_t* _info = (apr_finfo_t*)w_malloc(
            pMemPool,
            sizeof(apr_finfo_t));
        if (!_info)
        {
            W_ASSERT_P(false,
                "could not allocate memory for apr_finfo_t for file %s. trace info: %s",
                pPath,
                _trace_info);
            return NULL;
        }
        apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
        if (_pool && (apr_stat(_info, pPath, APR_FINFO_NORM, _pool) == APR_SUCCESS))
        {
            return _info;
        }
    }
    W_ASSERT_P(false, "error just happened. trace info: %s", _trace_info);
    return NULL;
}

w_file_info w_io_file_get_info(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_get_info";
    if (!pMemPool || !pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    apr_finfo_t* _info = (apr_finfo_t*)w_malloc(
        pMemPool,
        sizeof(apr_finfo_t));
    if (_info && apr_file_info_get(_info, APR_FINFO_NORM, pFile) == APR_SUCCESS)
    {
        return _info;
    }

    W_ASSERT_P(false, "error just happened. trace info: %s", _trace_info);
    return NULL;
}

const char* w_io_file_get_extension_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pFilePath)
{
    const char* _trace_info = "w_io_file_get_extension_from_path";
    if (!pMemPool || !pFilePath)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }
    const char* _dot = strrchr(pFilePath, '.');
    if (!_dot || _dot == pFilePath) return "";
    return _dot + 1;
}

const char* w_io_file_get_extension(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_get_extension";
    if (!pMemPool || !pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    const char* _ret = NULL;
    apr_finfo_t* _info = w_io_file_get_info(pMemPool, pFile);
    if (_info)
    {
        _ret = w_io_file_get_extension_from_path(pMemPool, _info->fname);
    }
    return _ret;
}

const char* w_io_file_get_name_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_file_get_name_from_path";
    if (!pMemPool || !pPath)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    // traverse from right
    size_t _len = strlen(pPath);
    size_t _index = -1;
    for (size_t i = _len - 1; i > 0; i--)
    {
        if (pPath[i] == '.')
        {
            _index = i;
            break;
        }
    }

    if (_index == -1) return NULL;
    _index++;

    char* _dst_str = NULL;


    _dst_str = (char*)w_malloc(pMemPool, _index);
    apr_cpystrn(_dst_str, pPath, _index);
    _dst_str[_index] = '\0';


    return _dst_str;
}

const char* w_io_file_get_name(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_get_name";
    if (!pMemPool || !pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    const char* _ret = NULL;
    apr_finfo_t* _info = w_io_file_get_info(pMemPool, pFile);
    if (_info)
    {
        _ret = w_io_file_get_name_from_path(pMemPool, _info->fname);
    }
    return _ret;
}

const char* w_io_file_get_basename_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pFilePath)
{
    const char* _trace_info = "w_io_file_get_basename_from_path";
    if (!pMemPool || !pFilePath)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    size_t _len = strlen(pFilePath);
    char* _s = (char*)w_malloc(pMemPool, _len);
    strcpy(_s, pFilePath);

#ifdef W_PLATFORM_WIN
    PathStripPathA(_s);
#else
    _s = basename(_s);
#endif
    return _s;
}

const char* w_io_file_get_basename(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_get_basename";
    if (!pMemPool || !pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    const char* _ret = NULL;

    apr_finfo_t* _info = w_io_file_get_info(pMemPool, pFile);
    if (_info)
    {
        _ret = w_io_file_get_basename_from_path(
            pMemPool,
            _info->fname);
    }

    return _ret;
}

const char* w_io_file_get_basename_without_extension_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_file_get_basename_without_extension_from_path";
    const char* _ret = NULL;
    if (!pPath || !pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return _ret;
    }

    const char* _basename = w_io_file_get_basename_from_path(
        pMemPool,
        pPath);

    if (_basename)
    {
        _ret = w_io_file_get_name_from_path(pMemPool, _basename);
    }

    return _ret;
}

const char* w_io_file_get_basename_without_extension(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_get_basename_without_extension";
    if (!pMemPool || !pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    const char* _ret = NULL;

    apr_finfo_t* _info = w_io_file_get_info(pMemPool, pFile);
    if (_info)
    {
        const char* _name = strdup(_info->fname);
        _ret = w_io_file_get_basename_without_extension_from_path(pMemPool, _name);
    }

    return _ret;
}

w_file_istream w_io_file_read_full_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    return w_io_file_read_nbytes_from_path(pMemPool, pPath, 0);
}

w_file_istream w_io_file_read_nbytes_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath,
    _In_ size_t pNBytes)
{
    const char* _trace_info = "w_io_file_read_nbytes_from_path";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing fast extend memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    if (!pPath)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    w_file_istream _istream = NULL;
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        _istream = (w_file_istream_t*)w_malloc(pMemPool, sizeof(w_file_istream_t));
        if (!_istream)
        {
            W_ASSERT_P(false,
                "could not allocate memory for w_file_input_stream for file: %s. trace info: %s",
                pPath,
                _trace_info);
            return NULL;
        }
        else
        {
            apr_file_t* _file = NULL;
            apr_status_t _ret = apr_file_open(
                &_file,
                pPath,
                APR_READ,
                APR_OS_DEFAULT,
                _pool);
            if (_ret != APR_SUCCESS)
            {
                return NULL;
            }

            if (pNBytes)
            {
                _istream->size = pNBytes;
            }
            else
            {
                //read it all
                apr_finfo_t _finfo;
                _ret = apr_file_info_get(&_finfo, APR_FINFO_NORM, _file);
                if (_ret)
                {
                    W_ASSERT_P(false,
                        "could not get file info for file: %s. trace info: %s",
                        pPath,
                        _trace_info);
                    apr_file_close(_file);
                    return NULL;
                }
                _istream->size = _finfo.size;
            }
            _istream->buffer = w_malloc(pMemPool, _istream->size);
            _istream->bytes_read = _istream->size;
            apr_file_read(_file, _istream->buffer, &_istream->bytes_read);
            apr_file_close(_file);
        }
    }

    return _istream;
}

w_file_istream	w_io_file_read_nbytes(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile,
    _In_ size_t pNBytes)
{
    const char* _trace_info = "w_io_file_read_nbytes";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing fast extend memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    if (!pFile)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    w_file_istream _istream = (w_file_istream_t*)w_malloc(
        pMemPool,
        sizeof(w_file_istream_t));
    if (!_istream)
    {
        W_ASSERT_P(false,
            "could not allocate memory for w_file_input_stream. trace info: %s",
            _trace_info);
        return NULL;
    }

    if (pNBytes)
    {
        _istream->size = pNBytes;
    }
    //else
    //{
    //    //read it all
    //    apr_finfo_t _finfo;
    //    apr_status_t _ret = apr_file_info_get(&_finfo, APR_FINFO_NORM, pFile);
    //    if (_ret)
    //    {
    //        W_ASSERT_P(false,
    //            "could not get file info for file: %s. trace info: %s",
    //            _finfo.fname,
    //            _trace_info);
    //        return NULL;
    //    }
    //    _istream->size = _finfo.size;
    //}
    _istream->buffer = w_malloc(pMemPool, _istream->size);
    _istream->bytes_read = _istream->size;
    apr_file_read(pFile, _istream->buffer, &_istream->bytes_read);
    apr_file_close(pFile);

    return _istream;
}

w_file_istream w_io_file_read_full(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    return w_io_file_read_nbytes(pMemPool, pFile, 0);
}

W_RESULT w_io_file_delete_from_path(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_file_delete_from_path";
    if (!pPath || !pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_file_remove(pPath, _pool);
    }
    return W_FAILURE;
}

W_RESULT	w_io_file_delete(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_file pFile)
{
    const char* _trace_info = "w_io_file_delete";
    if (!pFile || !pMemPool)
    {
        W_ASSERT_P(false, "bad args file. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_status_t _ret = W_FAILURE;
    apr_finfo_t* _file = w_io_file_get_info(pMemPool, pFile);
    if (_file)
    {
        apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
        if (_pool)
        {
            _ret = apr_file_remove(_file->fname, _pool);
        }
    }
    return _ret;
}

W_RESULT w_io_dir_get_current(_Inout_ w_mem_pool pMemPool, _Inout_ char** pDir)
{
    if (!pMemPool || !pDir)
    {
        return APR_BADARG;
    }

    char* _dir = NULL;

    w_mem_pool _pool = NULL;
    w_mem_pool_init(&_pool);
    if (!_pool)
    {
        return W_FAILURE;
    }

    char* _tmp = (char*)w_malloc(_pool, sizeof(W_MAX_BUFFER_SIZE));
    if (_tmp)
    {
#ifdef W_PLATFORM_WIN
        GetCurrentDirectoryA(W_MAX_BUFFER_SIZE, _tmp);
#else
        if (getcwd(_tmp, W_MAX_BUFFER_SIZE) == NULL)
        {
            return W_FAILURE;
        }
#endif
        size_t _len = strlen(_tmp);
        if (_len)
        {
            _dir = (char*)w_malloc(pMemPool, _len + 1);
            memcpy(_dir, _tmp, _len);
            _dir[_len] = '\0';
            *pDir = _dir;
        }
    }

    w_mem_pool_fini(&_pool);

    return W_SUCCESS;
}

W_RESULT w_io_dir_get_current_exe(_Inout_ w_mem_pool pMemPool, _Inout_ char** pDir)
{
    if (!pDir || !pMemPool)
    {
        return APR_BADARG;
    }

    char* _dir = NULL;

    w_mem_pool _pool = NULL;
    w_mem_pool_init(&_pool);
    if (!_pool)
    {
        return W_FAILURE;
    }

    char* _tmp = (char*)w_malloc(_pool, sizeof(W_MAX_BUFFER_SIZE));
    if (_tmp)
    {
#ifdef W_PLATFORM_WIN
        GetModuleFileNameA(NULL, _tmp, W_MAX_BUFFER_SIZE);
        size_t idx = strlen(_tmp);// wcslen(_tmp);

        while (idx >= 0)
        {
            if (_tmp[idx] == '/' || _tmp[idx] == '\\')
            {
                _tmp[idx] = 0;
                break;
            }
            idx--;
        }
#elif defined W_PLATFORM_OSX
        int size = W_MAX_BUFFER_SIZE;

        NSString* p = [[NSBundle mainBundle]executablePath];
        if (p == NULL)
        {
            return W_FAILURE;
        }

        _tmp = (char*)[p UTF8String];

        int idx = strlen(_tmp);
        while (idx >= 0)
        {
            if (_tmp[idx] == '/' || _tmp[idx] == '\\')
            {
                _tmp[idx] = 0;
                break;
            }
            idx--;
        }
#else
        if (getcwd(_tmp, W_MAX_BUFFER_SIZE) == NULL)
        {
            return W_FAILURE;
        }
#endif
        size_t _len = strlen(_tmp);
        if (_len)
        {
            _dir = (char*)w_malloc(pMemPool, _len + 1);
            memcpy(_dir, _tmp, _len);
            _dir[_len] = '\0';
            *pDir = _dir;
        }
    }

    w_mem_pool_fini(&_pool);

    return W_SUCCESS;
}


W_RESULT	w_io_dir_check_is_dir(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_dir_check_is_dir";
    if (!pPath || !pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }

    W_RESULT _ret = W_FAILURE;
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        //open a directory
        apr_dir_t* _dir = NULL;
        _ret = apr_dir_open(&_dir, pPath, _pool);
        if (_dir && _ret == APR_SUCCESS)
        {
            apr_dir_close(_dir);
        }
    }
    return _ret;
}

const char* w_io_dir_get_parent(
    _Inout_ w_mem_pool  pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_dir_check_is_dir";
    if (!pPath || !pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

#ifdef W_PLATFORM_UNIX
    char* _s = strdup(pPath);
    return dirname(_s);
#else
    apr_size_t _size = 0;
    for (int i = (int)strlen(pPath) - 1; i >= 0; i--)
    {
        if (pPath[i] == '\\' || pPath[i] == '/')
        {
            _size = (apr_size_t)i;
            break;
        }
    }
    //copy strings
    if (_size == 0) return "";

    _size++;
    char* _dst = w_malloc(pMemPool, _size + 1);//size + '\0'
    apr_cpystrn(_dst, pPath, _size);
    _dst[_size] = '\0';
    return _dst;
#endif
}

W_RESULT	w_io_dir_create(
    _Inout_ w_mem_pool  pMemPool,
    _In_z_ const char* pPath)
{
    const char* _trace_info = "w_io_dir_create";
    if (!pPath || !pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    W_RESULT _ret = W_FAILURE;
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        //create a directory
        _ret = apr_dir_make_recursive(pPath, APR_OS_DEFAULT, _pool);
    }

    return _ret;
}

W_RESULT	w_io_utf8_to_ucs2(
    _In_ char* pIn,
    _In_ size_t* pInLen,
    _Out_ uint16_t* pOut,
    _Out_ size_t* pOutLen)
{
    apr_int64_t newch, mask;
    apr_size_t expect, eating;
    int ch;

    while (*pInLen && *pOutLen)
    {
        ch = (unsigned char)(*pIn++);
        if (!(ch & 0200)) {
            /* US-ASCII-7 plain text
             */
            --* pInLen;
            --* pOutLen;
            *(pOut++) = ch;
        }
        else
        {
            if ((ch & 0300) != 0300) {
                /* Multibyte Continuation is out of place
                 */
                return APR_BADARG;
            }
            else
            {
                /* Multibyte Sequence Lead Character
                 *
                 * Compute the expected bytes while adjusting
                 * or lead byte and leading zeros mask.
                 */
                mask = 0340;
                expect = 1;
                while ((ch & mask) == mask) {
                    mask |= mask >> 1;
                    if (++expect > 3) /* (truly 5 for ucs-4) */
                        return APR_BADARG;
                }
                newch = ch & ~mask;
                eating = expect + 1;
                if (*pInLen <= expect)
                    return APR_BADARG;
                /* Reject values of excessive leading 0 bits
                 * utf-8 _demands_ the shortest possible byte length
                 */
                if (expect == 1) {
                    if (!(newch & 0036))
                        return APR_BADARG;
                }
                else {
                    /* Reject values of excessive leading 0 bits
                     */
                    if (!newch && !((unsigned char)*pIn & 0077 & (mask << 1)))
                        return APR_BADARG;
                    if (expect == 2) {
                        /* Reject values D800-DFFF when not utf16 encoded
                         * (may not be an appropriate restriction for ucs-4)
                         */
                        if (newch == 0015 && ((unsigned char)*pIn & 0040))
                            return APR_BADARG;
                    }
                    else if (expect == 3) {
                        /* Short circuit values > 110000
                         */
                        if (newch > 4)
                            return APR_BADARG;
                        if (newch == 4 && ((unsigned char)*pIn & 0060))
                            return APR_BADARG;
                    }
                }
                /* Where the boolean (expect > 2) is true, we will need
                 * an extra word for the output.
                 */
                if (*pOutLen < (apr_size_t)(expect > 2) + 1)
                    break; /* buffer full */
                while (expect--)
                {
                    /* Multibyte Continuation must be legal */
                    if (((ch = (unsigned char)*(pIn++)) & 0300) != 0200)
                        return APR_BADARG;
                    newch <<= 6;
                    newch |= (ch & 0077);
                }
                *pInLen -= eating;
                /* newch is now a true ucs-4 character
                 *
                 * now we need to fold to ucs-2
                 */
                if (newch < 0x10000)
                {
                    --* pOutLen;
                    *(pOut++) = (apr_uint16_t)newch;
                }
                else
                {
                    *pOutLen -= 2;
                    newch -= 0x10000;
                    *(pOut++) = (apr_uint16_t)(0xD800 | (newch >> 10));
                    *(pOut++) = (apr_uint16_t)(0xDC00 | (newch & 0x03FF));
                }
            }
        }
    }
    /* Buffer full 'errors' aren't errors, the client must inspect both
     * the inbytes and outwords values
     */
    return W_SUCCESS;
}

W_RESULT w_io_ucs2_to_utf8(
    _In_ uint16_t* pIn,
    _In_ size_t* pInLen,
    _Out_ char* pOut,
    _Out_ size_t* pOutLen)
{
    apr_int64_t newch, require;
    apr_size_t need;
    char* invout;
    int ch;

    while (*pInLen && *pOutLen)
    {
        ch = (unsigned short)(*pIn++);
        if (ch < 0x80)
        {
            --* pInLen;
            --* pOutLen;
            *(pOut++) = (unsigned char)ch;
        }
        else
        {
            if ((ch & 0xFC00) == 0xDC00) {
                /* Invalid Leading ucs-2 Multiword Continuation Character
                 */
                return APR_BADARG;
            }
            if ((ch & 0xFC00) == 0xD800) {
                /* Leading ucs-2 Multiword Character
                 */
                if (*pInLen < 2) {
                    /* Missing ucs-2 Multiword Continuation Character
                     */
                    return APR_BADARG;
                }
                if (((unsigned short)(*pIn) & 0xFC00) != 0xDC00) {
                    /* Invalid ucs-2 Multiword Continuation Character
                     */
                    return APR_BADARG;
                }
                newch = (ch & 0x03FF) << 10 | ((unsigned short)(*pIn++) & 0x03FF);
                newch += 0x10000;
            }
            else {
                /* ucs-2 Single Word Character
                 */
                newch = ch;
            }
            /* Determine the absolute minimum utf-8 bytes required
             */
            require = newch >> 11;
            need = 1;
            while (require)
                (void)(require >>= 5), ++need;
            if (need >= *pOutLen)
                break; /* Insufficient buffer */
            *pInLen -= (need > 2) + 1;
            *pOutLen -= need + 1;
            /* Compute the utf-8 characters in last to first order,
             * calculating the lead character length bits along the way.
             */
            ch = 0200;
            pOut += need + 1;
            invout = pOut;
            while (need--) {
                ch |= ch >> 1;
                *(--invout) = (unsigned char)(0200 | (newch & 0077));
                newch >>= 6;
            }
            /* Compute the lead utf-8 character and move the dest offset
             */
            *(--invout) = (unsigned char)(ch | newch);
        }
    }
    /* Buffer full 'errors' aren't errors, the client must inspect both
     * the inwords and outbytes values
     */
    return W_SUCCESS;
}

long w_io_to_hex(_In_z_ const char* pHexStr)
{
    return strtol(pHexStr, NULL, 16);
}

W_RESULT w_io_wchar_ptr_to_char_ptr(
    _Inout_ w_mem_pool pMemPool,
    _In_	wchar_t* pIn,
    _In_	size_t pInLen,
    _Out_	char** pOut)
{
    if (!pIn || !pOut || pInLen == 0 || !pMemPool)
    {
        *pOut = NULL;
        return APR_BADARG;
    }

    size_t _wide_char_mem_size = (pInLen * 2) + 1; // +1 for null terminator
    (*pOut) = (char*)w_malloc(pMemPool, _wide_char_mem_size);
    if ((*pOut))
    {
        (*pOut)[_wide_char_mem_size - 1] = '\0';
#ifdef W_PLATFORM_WIN
        size_t _chars_onverted = 0;
        return (wcstombs_s(
            &_chars_onverted,
            *pOut,
            _wide_char_mem_size,
            pIn,
            pInLen) == 0) ? W_SUCCESS : W_FAILURE;
#else
        return (wcstombs(
            *pOut,
            pIn,
            pInLen) == 0) ? W_SUCCESS : W_FAILURE;
#endif
    }
    return W_FAILURE;
}

W_RESULT w_io_string_has_start_with(_In_z_ const char* pString, _In_z_ const char* pStartWith)
{
    return strncmp(pStartWith, pString, strlen(pStartWith));
}

W_RESULT w_io_string_has_end_with(_In_z_ const char* pString, _In_z_ const char* pEndWith)
{
    size_t _str_size = strlen(pString);
    size_t _str_end_size = strlen(pEndWith);

    if (_str_size >= _str_end_size)
    {
        return strncmp(pString + _str_size - _str_end_size, pEndWith, _str_end_size) == 0;
    }
    return W_FAILURE;
}

W_RESULT w_io_wstring_has_start_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pStartWith)
{
    return wcsncmp(pStartWith, pString, wcslen(pStartWith));
}

W_RESULT w_io_wstring_has_end_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pEndWith)
{
    size_t _str_size = wcslen(pString);
    size_t _str_end_size = wcslen(pEndWith);

    if (_str_size >= _str_end_size)
    {
        return wcsncmp(pString + _str_size - _str_end_size, pEndWith, _str_end_size) == 0;
    }
    return W_FAILURE;
}

W_RESULT w_io_string_split(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pString,
    _In_z_ const char* pSplit,
    _Out_ w_array* pResults)
{
    const char* _trace_info = "w_io_string_split";
    if (!pMemPool || !pString || !pSplit)
    {
        W_ASSERT_P(false, "missing parameters. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    *pResults = w_array_init(pMemPool, 32, sizeof(const char*));
    if (!*pResults)
    {
        W_ASSERT_P(false, "could not create array. trace info: %s", _trace_info);
        return W_FAILURE;
    }

    char* _splits = strtok((char*)pString, pSplit);
    // loop through the string and extract all other splits
    while (_splits != NULL)
    {
        w_array_append(*pResults, _splits);
        _splits = strtok(NULL, pSplit);
    }

    return W_SUCCESS;
}

size_t w_io_to_base_64(_Inout_z_ char** pDestinationBuffer,
    _In_z_ char* pSourceBuffer,
    _In_z_ size_t pSourceBufferLenght,
    _In_ base_64_mode pEncodeMode)
{
    size_t _encoded_size = 0;
    switch (pEncodeMode)
    {
#ifndef W_PLATFORM_IOS
    case chromium:
        _encoded_size = chromium_base64_encode(
            *pDestinationBuffer,
            pSourceBuffer,
            pSourceBufferLenght);
        break;
    case scalar:
        scalar_base64_encode(
            pSourceBuffer,
            pSourceBufferLenght,
            *pDestinationBuffer,
            &_encoded_size);
        break;
#endif
#if !defined(W_PLATFORM_ANDROID) && !defined(W_PLATFORM_IOS)

    case klomp_avx:
        klomp_avx2_base64_encode(
            pSourceBuffer,
            pSourceBufferLenght,
            *pDestinationBuffer,
            &_encoded_size);
        break;
    case fast_avx:
        _encoded_size = fast_avx2_base64_encode(
            *pDestinationBuffer,
            pSourceBuffer,
            pSourceBufferLenght);
        break;
    case fast_avx512:
#if USE_AVX512 != 0 && ((defined(_MSC_VER) && _MSC_VER >= 1911) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1600) || (defined(__clang__) && __clang_major__ >= 4) || (defined(__GNUC__) && __GNUC__ >= 5))
        _encoded_size = fast_avx512bw_base64_encode(
            pDestinationBuffer,
            pSourceBuffer,
            pSourceBufferLenght);
#endif
        break;
    case quick_time:
        _encoded_size = (size_t)(quicktime_base64_encode(
            *pDestinationBuffer,
            pSourceBuffer,
            (int)pSourceBufferLenght));
        break;

#endif
    }
    return _encoded_size;
}

#if !defined(W_PLATFORM_ANDROID) && !defined(W_PLATFORM_IOS)

W_RESULT w_io_file_is_jpeg(_In_ const char* pFilePath)
{
    W_RESULT _ret = W_FAILURE;

    w_mem_pool _pool = NULL;
    if (w_mem_pool_init(&_pool) == W_SUCCESS)
    {
        w_file_istream _istream = w_io_file_read_full_from_path(_pool, pFilePath);
        _ret = w_io_stream_is_jpeg(_istream);
        w_mem_pool_fini(&_pool);
    }
    return _ret;
}

W_RESULT w_io_stream_is_jpeg(_In_ w_file_istream pFileStream)
{
    tjhandle _tj_handle = tjInitDecompress();

    if (!_tj_handle)
    {
        return W_FAILURE;
    }

    int _width, _height, _sub_sample, _color_space;
    W_RESULT _rt = tjDecompressHeader3(_tj_handle,
        pFileStream->buffer,
        (int)pFileStream->bytes_read,
        &_width,
        &_height,
        &_sub_sample,
        &_color_space);
    tjDestroy(_tj_handle);

    return _rt;
}

W_RESULT w_io_pixels_from_jpeg_stream(
    _Inout_  w_mem_pool pMemPool,
    _In_z_   const uint8_t* pJpegStream,
    _In_     size_t pJpegStreamLen,
    _In_     w_jpeg_pixel_format pPixelFormat,
    _Out_    int* pWidth,
    _Out_    int* pHeight,
    _Out_    int* pSubSample,
    _Out_    int* pColorSpace,
    _Out_    int* pNumberOfPasses,
    _Out_    uint8_t** pPixels)
{
    *pWidth = 0;
    *pHeight = 0;
    *pSubSample = 0;
    *pColorSpace = 0;
    *pNumberOfPasses = 0;
    *pPixels = NULL;

    const char* _trace_info = "w_io_pixels_from_jpeg_stream";
    if (!pMemPool || !pJpegStream || !pJpegStreamLen)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }
    W_RESULT _rt;
    tjhandle _tj_handle = NULL;

    if (pPixelFormat == TJPF_UNKNOWN)
    {
        _rt = APR_BADARG;
        goto out;
    }

    _tj_handle = tjInitDecompress();
    if (!_tj_handle)
    {
        _rt = W_FAILURE;
        goto out;
    }

    if ((_rt = tjDecompressHeader3(_tj_handle,
        pJpegStream,
        (int)pJpegStreamLen,
        pWidth,
        pHeight,
        pSubSample,
        pColorSpace)))
    {
        _rt = tjGetErrorCode(_tj_handle);
        W_ASSERT_P(false,
            "tjDecompressHeader3 failed. error code %d . error message %s .trace info: w_io_pixels_from_jpeg_stream",
            _rt,
            tjGetErrorStr2(_tj_handle));
        goto out;
    }

    int _comp;
    switch (pPixelFormat)
    {
    default:
        _comp = 4;
    case TJPF_RGB:
    case TJPF_BGR:
        _comp = 3;
        break;
    }

    size_t _memory_size = (size_t)_comp * (size_t)(*pWidth) * (size_t)(*pHeight) * sizeof(uint8_t);
    *pPixels = (uint8_t*)w_malloc(pMemPool, _memory_size);
    _rt = tjDecompress2(
        _tj_handle,
        pJpegStream,
        (unsigned long)pJpegStreamLen,
        *pPixels,
        *pWidth,
        _comp * (*pWidth),
        *pHeight,
        pPixelFormat,
        0);
out:
    if (_tj_handle)
    {
        tjDestroy(_tj_handle);
    }
    return _rt;
}

W_RESULT w_io_pixels_from_jpeg_file(
    _Inout_  w_mem_pool pMemPool,
    _In_z_   const char* pJpegFile,
    _In_     w_jpeg_pixel_format pPixelFormat,
    _Out_    int* pWidth,
    _Out_    int* pHeight,
    _Out_    int* pSubSample,
    _Out_    int* pColorSpace,
    _Out_    int* pNumberOfPasses,
    _Out_    uint8_t** pPixels)
{
    *pWidth = 0;
    *pHeight = 0;
    *pSubSample = 0;
    *pColorSpace = 0;
    *pNumberOfPasses = 0;
    *pPixels = NULL;

    W_RESULT _ret = W_FAILURE;
    if (!pMemPool || !pJpegFile)
    {
        return _ret;
    }
    w_file_istream _istream = w_io_file_read_full_from_path(pMemPool, pJpegFile);
    if (_istream)
    {
        _ret = w_io_pixels_from_jpeg_stream(
            pMemPool,
            (const uint8_t*)_istream->buffer,
            _istream->bytes_read,
            pPixelFormat,
            pWidth,
            pHeight,
            pSubSample,
            pColorSpace,
            pNumberOfPasses,
            pPixels);
    }
    return _ret;
}

W_RESULT w_io_stream_is_png(_In_ w_file_istream pFileStream)
{
    if (!pFileStream)
    {
        return W_FAILURE;
    }
    uint8_t _buf[PNG_BYTES_TO_CHECK];
    memcpy(_buf, pFileStream->buffer, PNG_BYTES_TO_CHECK);
    //Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
    return png_sig_cmp(_buf, 0, PNG_BYTES_TO_CHECK);
}

W_RESULT w_io_file_is_png(_In_z_ const char* pFilePath)
{
    W_RESULT _ret = W_FAILURE;

    w_mem_pool _pool = NULL;
    if (w_mem_pool_init(&_pool) == W_SUCCESS)
    {
        w_file_istream _fs = w_io_file_read_nbytes_from_path(
            _pool,
            pFilePath,
            PNG_BYTES_TO_CHECK);
        if (_fs)
        {
            _ret = png_sig_cmp(_fs->buffer, 0, PNG_BYTES_TO_CHECK);
        }
        w_mem_pool_fini(&_pool);
    }
    return _ret;
}

static  void _png_user_read_data(
    png_structp pPngPtr,
    png_bytep pData,
    int pLength)
{
    struct png_context* context;

    context = png_get_io_ptr(pPngPtr);
    if (pLength > context->pos + context->len)
        pLength = context->len - context->pos;
    memcpy(pData, (char*)context->data + context->pos, pLength);
    context->pos += pLength;

    //cast istream
    //png_voidp _io = png_get_io_ptr(pPngPtr);
    //memcpy(pData, _io, pLength);
}

W_RESULT w_io_pixels_from_png_stream(
    _Inout_ w_mem_pool pMemPool,
    _In_   w_file_istream pFileStream,
    _In_   w_png_pixel_format pPixelFormat,
    _Out_  int* pWidth,
    _Out_  int* pHeight,
    _Out_  uint8_t* pColorType,
    _Out_  uint8_t* pBitDepth,
    _Out_  int* pNumberOfPasses,
    _Out_  uint8_t** pPixels)
{
    const char* _trace_info = "w_io_pixels_from_png_stream";

    if (!pMemPool)
    {
        W_ASSERT_P(false,
            "file stream does not contain png data. trace info: %s::png_sig_cmp",
            _trace_info);
        return APR_BADARG;
    }
    if (png_sig_cmp(pFileStream->buffer, 0, PNG_BYTES_TO_CHECK))
    {
        W_ASSERT_P(false,
            "file stream does not contain png data. trace info: %s::png_sig_cmp",
            _trace_info);
        return APR_BADARG;
    }

    //initialize stuff
    png_structp _png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING,
        NULL,
        NULL,
        NULL);
    if (!_png_ptr)
    {
        W_ASSERT_P(false,
            "could not create png pointer. trace info: %s::png_create_read_struct",
            _trace_info);
        return W_FAILURE;
    }

    png_infop _info_ptr = png_create_info_struct(_png_ptr);
    if (!_info_ptr)
    {
        W_ASSERT_P(false,
            "could not create info struct. trace info: %s::png_create_info_struct",
            _trace_info);
        return W_FAILURE;
    }

    if (setjmp(png_jmpbuf(_png_ptr)))
    {
        png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);

        W_ASSERT_P(false,
            "failed on setjmp. trace info: %s",
            _trace_info);
        return W_FAILURE;
    }

    struct png_context context;

    context.data = pFileStream->buffer;
    context.len = (int)pFileStream->bytes_read;
    context.pos = PNG_PAGING_SIZE;

    png_set_read_fn(
        _png_ptr,
        (png_voidp)(&context),
        (png_rw_ptr)(&_png_user_read_data));//png_init_io(_png_ptr, _file);
    //png_init_io(_png_ptr, )
    png_set_sig_bytes(_png_ptr, PNG_PAGING_SIZE);
    png_read_info(_png_ptr, _info_ptr);

    *pWidth = (int)png_get_image_width(_png_ptr, _info_ptr);
    *pHeight = (int)png_get_image_height(_png_ptr, _info_ptr);
    *pColorType = png_get_color_type(_png_ptr, _info_ptr);
    *pBitDepth = png_get_bit_depth(_png_ptr, _info_ptr);
    *pNumberOfPasses = png_set_interlace_handling(_png_ptr);

    //check bit depth
    if (*pBitDepth == 16)
    {
        png_set_strip_16(_png_ptr);
    }

    if (*pColorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(_png_ptr);
    }

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16 bit depth.
    if (*pColorType == PNG_COLOR_TYPE_GRAY && *pBitDepth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(_png_ptr);
    }

    if (png_get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(_png_ptr);
    }

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (*pColorType == PNG_COLOR_TYPE_RGB ||
        *pColorType == PNG_COLOR_TYPE_GRAY ||
        *pColorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_filler(_png_ptr, 0xFF, PNG_FILLER_AFTER);
    }

    if (*pColorType == PNG_COLOR_TYPE_GRAY || *pColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(_png_ptr);
    }

    png_read_update_info(_png_ptr, _info_ptr);

    //now data must be rgba
    size_t _comp = 4;
    if (pPixelFormat == RGB_PNG ||
        pPixelFormat == BGR_PNG)
    {
        _comp = 3;
    }

    //allocate memory
    (*pPixels) = (uint8_t*)w_malloc(
        pMemPool,
        _comp * (size_t)(*pWidth) * (size_t)(*pHeight) * sizeof(uint8_t));

    w_mem_pool _mem_pool = NULL;
    w_mem_pool_init(&_mem_pool);
    if (!_mem_pool)
    {
        return W_FAILURE;
    }

    size_t _bytes_per_row = png_get_rowbytes(_png_ptr, _info_ptr);
    uint8_t* _raw_data = (uint8_t*)w_malloc(_mem_pool, _bytes_per_row);

    //pixels counter
    uint32_t _k = 0;

    //read single row at a time and then convert it to desired pixel format
    switch (pPixelFormat)
    {
    case RGB_PNG:
        for (int i = 0; i < (*pHeight); ++i)
        {
            png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

            //const int _row_offset = i * (*pWidth);

            size_t _byte_index = 0;
            for (int j = 0; j < (*pWidth); ++j)
            {
                const uint8_t _r = _raw_data[_byte_index++];
                const uint8_t _g = _raw_data[_byte_index++];
                const uint8_t _b = _raw_data[_byte_index++];
                _byte_index++;//alpha ignored

                (*pPixels)[_k] = _r;
                (*pPixels)[_k + 1] = _g;
                (*pPixels)[_k + 2] = _b;

                _k += (uint32_t)_comp;
            }
        }
        break;
    case BGR_PNG:
        for (int i = 0; i < *pHeight; ++i)
        {
            png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

            //const auto _row_offset = i * pWidth;

            size_t _byte_index = 0;
            for (int j = 0; j < *pWidth; ++j)
            {
                const uint8_t _r = _raw_data[_byte_index++];
                const uint8_t _g = _raw_data[_byte_index++];
                const uint8_t _b = _raw_data[_byte_index++];
                _byte_index++;//alpha ignored

                (*pPixels)[_k] = _b;
                (*pPixels)[_k + 1] = _g;
                (*pPixels)[_k + 2] = _r;

                _k += (uint32_t)_comp;
            }
        }
        break;
    case RGBA_PNG:
        for (int i = 0; i < *pHeight; ++i)
        {
            png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

            //const auto _row_offset = i * (*pWidth);

            uint32_t _byte_index = 0;
            for (int j = 0; j < (*pWidth); ++j)
            {
                const uint8_t _r = _raw_data[_byte_index++];
                const uint8_t _g = _raw_data[_byte_index++];
                const uint8_t _b = _raw_data[_byte_index++];
                const uint8_t _a = _raw_data[_byte_index++];

                (*pPixels)[_k] = _r;
                (*pPixels)[_k + 1] = _g;
                (*pPixels)[_k + 2] = _b;
                (*pPixels)[_k + 3] = _a;

                _k += (uint32_t)_comp;
            }
        }
        break;
    case BGRA_PNG:
        for (int i = 0; i < *pHeight; ++i)
        {
            png_read_row(_png_ptr, (png_bytep)_raw_data, NULL);

            //const auto _row_offset = i * pWidth;

            uint32_t _byte_index = 0;
            for (int j = 0; j < *pWidth; ++j)
            {
                const uint32_t _r = _raw_data[_byte_index++];
                const uint32_t _g = _raw_data[_byte_index++];
                const uint32_t _b = _raw_data[_byte_index++];
                const uint32_t _a = _raw_data[_byte_index++];

                (*pPixels)[_k] = _b;
                (*pPixels)[_k + 1] = _g;
                (*pPixels)[_k + 2] = _r;
                (*pPixels)[_k + 3] = _a;

                _k += (uint32_t)_comp;
            }
        }
        break;
    };

    png_destroy_read_struct(&_png_ptr, &_info_ptr, (png_infopp)0);
    w_mem_pool_fini(&_mem_pool);// free _raw_data

    return W_SUCCESS;
}

W_RESULT w_io_pixels_from_png_file(
    _Inout_ w_mem_pool pMemPool,
    _In_   const char* pFilePath,
    _In_   w_png_pixel_format pPixelFormat,
    _Out_  int* pWidth,
    _Out_  int* pHeight,
    _Out_  uint8_t* pColorType,
    _Out_  uint8_t* pBitDepth,
    _Out_  int* pNumberOfPasses,
    _Out_  uint8_t** pPixels)
{
    W_RESULT _ret = W_FAILURE;

    const char* _trace_info = "w_io_pixels_from_png_file";

    if (!pMemPool || !pFilePath)
    {
        W_ASSERT_P(false,
            "bad args. trace info: %s::png_sig_cmp",
            _trace_info);
        return APR_BADARG;
    }

    *pWidth = 0;
    *pHeight = 0;
    *pColorType = 0;
    *pBitDepth = 0;
    *pNumberOfPasses = 0;
    *pPixels = NULL;

    w_file_istream _fs = w_io_file_read_full_from_path(pMemPool, pFilePath);
    if (_fs)
    {
        _ret = w_io_pixels_from_png_stream(
            pMemPool,
            _fs,
            pPixelFormat,
            pWidth,
            pHeight,
            pColorType,
            pBitDepth,
            pNumberOfPasses,
            pPixels);
    }
    return _ret;
}

#endif



//apr_dir_read(<#apr_finfo_t *finfo#>, <#apr_int32_t wanted#>, <#apr_dir_t *thedir#>)
//#if defined(__cpp_lib_filesystem) || defined(__cpp_lib_experimental_filesystem)
//    inline void get_files_folders_in_directoryW(_In_z_ const std::wstring & pDirectoryPath, _Inout_ std::vector<std::wstring> & pPaths)
//    {
//        pPaths.clear();
//
//        for (auto& _file_name : fs::directory_iterator(pDirectoryPath))
//        {
//            const auto _path = _file_name.path().wstring();
//            pPaths.push_back(get_file_nameW(_path));
//        }
//    }
//    inline void get_files_folders_in_directory(_In_z_ const std::string& pDirectoryPath, _Inout_ std::vector<std::string>& pPaths)
//    {
//        pPaths.clear();
//
//        for (auto& _file_name : fs::directory_iterator(pDirectoryPath))
//        {
//            const auto _path = _file_name.path().string();
//            pPaths.push_back(get_file_name(_path));
//        }
//    }
//#else
//    inline void get_files_folders_in_directoryW(_In_z_ const std::wstring& pDirectoryPath, _Inout_ std::vector<std::wstring>& pPaths)
//    {
//        pPaths.clear();
//
//        auto _path = wolf::system::convert::wstring_to_string(pDirectoryPath);
//        DIR* _dir;
//        struct dirent* _ent;
//        if ((_dir = opendir(_path.c_str())) != NULL)
//        {
//            while ((_ent = readdir(_dir)) != NULL)
//            {
//                pPaths.push_back(wolf::system::convert::string_to_wstring(_ent->d_name));
//            }
//            closedir(_dir);
//        }
//    }
//
//    inline void get_files_folders_in_directory(_In_z_ const std::string& pDirectoryPath, _Inout_ std::vector<std::string>& pPaths)
//    {
//        pPaths.clear();
//
//        DIR* _dir;
//        struct dirent* _ent;
//        if ((_dir = opendir(pDirectoryPath.c_str())) != NULL)
//        {
//            while ((_ent = readdir(_dir)) != NULL)
//            {
//                pPaths.push_back(_ent->d_name);
//            }
//            closedir(_dir);
//        }
//    }
//#endif
//    /*
//        Read text file from root path
//        pState indicates to state of file and the permission status, the first integer value means :
//             1 means everything is ok
//            -1 means the file could not be found,
//            -2 means file is exist but it might be corrupted
//            -3 means file is empty
//            -4 means could not open file
//    */
//    inline const char* read_text_file(_In_z_ const char* pPath, _Out_ int& pState)
//    {
//        std::ifstream _file(pPath, std::ios::ate | std::ios::in);
//        if (!_file)
//        {
//            //file is not exist
//            pState = -1;
//            return nullptr;
//        }
//        if (_file.bad())
//        {
//            //file exists but it might be corrupted
//            pState = -2;
//            return nullptr;
//        }
//
//        auto _file_length = static_cast<unsigned long>(_file.tellg());
//        if (_file_length == 0)
//        {
//            //file is empty
//            pState = -3;
//            return "";
//        }
//
//        if (!_file.is_open())
//        {
//            //could not open file
//            pState = -4;
//            return nullptr;
//        }
//
//        auto _source = new char[_file_length + 1];
//        _source[_file_length] = '\0';
//        _file.seekg(0, std::ios::beg);
//
//        unsigned int _i = 0;
//        while (_file.good())
//        {
//            _source[_i] = _file.get();
//            if (!_file.eof())
//            {
//                _i++;
//            }
//        }
//        _source[_i] = '\0';
//
//        //close the file
//        _file.close();
//
//        pState = 1;
//        return _source;
//    }
//
//#ifdef __WIN32
//    /*
//        Read text file from root path
//        pState indicates to state of file and the permission status, the first integer value means :
//             1 means everything is ok
//            -1 means the file could not be found,
//            -2 means file is exist but it might be corrupted
//            -3 means file is empty
//            -4 means could not open file
//    */
//    inline const wchar_t* read_text_fileW(_In_z_ const wchar_t* pPath, _Out_ int& pState)
//    {
//        std::ifstream _file(pPath, std::ios::ate | std::ios::in);
//        if (!_file)
//        {
//            //file is not exist
//            pState = -1;
//            return nullptr;
//        }
//
//        if (_file.bad())
//        {
//            //file exists but it might be corrupted
//            pState = -2;
//            return nullptr;
//        }
//
//        auto _file_length = static_cast<unsigned long>(_file.tellg());
//        if (_file_length == 0)
//        {
//            //file is empty
//            pState = -3;
//            return L"";
//        }
//
//        if (!_file.is_open())
//        {
//            //could not open file
//            pState = -4;
//            return nullptr;
//        }
//
//        auto _source = new wchar_t[_file_length + 1];
//        _source[_file_length] = '\0';
//        _file.seekg(0, std::ios::beg);
//
//        unsigned int _i = 0;
//        while (_file.good())
//        {
//            _source[_i] = _file.get();
//            if (!_file.eof())
//            {
//                _i++;
//            }
//        }
//        _source[_i] = '\0';
//
//        //close the file
//        _file.close();
//
//        pState = 1;
//        return _source;
//    }
//
//    /*
//        Read binary file and return array of uint8_t
//        fileState indicates to state of file
//        1 means everything is ok
//        -1 means the file could not be found,
//        -2 means file is exist but could not open
//    */
//    inline void read_binary_fileW(_In_z_ const wchar_t* pPath, _Inout_ std::vector<uint8_t>& pData,
//        _Out_ int& pFileState)
//    {
//        pFileState = 1;
//        std::ifstream _file(pPath, std::ios::binary);
//        if (!_file)
//        {
//            pFileState = -1;
//            return;
//        }
//
//        if (_file.fail())
//        {
//            pFileState = -2;
//            return;
//        }
//
//        // Stop eating new lines in binary mode!!!
//        _file.unsetf(std::ios::skipws);
//
//        // get its size:
//        //std::streampos _file_size;
//        size_t _data_size = 0;
//
//        _file.seekg(0, std::ios::end);
//        _data_size = static_cast<size_t>(_file.tellg());
//        _file.seekg(0, std::ios::beg);
//
//        // reserve capacity
//        pData.reserve(_data_size);
//
//        // read the data:
//        std::copy(std::istream_iterator<uint8_t>(_file),
//            std::istream_iterator<uint8_t>(),
//            std::back_inserter(pData));
//
//    }
//
//#endif
//    /*
//        Read binary file and return array of uint8_t
//        fileState indicates to state of file
//        1 means everything is ok
//        -1 means the file could not be found,
//        -2 means file is exist but could not open
//    */
//    inline void read_binary_file(_In_z_ const char* pPath, _Inout_ std::vector<uint8_t>& pData,
//        _Out_ int& pFileState)
//    {
//        pFileState = 1;
//        std::ifstream _file(pPath, std::ios::binary);
//        if (!_file)
//        {
//            pFileState = -1;
//            return;
//        }
//
//        if (_file.fail())
//        {
//            pFileState = -2;
//            return;
//        }
//
//        // Stop eating new lines in binary mode!!!
//        _file.unsetf(std::ios::skipws);
//
//        // get its size:
//        //std::streampos _file_size;
//        size_t _data_size = 0;
//
//        _file.seekg(0, std::ios::end);
//        _data_size = static_cast<size_t>(_file.tellg());
//        _file.seekg(0, std::ios::beg);
//
//        // reserve capacity
//        pData.reserve(_data_size);
//
//        // read the data:
//        std::copy(std::istream_iterator<uint8_t>(_file),
//            std::istream_iterator<uint8_t>(),
//            std::back_inserter(pData));
//
//    }

