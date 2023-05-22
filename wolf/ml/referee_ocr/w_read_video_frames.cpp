#include "w_read_video_frames.hpp"
#ifdef __TELEMETRY
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/trace/provider.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

namespace {
nostd::shared_ptr<trace::Tracer> get_tracer() {
  auto provider = trace::Provider::GetTracerProvider();
  return provider->GetTracer("pes_21", OPENTELEMETRY_SDK_VERSION);
}

} // namespace
#endif

using w_read_video_frames = wolf::ml::ocr::w_read_video_frames;

w_read_video_frames::w_read_video_frames(std::string video_path) {
  path = video_path;

  cap = cv::VideoCapture(path.c_str(), cv::CAP_ANY);
  if (!cap.isOpened()) {
    std::cout << "Error opening video stream or file" << std::endl;
  }
  frame_count = cap.get(cv::CAP_PROP_FRAME_COUNT);
}

cv::Mat w_read_video_frames::read_video_frame_by_frame() {
  cv::Mat frame;
  if (!cap.read(frame)) {
    release();
    return frame;
  }
  frame_number += 1;
  return frame;
}

void w_read_video_frames::write_image_to_video(std::string out_video_path) {
#ifdef __TELEMETRY
  auto scoped_span =
      trace::Scope(get_tracer()->StartSpan("write_image_to_video"));
#endif
  cv::Mat frame;

  frame = read_video_frame_by_frame();
  if (frame.empty()) {
    std::cout << "Error opening video stream" << std::endl;
    return;
  }
  int frame_width = frame.cols;
  int frame_height = frame.rows;

  // Define the codec and create VideoWriter object.The output is stored in
  // 'outcpp.avi' file.
  cv::VideoWriter video(out_video_path,
                        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 60.0,
                        cv::Size(frame_width, frame_height));

  while (true) {
    frame = read_video_frame_by_frame();
    if (frame.empty())
      break;

    // Write the frame into the file 'outcpp.avi'
    video.write(frame);
    cv::waitKey(10);
  }

  video.release();
  frame.release();

  return;
}

void w_read_video_frames::release() { cap.release(); }

cv::Mat w_read_video_frames::read_specific_frame(int input_frame_number) {
  frame_number = input_frame_number;

  cv::Mat frame;
  if (frame_number > int(frame_count)) {
    release();
    return frame;
  }
  cap.set(1, frame_number);

  if (!cap.read(frame)) {
    return frame;
  }

  return frame;
}

int w_read_video_frames::get_current_frame_number() { return frame_number; }

double w_read_video_frames::get_frame_amount() { return frame_count; }

void w_read_video_frames::video_player() {
  w_read_video_frames player(path);

  int skip = 100;
  int frame_number = 52000 - skip;

  while (true) {
    frame_number += skip;

    cv::Mat frame = player.read_specific_frame(frame_number);
    cv::imshow("video player", frame);
    std::cout << frame_number << std::endl;
    int key = cv::waitKey();
    //        std::cout << key << std::endl;

    if (key == 1113937 || key == 1113940) {
      frame_number -= 2 * skip;
    }
  }
}
