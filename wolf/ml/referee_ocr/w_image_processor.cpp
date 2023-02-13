#include "w_image_processor.hpp"

#include "salieri.h"

// using config_for_ocr_struct = wolf::ml::ocr::config_for_ocr_struct;
// using gaussian_blur = wolf::ml::ocr::gaussian_blur;
// using make_contour_white_background =
//     wolf::ml::ocr::make_contour_white_background;
// using negative_image = wolf::ml::ocr::negative_image;
// using prepare_image_for_contour_detection =
//     wolf::ml::ocr::prepare_image_for_contour_detection;
// using resize_image = wolf::ml::ocr::resize_image;
// using threshold_image = wolf::ml::ocr::threshold_image;
// using config_for_ocr_struct = wolf::ml::ocr::config_for_ocr_struct;
// using namespace wolf::ml::ocr;

namespace wolf::ml::ocr {

std::vector<std::vector<cv::Point>>
find_all_countors(_In_ cv::Mat &filtered_image) {
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(filtered_image, contours, hierarchy, cv::RETR_TREE,
                   cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

  return contours;
}

void gaussian_blur(_Inout_ cv::Mat &frame_box,
                   _In_ config_for_ocr_struct &ocr_config) {
  int kernel_size = ocr_config.gaussian_blur_win_size;
  cv::GaussianBlur(frame_box, frame_box, cv::Size(kernel_size, kernel_size), 0,
                   0);
}

void make_contour_white_background(_Inout_ cv::Mat &contour_image,
                                   _In_ config_for_ocr_struct &ocr_config) {
  int height = contour_image.rows;
  int width = contour_image.cols;
  int n_channels = contour_image.channels();

  if (ocr_config.make_white_background) {
    if (n_channels == 1) {
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          if (contour_image.at<uchar>(i, j) >
              ocr_config.white_background_threshold) {
            contour_image.at<uchar>(i, j) = 180;
          }
        }
      }
    } else if (n_channels == 3) {
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          cv::Vec3b &color_pixel = contour_image.at<cv::Vec3b>(i, j);
          int count = 0, combine = 0;
          for (int i = 0; i < 3; i++) {
            if (color_pixel[i] > ocr_config.white_background_threshold) {
              count++;
              combine += color_pixel[i];
            }
          }
          if (count > 2 ||
              combine > ocr_config.white_background_threshold * 2 + 100) {
            color_pixel[0] =
                255; // (color_pixel[0]*2 > 255) ? 255:color_pixel[0];
            color_pixel[1] =
                255; // (color_pixel[1]*2 > 255) ? 255:color_pixel[1];
            color_pixel[2] =
                255; // (color_pixel[2]*2 > 255) ? 255:color_pixel[2];
          }
        }
      }
    }
  }
}

void negative_image(_Inout_ cv::Mat &contour_image) {
  int height = contour_image.rows;
  int width = contour_image.cols;
  int n_channels = contour_image.channels();

  if (n_channels == 1) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        contour_image.at<uchar>(i, j) = 255 - contour_image.at<uchar>(i, j);
      }
    }
  } else if (n_channels == 3) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        cv::Vec3b &color_pixel = contour_image.at<cv::Vec3b>(i, j);
        color_pixel[0] = 255 - color_pixel[0];
        color_pixel[1] = 255 - color_pixel[1];
        color_pixel[2] = 255 - color_pixel[2];
      }
    }
  }
}

cv::Mat
prepare_image_for_contour_detection(_In_ cv::Mat &image,
                                    _In_ config_for_ocr_struct &ocr_config) {
  cv::Mat filtered_image = image.clone();
  if (ocr_config.do_resize) {
    int dist_height = ocr_config.resized_height;
    resize_image(filtered_image, dist_height);
  }

  if (ocr_config.do_blur) {
    gaussian_blur(filtered_image, ocr_config);
  }

  if (ocr_config.do_threshold) {
    threshold_image(filtered_image, ocr_config);
  }
  return filtered_image;
}

void resize_image(_Inout_ cv::Mat &frame_box, _In_ int dest_height,
                  _In_ int dest_width) {
  /*!<fraction = 1.0*/
  float ratio;
  cv::Size dim;

  int frame_height = frame_box.rows;
  int frame_width = frame_box.cols;

  if (dest_width == -1 && dest_height == -1) {
    return;
  }

  if (dest_width == -1) {
    ratio = float(dest_height) / float(frame_height);
    dim.width = int(frame_width * ratio);
    dim.height = dest_height;
    // fraction = r;
  } else {
    ratio = float(dest_width) / float(frame_width);
    dim.width = dest_width;
    dim.height = int(frame_height * ratio);
    // fraction = r;
  }
  cv::resize(frame_box, frame_box, dim);
}

void threshold_image(_Inout_ cv::Mat &frame_box,
                     _In_ config_for_ocr_struct &ocr_config) {
  if (frame_box.channels() == 3) {
    if (ocr_config.binary) {
      cv::cvtColor(frame_box, frame_box, cv::COLOR_BGR2GRAY);
      cv::threshold(frame_box, frame_box, 0, 255, cv::THRESH_OTSU);
      cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3),
                                                 cv::Point(0, 0));

      cv::morphologyEx(frame_box, frame_box, cv::MORPH_CLOSE, kernel);
    } else {
      cv::inRange(frame_box,
                  cv::Scalar(ocr_config.threshold_value,
                             ocr_config.threshold_value,
                             ocr_config.threshold_value),
                  cv::Scalar(255, 255, 255), frame_box);
    }
  } else if (frame_box.channels() == 1) {
    cv::threshold(frame_box, frame_box, ocr_config.threshold_value, 255,
                  cv::THRESH_BINARY);
  }
  // return image;
}
} // namespace wolf::ml::ocr
