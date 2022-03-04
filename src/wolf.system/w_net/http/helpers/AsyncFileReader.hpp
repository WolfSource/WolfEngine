#ifdef WOLF_ENABLE_HTTP1_1_WS

#pragma once

#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <future>
#include "../../ws/uWebSockets/App.h"
#include <w_log/w_log.h>

/* This is just a very simple and inefficient demo of async responses,
 * please do roll your own variant or use a database or Node.js's async
 * features instead of this really bad demo */
struct AsyncFileReader 
{
public:
    /* Construct a demo async. file reader for fileName */
    AsyncFileReader(std::string pFileName) : _file_name(pFileName)
    {
        const char* _trace_info = "AsyncFileReader";

        fin.open(this->_file_name, std::ios::binary);

        // get fileSize
        fin.seekg(0, fin.end);
        this->_file_size = fin.tellg();

        LOG_P(
            w_log_type::W_LOG_INFO,
            "File size is: %d! trace info: %s",
            this->_file_size,
            _trace_info);

        // cache up 1 mb!
        cache.resize(1024 * 1024);

        //std::cout << "Caching 1 MB at offset = " << 0 << std::endl;
        fin.seekg(0, fin.beg);
        fin.read(cache.data(), cache.length());
        cacheOffset = 0;
        hasCache = true;

        // get loop for thread

        loop = uWS::Loop::get();
    }

    /* Returns any data already cached for this offset */
    std::string_view peek(int offset) 
    {
        /* Did we hit the cache? */
        if (hasCache && offset >= cacheOffset && ((offset - cacheOffset) < cache.length()))
        {
            /* Cache hit */
            //std::cout << "Cache hit!" << std::endl;

            /*if (fileSize - offset < cache.length()) {
                std::cout << "LESS THAN WHAT WE HAVE!" << std::endl;
            }*/

            int chunkSize = std::min<int>(this->_file_size - offset, cache.length() - offset + cacheOffset);

            return std::string_view(cache.data() + offset - cacheOffset, chunkSize);
        }
        else
        {
            /* Cache miss */
            //std::cout << "Cache miss!" << std::endl;
            return std::string_view(nullptr, 0);
        }
    }

    /* Asynchronously request more data at offset */
    void request(int offset, std::function<void(std::string_view)> cb)
    {
        const char* _trace_info = "AsyncFileReader::request";

        // in this case, what do we do?
        // we need to queue up this chunk request and callback!
        // if queue is full, either block or close the connection via abort!
        if (!hasCache)
        {
            // already requesting a chunk!
            LOG_P(
                w_log_type::W_LOG_ERROR,
                "already requesting a chunk! trace info: %s",
                _trace_info);
            return;
        }

        // disable cache
        hasCache = false;

        std::async(std::launch::async, [this, cb, offset]()
            {
                //std::cout << "ASYNC Caching 1 MB at offset = " << offset << std::endl;

                // den har stängts! öppna igen!
                if (!fin.good())
                {
                    fin.close();
                    //std::cout << "Reopening fin!" << std::endl;
                    fin.open(_file_name, std::ios::binary);
                }
                fin.seekg(offset, fin.beg);
                fin.read(cache.data(), cache.length());

                cacheOffset = offset;

                loop->defer([this, cb, offset]()
                    {
                        const char* _trace_info = "AsyncFileReader::request::async::defer";

                        int _chunk_size = std::min<int>(cache.length(), this->_file_size - offset);

                        if (_chunk_size == 0)
                        {
                            LOG_P(
                                w_log_type::W_LOG_ERROR,
                                "chunk size equals zero! trace info: %s",
                                _trace_info);
                        }

                        if (_chunk_size != cache.length())
                        {
                            LOG_P(
                                w_log_type::W_LOG_ERROR,
                                "less than a cache 1 MB! trace info: %s",
                                _trace_info);
                        }

                        hasCache = true;
                        cb(std::string_view(cache.data(), _chunk_size));
                    });
            });
    }

    //Abort any pending async. request
    void abort()
    {
    }

    int getFileSize() 
    {
        return this->_file_size;
    }

    private:
        /* The cache we have in memory for this file */
        std::string cache;
        int cacheOffset;
        bool hasCache;

        /* The pending async file read (yes we only support one pending read) */
        std::function<void(std::string_view)> pendingReadCb;

        int _file_size;
        std::string _file_name;
        std::ifstream fin;
        uWS::Loop* loop;
};

#endif