/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <tesseract/baseapi.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "w_image_processor.hpp"

namespace wolf::ml::ocr {

//! optical character reader class.
/*! \brief This class is responsible for manipulating the OCR tasks.

        This class contains functions, structures, and variables that use
   for optical character reading purposes. In the project, in other classes,
   in the case of reading an optical character, one must create an object of
   the OCR class.
*/
class w_ocr_engine {
public:
  //! The enum shows the position of the cluster in the whole image.
  enum cluster_position { left, right, middle };
  //! The struct contains the features related to a cluster.
  struct cluster_features {
    /*!<The minimum amount of x value on the x-axis between all characters in
     * the cluster.*/
    int min_x;
    /*!<The maximum amount of x value on the x-axis between all characters in
     * the cluster.*/
    int max_x;
    /*!<The minimum amount of y value on the y-axis between all characters in
     * the cluster.*/
    int min_y;
    /*!<The average amount of y value of all characters.*/
    int average_y;
    /*!<The average height of all characters in pixel.*/
    int average_height;
    /*!<The distance between min_x and the middle of the frame.*/
    int symmetric_x1;
    /*!<The distance between max_x and the middle of the frame.*/
    int symmetric_x2;
    /*!<The position of the cluster in the frame.*/
    cluster_position position;
    /*!<The index of the current array in the parent vector.*/
    int index_in_parent_vector;
    /*!<The index of the twin array in the parent vector.*/
    int twin_index_in_parent_vector;

    /*!<If the cluster has a twin this variable should set true.*/
    bool matched = false;
  };
  //! Struct of the character information.
  struct characters_struct {
    /*!<Change to true after processing the character.*/
    bool processed = false;
    /*!<This variable shows the height of the character in pixel.*/
    int height;
    /*!<This variable contains the coordination of the character.*/
    cv::Point center;
    /*!<Show the character bounding box corners.*/
    cv::Rect bound_rect;
    /*!<The character text.*/
    std::string text;
    /*!<The character contour area.*/
    std::vector<cv::Point> contour;
    /*!<The operator is added to the struct to make it sortable.*/
    bool operator<(const characters_struct &rhs) const {
      return center.x < rhs.center.x;
    }
  };
  //! Struct of the character information.
  struct result_boxes {
    bool got_result = false;

    characters_struct home_name;
    characters_struct away_name;
    characters_struct home_result;
    characters_struct away_result;
  };
  //! Struct of the character value and position.
  struct character_and_center {
    std::string text = "";
    cv::Point center;
    /*!<The operator is added to the struct to make it sortable.*/
    bool operator<(const character_and_center &rhs) const {
      return center.x < rhs.center.x;
    }
  };
  // config_for_ocr_struct config_for_ocr_struct2;

  //! All configurations need for w_ocr_engine.
  /*!
          The necessary configurations use for creating w_ocr_engine class
     objects.
  */
  struct config_struct {
    /*!<Set a name for each window area.*/
    std::string name;
    /*!<If is time then true.*/
    bool is_time = false;
    bool relative = false;
    bool is_result = false;
    /*!<Show if the area has been checked.*/
    bool if_checked = false;
    /*!<Show the window corners.*/
    cv::Rect window;
    float fraction = 1.0;
    /*!<OCR configuration struct.*/
    config_for_ocr_struct config_for_ocr;
  };

  //! Result struct.
  struct result {
    int home_result;
    int away_result;
    std::string home_name;
    std::string away_name;
  };

  /*!
          The constructor of the class.

          Tesseract objects are initialized in the constructor.
  */
  w_ocr_engine();
  /*!
          The deconstructor of the class.

          Tesseract objects are ended in the deconstructor.
  */
  ~w_ocr_engine();

  /*!
          The char_vec_to_string function converts the input vector to the
     related strings.

          \param  frame    The image needs to be processed.
          \param  frame_number    The current image is the nth image of the
     video. frame_number contains the value of n. \param  ocr_config    The
     necessary configurations for processing optical characters. \return returns
     the text of the game scene
  */
  auto char_vec_to_string(
      _In_ std::vector<w_ocr_engine::characters_struct> char_vector,
      _In_ cv::Mat &frame, _In_ config_for_ocr_struct &ocr_config)
      -> std::vector<character_and_center>;

  /*!
          The image_to_string function gets an image and returns the text of the
     box.

          \param  frame_box    The image needs to be processed.
          \param  ocr_config    The necessary configurations for processing
     optical characters. \return    returns the text of frame_box
  */
  std::vector<character_and_center>
  image_to_string(_In_ cv::Mat &frame_box,
                  _In_ config_for_ocr_struct &ocr_config);

  /*!
          The check_if_overlapped checks the input rect of boxes to decide if
     two rects overlapped.

          \param  box_1    The rect information of the first box.
          \param  box_2    The rect information of the second box.
          \param  ocr_config    The necessary configurations for processing
     optical characters. \return    True, if two boxes overlapped.
  */
  bool check_if_overlapped(_In_ cv::Rect box_1, _In_ cv::Rect box_2,
                           _In_ config_for_ocr_struct &ocr_config);

  /*!
          Get vector of contours and create a vector of char structs. this
     structs does not have text.

          \param  contours    a vector contains contours.
          \return    a vector contains character structs
  */
  std::vector<w_ocr_engine::characters_struct>
  contours_to_char_structs(_In_ std::vector<std::vector<cv::Point>> contours);

  /*!
          The enchance_contour_image function modifies the background of the
     contour, makes the background white. The function also resizes the contour
     image to reach a better w_ocr_engine result.

          \param  contour_image    The modified contour image. The output image
     of the function. \param  ocr_config    The necessary configurations for
     processing optical characters.
  */
  void enhance_contour_image_for_model(_Inout_ cv::Mat &contour_image,
                                       _In_ config_for_ocr_struct &ocr_config);

  /*!
          The euclidean_distance function calculates the euclidean distance of
     two input characters.

          \param  first_character    The first character.
          \param  second_character    The second character.
          \return    The calculated distance of two input characters.
  */
  double euclidean_distance(characters_struct &first_character,
                            characters_struct &second_character);

  auto euclidean_distance(int x1, int x2, int y1, int y2) -> double;

  /*!
          The spaces_between_two_chars function returns a string containing the
     spaces that should be placed between characters.

          \param  left_char    The left hand side character.
          \param  right_char   The right hand side character.
          \param  height_to_dist_ratio The ratio of the character height and
     distance. \return    The spaces that should be placed between characters.
  */
  auto spaces_between_two_chars(_In_ characters_struct left_char,
                                _In_ characters_struct right_char,
                                _In_ float height_to_dist_ratio) -> std::string;

  /*!
          The char_clusters_to_text puts the clustered characters together to
     create the word. This function uses one of the class variables as input and
     stores the result in another variable of the class.
  */
  std::vector<character_and_center> char_clusters_to_text(
      _In_ std::vector<std::vector<characters_struct>> clustered_characters);

  /*!
          The filter_chars_by_contour_size function eliminates abnormal contours
     from the char struct vector and returns new vector.

          \param  contours    A vector of all contours.
          \param  ocr_config   The necessary configurations for processing
     optical characters. \return    a vector of char structs.
  */
  std::vector<characters_struct>
  filter_chars_by_contour_size(_In_ std::vector<characters_struct> &character,
                               _In_ config_for_ocr_struct &ocr_config);

  /*!
          the image_to_char_structs takes an image and returns a vector of char
     struct.

          \param  frame_box    image contains characters.
          \param  ocr_config   The necessary configurations for processing
     optical characters. \return    a vector of char structs.
  */
  std::vector<characters_struct>
  image_to_char_structs(_In_ cv::Mat &frame_box,
                        _In_ config_for_ocr_struct &ocr_config);

  /*!
          Takes vector of char structs and recognize text in each struct.

          \param  characters    vector of char structs
          \param  frame_box    image contains characters.
          \param  ocr_config   The necessary configurations for processing
     optical characters. \return    a vector of char structs.
  */
  std::vector<characters_struct>
  label_chars_in_char_structs(_In_ std::vector<characters_struct> &characters,
                              _In_ cv::Mat &frame_box,
                              _In_ config_for_ocr_struct &ocr_config);

  /*!
          The margin_bounding_rect function margins the contours. It is
     necessary for obtaining better results.

          \param  bounding_rect    A vector of the bounding rect of the
     contours. \param  margin    The margin value. \param  filtered_image image
     contains characters.

  */
  void margin_bounding_rect(_Inout_ cv::Rect &bounding_rect, _In_ int margin,
                            _In_ cv::Mat &filtered_image);

  /*!
          The mask_contour function applies the mask of the contour area to the
     input image. The resulting image should contain the contour.

          \param  image    The input image. A window of the original image. The
     window contains all characters of one property. \param  contour_info
     Information of the desired contour. \return  The modified contour image.
     The output image of the function.
  */
  cv::Mat mask_contour(_In_ cv::Mat &image,
                       _In_ characters_struct &contour_info);

  /*!
          The merge_overlapped_contours function merges the overlapped contours.

          \param  bound_rect    A vector of the bounding rect of the contours.
          \param  ocr_config    The necessary configurations for processing
     optical characters.
  */
  void
  merge_overlapped_contours(_Inout_ std::vector<characters_struct> &bound_rect,
                            _In_ config_for_ocr_struct &ocr_config);

  /*!
          The cluster_char_structs function puts related characters togethter.

          \param  ocr_config    The necessary configurations for processing
     optical characters.
  */
  std::vector<std::vector<characters_struct>>
  cluster_char_structs(std::vector<w_ocr_engine::characters_struct> characters,
                       config_for_ocr_struct &ocr_config);

  /*!
          The function resizes the input image and maps it in the output image.
     It helps the programmers to check their algorithm's influence on the input
     image in a better way.

          \param  input_image    The input image.
          \param  out_put_image_height    The height of output image.
          \param  resize_factor    The resize factor.
          \return    returns resized image
  */
  cv::Mat show_in_better_way(cv::Mat &input_image,
                             int out_put_image_height = 400,
                             float resize_factor = 5);

  /*!
          The split_string function uses for splitting input strings based on
     the reference character.

          \param  input_string    The input string.
          \param  reference    The strings that are located between two
     references would be placed in a string. \return    A vector of split
     string.
  */
  std::vector<std::string> split_string(std::string input_string,
                                        char reference);

  /*!
          The same_height function returns true if all characters in the cluster
     share the same height.

          \param  pClusteredChars    The cluster contains many characters.
          \return    The result would be true if all characters in the cluster
     share the same height.
  */
  auto same_height(_In_ std::vector<characters_struct> pClusteredChars) -> bool;

  /*!
          The same_level function returns true if all characters in the cluster
     share the same y-position.

          \param  pClusteredChars    The cluster contains many characters.
          \return    The result would be true if all characters in the cluster
     share the same y-position.
  */
  auto same_level(_In_ std::vector<characters_struct> pClusteredChars) -> bool;

  /*!
          The same_level function returns true if all characters in the cluster
     share the same y-position.

          \param  image    A frame of the game.
          \param  pClusteredChars    The cluster contains many characters.
          \param  pWindowName    The cv::imshow image name.
          \param  pShow
          \return    The output would be a cv image containing the clusters.
  */
  static auto show_contours(_Inout_ cv::Mat &pImage,
                            _In_ std::vector<characters_struct> pClusteredChars,
                            _In_ std::string pWindowName,
                            _In_ bool pShow = true) -> void;

  /*!
          The fill_cluster_features function extracts all feature of the input
     cluster.

          \param  pClusteredChars    The cluster contains many characters.
          \param  pImageWidth    The width of input image in pixel.
          \param  pIndex    The index of the array in the vector.
          \return    An structure of the cluster features.
  */
  auto
  fill_cluster_features(_Inout_ std::vector<characters_struct> &pClusteredChar,
                        _In_ int pImageWidth, _In_ int pIndex)
      -> cluster_features;

  /*!
          The check_twin_clusters function checks the input cluster features and
     decides whether two input clusters are twins.

          \param  pFirstInput    The first cluster features.
          \param  pSecondInput    The second cluster features.
          \param  pThreshold    The threshold for decision-making.
          \return    The result would be true if the input clusters are twins.
  */
  auto check_twin_clusters(_In_ cluster_features &pFirstInput,
                           _In_ cluster_features &pSecondInput,
                           _In_ float pThreshold) -> bool;

  /*!
          The keep_twins function eliminates the single clusters.

          \param  pClusteredChars    The cluster contains many characters.
          \param  pImageWidth    The width of input image in pixel.
          \param	pWord    The value would be "true" if it is a word
     cluster. \return
  */
  auto keep_twins(
      _Inout_ std::vector<std::vector<characters_struct>> &pClusteredChar,
      _In_ int pImageWidth, _In_ int pImageHeight, _In_ bool pWord) -> void;

  /*!
          The keep_time function keeps the time cluster.

          \param  pClusteredChars    The cluster contains many characters.
          \return
  */
  auto
  keep_time(_Inout_ std::vector<std::vector<characters_struct>> &pClusteredChar)
      -> void;

  /*!
          The add_text_to_original_image adds texts to the input image.

          \param  pImage    The input image.
          \param  pClusteredChars    The cluster contains many characters.
          \return
  */
  auto add_text_to_original_image(
      _Inout_ cv::Mat &pImage,
      _In_ std::vector<characters_struct> &pClusteredChar) -> void;

private:
  /*!<An object of tesseract library (to recognize digits)..*/
  tesseract::TessBaseAPI *digit_api = new tesseract::TessBaseAPI();
  /*!<An object of tesseract library (to recognize words).*/
  tesseract::TessBaseAPI *word_api = new tesseract::TessBaseAPI();
};
} // namespace wolf::ml::ocr
