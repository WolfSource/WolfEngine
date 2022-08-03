/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>
#include <thread>

#include "DISABLE_ANALYSIS_BEGIN"

#include <stream/webrtc/media/w_video_source.hpp>
#include <memory>
#include <modules/desktop_capture/desktop_capturer.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    class w_desktop_capturer : public w_video_source, public webrtc::DesktopCapturer::Callback
    {
    public:
        w_desktop_capturer(uint32_t p_width = 0, uint32_t p_height = 0);

        // prevent copy constructor
        w_desktop_capturer(const w_desktop_capturer &) = delete;

        // prevent copying
        w_desktop_capturer &operator=(const w_desktop_capturer &) = delete;

        // destructor
        virtual ~w_desktop_capturer();

        /**
         * start webrtc desktop capturing
         */
        void start();

        /**
         * stop webrtc desktop capturing
         */
        void stop();

        /**
         * check is the capturing thread running
         */
        [[nodiscard]] bool get_is_running() const;

        /**
         * returns the height
         */
        [[nodiscard]] uint32_t get_height() const;

        /**
         * returns the width
         */
        [[nodiscard]] uint32_t get_width() const;

        /**
         * overide webrtc::DesktopCapturer::Callback
         */
        void OnCaptureResult(webrtc::DesktopCapturer::Result p_result,
                             std::unique_ptr<webrtc::DesktopFrame> p_frame) override;

    private:
        std::thread capture_thread_;
        std::unique_ptr<webrtc::DesktopCapturer> capturer = nullptr;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        bool is_running_ = false;
    };
} // namespace wolf::stream::webRTC
