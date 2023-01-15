#include "ocr_referee_class.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

#include "ocr_engine.h"

namespace fs = std::filesystem;
fs::path ocr_referee_class_path = "../../../ocr/common_test_asset/ocr_referee_class";

TEST_CASE("concatenate_name_result function", "[ocr_referee_class_test.cpp]")
{
	ocr_referee_class ocr_referee_object;

	std::vector<OCR::character_and_center> cluster_temp;

	OCR::character_and_center char_center_temp;
	char_center_temp.text = "T";
	char_center_temp.center = cv::Point(5, 20);
	cluster_temp.push_back(char_center_temp);
	char_center_temp.text = "e";
	char_center_temp.center = cv::Point(10, 20);
	cluster_temp.push_back(char_center_temp);
	char_center_temp.text = "s";
	char_center_temp.center = cv::Point(15, 20);
	cluster_temp.push_back(char_center_temp);
	char_center_temp.text = "t";
	char_center_temp.center = cv::Point(20, 20);
	cluster_temp.push_back(char_center_temp);

	OCR::character_and_center temp_result = ocr_referee_object.concatenate_name_result(cluster_temp);

	REQUIRE(temp_result.text.compare("T e s t") == 0);
}

TEST_CASE("if_the_string_is_in_the_vector function", "[ocr_referee_class_test.cpp]")
{
	ocr_referee_class ocr_referee_object;

	std::vector<ocr_referee_class::vote_over_string_vector> vote_vector_temp;
	ocr_referee_class::vote_over_string_vector vote_var_temp;
	vote_var_temp.already_voted = false;
	vote_var_temp.center = cv::Point(70, 77);
	vote_var_temp.str = "Test";
	vote_vector_temp.push_back(vote_var_temp);

	OCR::character_and_center char_temp;
	char_temp.center = cv::Point(70, 77);
	char_temp.text = "Test";

	REQUIRE(ocr_referee_object.if_the_string_is_in_the_vector(char_temp, vote_vector_temp));
}

TEST_CASE("voting_over_results_and_names function", "[ocr_referee_class_test.cpp]")
{
	ocr_referee_class ocr_referee_object;

	std::vector<ocr_referee_class::frame_result_struct> all_results_temp;
	ocr_referee_class::frame_result_struct frame_date_temp;

	frame_date_temp.frame_number = 110;
	frame_date_temp.away_name.text = "ENGLAND";
	frame_date_temp.away_result.text = "1";
	frame_date_temp.home_name.text = "IRAN";
	frame_date_temp.home_result.text = "2";
	frame_date_temp.stat = ocr_referee_class::final_result;
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	frame_date_temp.away_result.text = "3";
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);
	all_results_temp.push_back(frame_date_temp);

	ocr_referee_class::frame_result_struct frame_date_result;

	ocr_referee_object.voting_over_results_and_names(frame_date_result, all_results_temp);

	REQUIRE(frame_date_result.away_result.text.compare("1") == 0);
}
