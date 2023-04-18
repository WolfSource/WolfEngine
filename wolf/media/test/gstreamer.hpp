/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_GSTREAMER)

#pragma once

#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <media/gst/w_application.hpp>
#include <media/gst/core/w_refptr.hpp>
#include <media/gst/core/w_buffer.hpp>
#include <media/gst/core/w_mainloop.hpp>
#include <media/gst/core/w_pipeline.hpp>
#include <media/gst/core/w_element_factory.hpp>
#include <media/gst/video/w_video_info.hpp>
#include <media/gst/video/w_video_format.hpp>
#include <media/gst/elements/w_element_appsrc.hpp>

#include <thread>

BOOST_AUTO_TEST_CASE(gstreamer_wrapper) {
    namespace gst = wolf::media::gst;

    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void> {
            gst::w_application::init(nullptr, nullptr);

            std::size_t width = 640;
            std::size_t height = 480;

            BOOST_LEAF_AUTO(video_info,
                    gst::w_video_info::make(gst::w_video_format::RGBx, width, height));
            auto video_caps = video_info.to_caps();

            BOOST_LEAF_AUTO(pipeline, gst::w_pipeline::make("main"));
            BOOST_LEAF_AUTO(mainloop, gst::w_mainloop::make());

            BOOST_LEAF_AUTO(appsrc, gst::w_element_appsrc::make());
            auto appsrc_ref = gst::to_refptr(std::move(appsrc));
            appsrc_ref->set_caps(video_caps);
            appsrc_ref->set_format(gst::w_format::Time);

            auto push_data = [&appsrc_ref, width, height, frames=0]() mutable {
                auto buffer = gst::w_buffer::make(width * height * 4);
                if (!buffer) {
                    return;
                }

                //buffer->set_timestamp(appsrc_ref->clock()->get_time());
                buffer->set_duration(double(frames) / 120.0 * 1e9);

                {
                    // make sure buffer_map doesn't outlive buffer.
                    auto data_map = buffer->map_data_write();
                    auto data = data_map->data();
                    for (std::size_t y = 0; y < height; ++y) {
                        for (std::size_t x = 0; x < width; ++x) {
                            // a simple SDF rendering. a square in middle,
                            // and a circle of same size passing through diagonally.
                            const auto ratio = double(width) / height;
                            const auto t = double(frames % 120) / 120 - 0.5;
                            const auto u = (double(x) / width - 0.5) * ratio;
                            const auto v = double(y) / height - 0.5;

                            const auto square = std::max(std::abs(u), std::abs(v)) - 0.2;
                            const auto circle = std::sqrt(std::pow(u - t, 2) + std::pow(v - t, 2)) - 0.2;

                            data[y * width * 4 + x * 4 + 0] = (square < 0) * 255;
                            data[y * width * 4 + x * 4 + 1] = (circle < 0) * 255;
                            data[y * width * 4 + x * 4 + 2] = 0;
                            data[y * width * 4 + x * 4 + 3] = 0;
                        }
                    }
                }

                appsrc_ref->emit_push_buffer(std::move(*buffer));

                frames = (frames + 1) % 120;
            };

            std::size_t sourceid = 0;
            appsrc_ref->hook_need_data([&](auto /* length */) {
                if (sourceid) return;
                sourceid = mainloop.idle_add(push_data);
            });
            appsrc_ref->hook_enough_data([&] {
                mainloop.idle_remove(sourceid);
                sourceid = 0;
            });

            BOOST_LEAF_AUTO(test_video_src,
                    gst::w_element_factory::make_simple("videotestsrc", "src"));
            BOOST_LEAF_AUTO(video_convert,
                    gst::w_element_factory::make_simple("videoconvert", "video_convert"));
            BOOST_LEAF_AUTO(auto_video_sink,
                    gst::w_element_factory::make_simple("autovideosink", "sink"));

            auto flow = gst::w_flow_path::make(
                appsrc_ref,
                std::move(video_convert),
                std::move(auto_video_sink)
            );

            BOOST_REQUIRE(pipeline.bin(flow));
            BOOST_REQUIRE(pipeline.link(flow));

            BOOST_REQUIRE(pipeline.play());

            auto timer = std::jthread([&mainloop, &pipeline]() {
                for (int i = 1; i <= 5; ++i) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                pipeline.stop();
                mainloop.stop();
            });

            mainloop.run();

            return {};
        },
        [](const w_trace &p_trace) {
          BOOST_ERROR(wolf::format("got error: {}", p_trace.to_string()));
        },
        [] {
          BOOST_ERROR(wolf::format("got an error"));
        }
    );
}

#endif  // defined(WOLF_TEST) && defined(WOLF_MEDIA_GSTREAMER)
