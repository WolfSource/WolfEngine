#include "w_desktop_capturer.hpp"
#include <api/video/i420_buffer.h>
#include <libyuv/convert.h>
#include <libyuv/video_common.h>
#include <media/base/video_common.h>
#include <modules/desktop_capture/desktop_capture_options.h>

using w_desktop_capturer = wolf::stream::webRTC::w_desktop_capturer;

w_desktop_capturer::w_desktop_capturer(uint32_t p_width, uint32_t p_height)
    : width_(p_width), height_(p_height) {}

w_desktop_capturer::~w_desktop_capturer() { this->stop(); }

void w_desktop_capturer::start()
{
  this->is_running_ = true;
  this->capture_thread_ = std::thread([this]()
                                      {
#pragma unroll
    while (get_is_running()) {
      this->capturer->CaptureFrame();
    } });
  this->capturer->Start(this);
}

void w_desktop_capturer::stop()
{
  this->is_running_ = false;
  if (this->capture_thread_.joinable())
  {
    this->capture_thread_.join();
  }
}

bool w_desktop_capturer::get_is_running() const { return this->is_running_; }

uint32_t w_desktop_capturer::get_height() const { return this->height_; }

uint32_t w_desktop_capturer::get_width() const { return this->width_; }

void w_desktop_capturer::OnCaptureResult(
    webrtc::DesktopCapturer::Result p_result,
    std::unique_ptr<webrtc::DesktopFrame> p_frame)
{

  if (p_result != webrtc::DesktopCapturer::Result::SUCCESS)
  {
    //   RTC_LOG(LS_ERROR) << "DesktopCapturer:OnCaptureResult capture error:"
    //                     << (int)result;
    return;
  }

  auto width = p_frame->stride() / webrtc::DesktopFrame::kBytesPerPixel;
  auto height = p_frame->rect().height();

  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer =
      webrtc::I420Buffer::Create(width, height);

  const auto conversion_result = libyuv::ConvertToI420(
      p_frame->data(), 0, i420_buffer->MutableDataY(), i420_buffer->StrideY(),
      i420_buffer->MutableDataU(), i420_buffer->StrideU(),
      i420_buffer->MutableDataV(), i420_buffer->StrideV(), 0, 0, width, height,
      i420_buffer->width(), i420_buffer->height(), libyuv::kRotate0,
      ::libyuv::FOURCC_ARGB);

  if (conversion_result < 0)
  {
    // RTC_LOG(LS_ERROR) << "DesktopCapturer:OnCaptureResult conversion
    // error:"
    //                   << conversionResult;
    return;
  }

  auto video_frame = webrtc::VideoFrame(
      i420_buffer, webrtc::VideoRotation::kVideoRotation_0, rtc::TimeMicros());

  if ((this->height_ == 0) && (this->width_ == 0))
  {
    this->broadcaster.OnFrame(video_frame);
    return;
  }

  width = this->width_;
  height = this->height_;

  if (height == 0)
  {
    height = (video_frame.height() * width) / video_frame.width();
  }
  if (width == 0)
  {
    width = (video_frame.width() * height) / video_frame.height();
  }

  auto stride_y = width;
  auto stride_uv = (width + 1) / 2;

  rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
      webrtc::I420Buffer::Create(width, height, stride_y, stride_uv, stride_uv);

  scaled_buffer->ScaleFrom(*video_frame.video_frame_buffer()->ToI420());

  auto frame = webrtc::VideoFrame::Builder()
                   .set_video_frame_buffer(scaled_buffer)
                   .set_rotation(webrtc::kVideoRotation_0)
                   .set_timestamp_us(rtc::TimeMicros())
                   .build();

  this->broadcaster.OnFrame(frame);
}
