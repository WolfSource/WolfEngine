/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <fstream>
#include <iostream>
#include <map>

#include "salieri.h"
#include "w_image_processor.hpp"
#include "w_ocr_engine.hpp"
#include "w_referee.hpp"
#include "wolf.hpp"

typedef void ocr_callback(char *result_buffer, int result_buffer_size,
                          uint8_t *image_buffer, int image_width,
                          int image_height);

namespace wolf::ml::ocr {

//! w_soccer class.
/*! \brief It is extract the soccer game result.

        This class contains functions, structures, and variables that use the
   w_soccer class for doing referee purposes.
*/
class w_soccer : public w_referee {
public:
  /*!
          The constructor of the class.
          In the constructor the configs are set.

          \return
  */
  W_API w_soccer();

  /*!
          The deconstructor of the class.

          The function is empty.
  */
  W_API ~w_soccer();

  /*!
  The function return the config of desired image window.

          \param pType The type of box
          \return
  */
  W_API static auto set_config(_In_ char *pType) -> w_ocr_engine::config_struct;

  /*!
          The function set the stat map global variable by dotenvs information.
  */
  auto fill_stat_map() -> void;

  W_API auto set_config_for_ocr(_In_ char *pType) -> config_for_ocr_struct;

  /*!
          The function returns frame results, the results are extracted from
     pre-defined frame boxes.

          \param frame The input frame image.
          \param frame_data The frame result would be stored in the frame_date.
          \return Void
  */
  W_API auto extract_result_from_frame_boxes(
      _In_ cv::Mat &frame, _Inout_ frame_result_struct &frame_data) -> void;

  /*!
          The function returns frame results, the results are extracted based on
     the character clusters' symmetricity.

          \param frame The input frame image.
          \param frame_data The frame result would be stored in the frame_date.
          \return Void
  */
  W_API auto extract_result_based_on_clusters_symmetricity(
      _In_ cv::Mat &frame, _Inout_ frame_result_struct &frame_data) -> void;

  /*!
          The function checks for penalty results and if the scene contains the
     penalty results the results would be extracted.

          \param frame The input frame image.
          \param digits_candidates The cluster of digits characters.
          \param words_candidates The cluster of words characters.
          \param time_candidates The cluster of time stat related characters.
          \param frame_data The frame result would be stored in the frame_date.
          \return Void
  */
  W_API auto extract_penalty_result_symmetricity(
      _In_ cv::Mat &frame,
      _In_ std::vector<std::vector<w_ocr_engine::characters_struct>>
          digits_candidates,
      _In_ std::vector<std::vector<w_ocr_engine::characters_struct>>
          words_candidates,
      _In_ std::vector<std::vector<w_ocr_engine::characters_struct>>
          time_candidates,
      _Inout_ frame_result_struct &frame_data) -> void;

  /*!
  The function returns the game results, if the image contains the game final
  result.

          \param pRawImage The sequence of the input image pixels array in BGR
  format. \param height The image height. \param width The image width. \param
  pStr The image result. \return Void
  */
  W_API auto single_image_result_extraction(_In_ uint8_t *pRawImage,
                                            _In_ int height, _In_ int width,
                                            _In_ ocr_callback *callback) -> int;

  /*!
  The extract_all_image_char_clusters function returns the character cluster
  related to the frame result.

          \param pImage The input image.
          \param pDdigitsCandidates The character clusters of the result texts.
          \param pWordsCandidates The character clusters of the team name texts.
          \param pTimeCandidates The character cluster of the stat texts.
          \return Void
  */
  W_API auto extract_all_image_char_clusters(
      cv::Mat &pImage,
      std::vector<std::vector<w_ocr_engine::characters_struct>>
          &pDigitsCandidates,
      std::vector<std::vector<w_ocr_engine::characters_struct>>
          &pWordsCandidates,
      std::vector<std::vector<w_ocr_engine::characters_struct>>
          &pTimeCandidates) -> void;

  /*!
  replace team names stored in match_result_struct using string similarity
  algorithms.

          \param result input struct
          \return
  */
  W_API static auto replace_team_names_with_most_similar_string(
      _Inout_ std::vector<w_referee::match_result_struct> &result) -> void;

  /*!
  The initial_match_result_struct function fills a match_result_struct with the
  initial values.

          \param frame_data input struct
          \param image input struct
          \return An initialed variable of match data structure.
  */
  W_API auto
  initial_match_result_struct(w_referee::frame_result_struct frame_data,
                              cv::Mat &image) -> w_referee::match_result_struct;

  /*!
  The update_match_data function store frames data in the match_date variable.

          \param frame_data input struct
          \param image input struct
          \return
  */
  W_API auto update_match_data(_In_ w_referee::frame_result_struct frame_data,
                               _In_ cv::Mat &image) -> void;

  /*!
  The extract_game_results function extracts the game results from the
  match_data and stores the results in the match_data.

          \return
  */
  W_API auto extract_game_results() -> void;

  /*!
  The get_matches_data function returns the private match_data variable.

          \return The private match_data variable.
  */
  W_API auto get_matches_data() -> std::vector<w_referee::match_result_struct>;

  /*!
          The get_stat_map function returns the private stat_map variable.

          \return The private stat_map variable.
  */
  W_API auto get_stat_map() -> std::map<std::string, std::string>;

private:
  /*!<The number of frame.*/
  int frame_number = 0;
  /*!<The configuration of the screen_identity window.*/
  w_ocr_engine::config_struct screen_identity;
  /*!<The configuration of the result_home window.*/
  w_ocr_engine::config_struct result_home;
  /*!<The configuration of the result_away window.*/
  w_ocr_engine::config_struct result_away;
  /*!<The configuration of the name_home window.*/
  w_ocr_engine::config_struct name_home;
  /*!<The configuration of the name_away window.*/
  w_ocr_engine::config_struct name_away;
  /*!<The configuration of the platform_free.*/
  config_for_ocr_struct platform_free;
  /*!<The configuration of the penalty.*/
  config_for_ocr_struct penalty;

  /*!<The matches data contains the results of the games.*/
  std::vector<w_referee::match_result_struct> matches_data;

  /*!<Game stat like first-half, second-half, and ... .*/
  std::string stat_first_half;
  std::string stat_second_half;
  std::string stat_extra_first_half;
  std::string stat_extra_second_half;
  std::string stat_penalty;

  std::map<std::string, std::string> stat_map;

  std::string the_last_result_message = "";

  /*!<An object of w_ocr_engine class.*/
  w_ocr_engine ocr_object;
};
} // namespace wolf::ml::ocr