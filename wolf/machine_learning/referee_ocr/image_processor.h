#ifndef ImageProcessor_H
#define ImageProcessor_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "salieri.h"

/*! \brief This class is responsible for image processing tasks.

        This class contains functions, structures, and variables that use for
   image processing purposes. In the project, in other classes, in the case of
   processing image, one must create an object of the ImageProcessing class.
*/

//! Restriction struct.
/*!
                Characters must satisfy the struct restrictions.
        */
struct restrictions_struct {
  int min_area;
  int max_area;
  int min_height;
  int max_height;
  int min_width;
  int max_width;
};

//! OCR configuration struct.
/*!
                The necessary configurations for processing optical characters.
        */
struct config_for_ocr_struct {
  /*!<If true, then the input image box change to the binary form.*/
  bool binary = false;
  /*!<The best height for processing. depend on the model.*/
  int desired_contour_height = 32;
  /*!<If true, it means the frame should be blurred.*/
  bool do_blur = true;
  /*!<The image box would be resized by the factor.*/
  bool do_resize = false;
  /*!<If true, it means the contour should be resized.*/
  bool do_resize_contour = false;
  /*!<If true, it means the frame should be thresholded.*/
  bool do_threshold = true;
  /*!<This fraction applies to finding out the space between two characters.*/
  double fraction = 0.9;
  /*!<Gaussian kernel size. the size must be positive and odd.*/
  int gaussian_blur_win_size = 3;
  /*!<If true, we are storing the image boxes*/
  bool if_store_image_boxes = false;
  /*!<If the character is white, then set it true.*/
  bool is_white = true;
  /*!<If the character is number, then set it true.*/
  bool is_digit = true;
  /*!<If the background needs to be white, then set it true.*/
  bool make_white_background = false;
  /*!<The margin between a character contour and the image window box.*/
  int margin = 6;
  /*!<The debugging path*/
  std::string ocr_debug_path = "../../database/ocr_debug_data";
  /*!<The contours with overlap more than this threshold would be combined.*/
  double overlapped_threshold = 0.2;
  /*!<Apply to resizing the image boxes.*/
  double resized_fraction = 1.0;
  /*!<The maximum height for an image window.*/
  int resized_height = 40;
  /*!<The maximum height for an image window.*/
  int resized_width = 40;
  /*!<The pixels with a value greater than the threshold change to 255.*/
  int threshold_value = 110;
  /*!<If true, then image converts to grayscale*/
  bool to_gray = true;
  /*!<If true, during the code execution some extra log would be shown.*/
  bool verbose = false;
  /*!<The character's background should be white.*/
  int white_background_threshold = 150;
  /*!<Characters must satisfy the struct restrictions.*/
  restrictions_struct restrictions;
};

/*!
        Find all contours in filtered image.
        \param  filtered_image    The image needs to be processed.
        \return vector of contours..
        */
std::vector<std::vector<cv::Point>>
find_all_countors(_In_ cv::Mat &filtered_image);

/*!
        blur image by specified configuration
        \param  frame_box    The image needs to be processed.
        \param  ocr_config    The necessary configurations for processing
   optical characters.
        */
void gaussian_blur(_Inout_ cv::Mat &frame_box,
                   _In_ config_for_ocr_struct &ocr_config);

/*!
        takes an image and makes the background white. this function uses a
   threshold to find background points \param  contour_image    The image needs
   to be processed. \param  ocr_config    The necessary configurations for
   processing optical characters.
        */
void make_contour_white_background(_Inout_ cv::Mat &contour_image,
                                   _In_ config_for_ocr_struct &ocr_config);

/*!
          The negative_image function changed the pixels' value. The new value
   is obtained by 255 - the previous value.

          \param  contour_image    contour_image is a cropped part of the
   original image that contains one of the contours.
        */
void negative_image(_Inout_ cv::Mat &contour_image);

/*!
          apply some filters to better find contours.

          \param  image    The image needs to be processed.
          \param  ocr_config    The necessary configurations for processing
   optical characters.
        */
cv::Mat
prepare_image_for_contour_detection(_In_ cv::Mat &image,
                                    _In_ config_for_ocr_struct &ocr_config);

/*!
          resize image to specified size.
          \param  frame_box    The image needs to be processed.
          \param  dest_height    The height of destination image.
          \param  dest_width    The width of destination image.
        */
void resize_image(_Inout_ cv::Mat &frame_box, _In_ int dest_height = -1,
                  _In_ int dest_width = -1);

/*!
        The initial_filtering function prepares the input image for processing.
        \param  frame_box     The image needs to be processed.
        \param  ocr_config    The necessary configurations for processing
   optical characters.
        */
void threshold_image(_Inout_ cv::Mat &frame_box,
                     _In_ config_for_ocr_struct &ocr_config);

#endif
