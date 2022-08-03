/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <map>
#include <string>
#include <memory>
#include <gsl/gsl>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/scoped_refptr.h>
#include <pc/video_track_source.h>
#include <api/video/video_frame.h>
#include <api/video/video_source_interface.h>

#include "DISABLE_ANALYSIS_END"

constexpr bool S_REMOTE = false;

namespace wolf::stream::webRTC
{
    template <class T>
    class w_track_source : public webrtc::VideoTrackSource
    {
    public:
        static rtc::scoped_refptr<w_track_source> create(
            const std::string &p_video_url,
            const std::map<std::string, std::string> &p_opts)
        {
            auto *_c = T::create(p_video_url, p_opts);
            std::unique_ptr<T> capturer = absl::WrapUnique(_c);
            if (capturer == nullptr)
            {
                return nullptr;
            }
            return rtc::make_ref_counted<w_track_source>(std::move(capturer));
        }

        virtual bool GetStats(Stats *p_stats) override
        {
            bool result = false;
            auto *source = this->_capturer.get();
            if (source != nullptr)
            {
                p_stats->input_height = gsl::narrow_cast<int>(source->get_height());
                p_stats->input_width = gsl::narrow_cast<int>(source->get_width());
                result = true;
            }
            return result;
        }

    protected:
        explicit w_track_source(std::unique_ptr<T> p_capturer)
            : webrtc::VideoTrackSource(S_REMOTE), _capturer(std::move(p_capturer)) {}

        SourceState state() const noexcept override
        {
            return SourceState::kLive;
        }

    private:
        rtc::VideoSourceInterface<webrtc::VideoFrame> *source() noexcept override
        {
            return this->_capturer.get();
        }

        std::unique_ptr<T> _capturer;
    };
} // namespace wolf::stream::webRTC