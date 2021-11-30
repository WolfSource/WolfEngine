#ifdef WOLF_ENABLE_HTTP1_1_WS

#pragma once

#include <filesystem>
#include <w_log/w_log.h>

struct AsyncFileStreamer {

    std::map<std::string_view, AsyncFileReader *> asyncFileReaders;
    std::string root;

    AsyncFileStreamer(std::string root) : root(root) 
    {
        // for all files in this path, init the map of AsyncFileReaders
        updateRootCache();
    }

    void updateRootCache()
    {
        // todo: if the root folder changes, we want to reload the cache
        for (auto& p : std::filesystem::recursive_directory_iterator(root))
        {
            std::string url = p.path().string().substr(root.length());
            if (url == "/index.html")
            {
                url = "/";
            }

            char* key = new char[url.length()];
            memcpy(key, url.data(), url.length());
            asyncFileReaders[std::string_view(key, url.length())] = new AsyncFileReader(p.path().string());
        }
    }

    template <bool SSL>
    void streamFile(uWS::HttpResponse<SSL>* res, std::string_view url) 
    {
        const char* _trace_info = "streamFile";

        auto it = asyncFileReaders.find(url);
        if (it == asyncFileReaders.end())
        {
            LOG_P(
                w_log_type::W_LOG_INFO,
                "file could not find! %s. trace info: %s",
                url,
                _trace_info);
        }
        else
        {
            streamFile(res, it->second);
        }
    }

    template <bool SSL>
    static void streamFile(uWS::HttpResponse<SSL>* res, AsyncFileReader* asyncFileReader)
    {
        /* Peek from cache */
        std::string_view chunk = asyncFileReader->peek(res->getWriteOffset());
        if (!chunk.length() || res->tryEnd(chunk, asyncFileReader->getFileSize()).first)
        {
            /* Request new chunk */
            // todo: we need to abort this callback if peer closed!
            // this also means Loop::defer needs to support aborting (functions should embedd an atomic boolean abort or something)

            // Loop::defer(f) -> integer
            // Loop::abort(integer)

            if (chunk.length() < asyncFileReader->getFileSize())
            {
                asyncFileReader->request(res->getWriteOffset(), [res, asyncFileReader](std::string_view chunk) {
                    // check if we were closed in the mean time
                    //if (us_socket_is_closed()) {
                        // free it here
                        //return;
                    //}

                    /* We were aborted for some reason */
                    if (!chunk.length())
                    {
                        // todo: make sure to check for is_closed internally after all callbacks!
                        res->close();
                    }
                    else
                    {
                        AsyncFileStreamer::streamFile(res, asyncFileReader);
                    }
                    });
            }
        }
        else
        {
            /* We failed writing everything, so let's continue when we can */
            res->onWritable([res, asyncFileReader](int offset)
                {
                    AsyncFileStreamer::streamFile(res, asyncFileReader);
                    // todo: I don't really know what this is supposed to mean?
                    return false;
                })->onAborted([]()
                    {
                        const char* _trace_info = "streamFile::onAborted";
                        LOG_P(
                            w_log_type::W_LOG_INFO,
                            "streaming file aborted! trace info: %s",
                            _trace_info);
                    });
        }
    }
};

#endif
