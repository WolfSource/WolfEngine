#ifndef OCR_REFEREE_CLASS_H
#define OCR_REFEREE_CLASS_H

#include <iostream>
#include <opencv2/opencv.hpp>

#include "dlib_export.h"
#include "ocr_engine.h"
#include "read_video_frames.h"

//! OCR referee class.
/*! \brief This class is a bridge between the OCR class and the referee task.

	This class contains functions, structures, and variables that use the OCR class
	for doing referee purposes.
*/
class ocr_referee_class
{
  public:
	//! The result_type enum shows the state of the result.
	/*!
		The result of the game would be stored in the database based on this enum
		and each stat has own properties.
	*/
	enum result_type
	{
		first_half,
		second_half,
		extra_first_half,
		extra_second_half,
		penalty,
		final_result
	};

	//! A structure for the frame result.
	/*!
	  The structure will be filled after the end of each frame processing and contain the frame necessary results.
	*/
	struct frame_result_struct
	{
		/*!<The home team scores(left).*/
		OCR::character_and_center home_result;
		/*!<The away team scores.*/
		OCR::character_and_center away_result;
		/*!<The home team name.*/
		OCR::character_and_center home_name;
		/*!<The away team name.*/
		OCR::character_and_center away_name;
		/*!<The home team penalty scores(left).*/
		OCR::character_and_center home_penalty_result;
		/*!<The away team penalty scores.*/
		OCR::character_and_center away_penalty_result;
		/*!<The frame number of the result image.*/
		int frame_number;
		/*!<The stat of the game.*/
		std::string stat;
		/*!<The operator is added to the struct to make it sortable.*/
		bool operator<(const frame_result_struct& rhs) const
		{
			return frame_number < rhs.frame_number;
		}
	};

	//! A structure for the match result.
	/*!
	  The structure will be filled after the end of each game processing and contain the match necessary results.
	*/
	struct match_result_struct
	{
		/*!<The results of frames share the same stat.*/
		std::vector<frame_result_struct> all_frames_results;
		/*!<The home team scores(left).*/
		OCR::character_and_center home_result;
		/*!<The away team scores.*/
		OCR::character_and_center away_result;
		/*!<The home team name.*/
		OCR::character_and_center home_name;
		/*!<The away team name.*/
		OCR::character_and_center away_name;
		/*!<The home team penalty scores(left).*/
		OCR::character_and_center home_penalty_result;
		/*!<The away team penalty scores.*/
		OCR::character_and_center away_penalty_result;

		/*!<The frame number.*/
		int frame_number;
		/*!<The stat of the game.*/
		std::string stat;
		/*!<This image is related to the extracted result.*/
		cv::Mat result_image;

		/*!<The ready variable shows the match result is ready.*/
		bool ready = false;
		/*!<The ready variable shows the match result is extracted..*/
		bool extracted = false;
		/*!<The ready variable shows the match result is applied.*/
		bool applied = false;

	  public:
		/*!
		  Release the memory.

		  This function frees the allocated memory.
		*/
		void release();
	};

	/*!
		The constructor of the class.

		The function is empty.
	  */
	W_SYSTEM_EXPORT ocr_referee_class();

  public:
	//! Contain the text and its repetition in the results.
	/*!
	  The final result is extracted from a series of results. This struct contains its vote throw the process.
	*/
	struct vote_over_string_vector
	{
		/*!<The candidate text.*/
		std::string str;
		/*!<The center coordination of the characters.*/
		cv::Point center;
		/*!<The text vote number.*/
		int already_voted = 1;
		/*!<The operator is added to the struct to make it sortable.*/
		bool operator<(const vote_over_string_vector& rhs) const
		{
			return already_voted < rhs.already_voted;
		}
	};

	//! Start and End of each match.
	/*!
	  The input video may contain multiple matches. It is a way to track the start and end frame of each game.
	*/
	struct related_results
	{
		/*!<The frame number of the first image of the game.*/
		int start;
		/*!<The frame number of the last image of the game.*/
		int end;
		/*!<The operator is added to the struct to make it sortable.*/
		bool operator<(const related_results& rhs) const { return start < rhs.start; }
	};
	/*!<This config helps to avoid processing of the frames which does not contain the game result.*/
	OCR::config_struct if_extract_game_result;
	/*!<The config for part of the image contains the score of the home team.*/
	OCR::config_struct extract_game_result_home;
	/*!<The config for part of the image contains the name of the home team.*/
	OCR::config_struct extract_home_name;
	/*!<The config for part of the image contains the score of the away team.*/
	OCR::config_struct extract_game_result_away;
	/*!<The config for part of the image contains the name of the away team.*/
	OCR::config_struct extract_away_name;
	/*!
		The function concatenates the separated alphabet of the team name.

		\param result A vector of alphabets.
		\return return the whole word of the team name.
	*/
	OCR::character_and_center concatenate_name_result(
		std::vector<OCR::character_and_center>& result);

	/*!
		The function updates the vector of voting.

		First, it checks if the input string is in the vector. If true, then the voting value would be incremented. Otherwise, a new variable would be added to the vector.

		\param the_character the input string and the related information related to the string.
		\param the_vector vector of string with theirs repetition number.
		\return is the string in the vector?
	*/
	W_SYSTEM_EXPORT auto if_the_string_is_in_the_vector(
		OCR::character_and_center the_character,
		std::vector<vote_over_string_vector>& the_vector) -> bool;

	/*!
		The voting function.

		\param voted_results The voted results.
		\param all_results All results.
	*/
	W_SYSTEM_EXPORT void voting_over_results_and_names(
		frame_result_struct& voted_results,
		std::vector<frame_result_struct>& all_results);
};

#endif	// OCR_REFEREE_CLASS_H
