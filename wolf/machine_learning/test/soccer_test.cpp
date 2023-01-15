#include "soccer.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <new>
#include <opencv2/opencv.hpp>

#include "ocr_engine.h"
#include "ocr_referee_class.h"
#include "read_video_frames.h"
#include "utilities.h"

namespace fs = std::filesystem;
fs::path soccer_asset_path = "../../../ocr/common_test_asset/soccer";

TEST_CASE("set_config function", "[soccer_test.cpp]")
{
	fs::path env_file_path = soccer_asset_path / ".set_config";

	set_env(env_file_path.string().c_str());

	char* type = new char[50];
	snprintf(type, 50, "SOCCER_SCREEN_IDENTITY");
	OCR::config_struct screen_identity = SOCCER::set_config(type);

	delete[] type;

	REQUIRE(!screen_identity.config_for_ocr.verbose);
	REQUIRE(screen_identity.config_for_ocr.restrictions.max_area == 105);
	REQUIRE(screen_identity.name.compare("window_name_test") == 0);
}

std::vector<ocr_referee_class::match_result_struct> video_result;
typedef void ocr_callback(char*, int, uint8_t*, int, int);

void ocr_result(char* pResultBuffer, int pResultBufferSize, uint8_t* pImageBuffer, int pImageWidth, int pImageHeight)
{
	ocr_referee_class::match_result_struct match_result_temp;
	std::string temp(pResultBuffer);
	std::vector<std::string> result_vec = split_string(temp, ',');

	match_result_temp.stat = result_vec[0];
	match_result_temp.home_name.text = result_vec[1];
	match_result_temp.home_result.text = result_vec[2];
	match_result_temp.away_name.text = result_vec[3];
	match_result_temp.away_result.text = result_vec[4];

	cv::Mat image = cv::Mat(pImageHeight, pImageWidth, CV_8UC3, pImageBuffer);

	match_result_temp.result_image = image;
	video_result.push_back(match_result_temp);
}

TEST_CASE("single_image_result_extraction function", "[soccer_test.cpp]")
{
	fs::path json_file_path = soccer_asset_path / ".single_image_result_extraction";
	fs::path video_path = soccer_asset_path / "single_image_result_extraction.mp4";

	set_env(json_file_path.string().c_str());

	ocr_callback* _callback = &ocr_result;

	SOCCER* referee_obj = new SOCCER();

	// This is an example of using the Project
	read_video_frames player(video_path.string());
	int number_of_frames = int(player.get_frame_amount());

	cv::Mat frame;
	for (int i = 0; i < number_of_frames; i++)
	{
		frame = player.read_video_frame_by_frame();
		if (frame.empty())
		{
			break;
		}

		char* res_str = new char[256];
		referee_obj->single_image_result_extraction(frame.data, frame.rows, frame.cols, _callback);
	}
	frame.release();

	std::cout << "Results: " << video_result[0].stat << " " << video_result[0].home_name.text << " " << video_result[0].away_name.text << " " << video_result[0].home_result.text << " " << video_result[0].away_result.text << std::endl;

	REQUIRE(video_result[0].stat.compare("90 00") == 0);
	REQUIRE(video_result[0].home_name.text.compare("ARSENAL") == 0);
	REQUIRE(video_result[0].away_name.text.compare("FC BAYERN") == 0);
	REQUIRE(video_result[0].home_result.text.compare("0") == 0);
	REQUIRE(video_result[0].away_result.text.compare("8") == 0);
}

TEST_CASE("fill_stat_map function", "[soccer_test.cpp]")
{
	fs::path json_file_path = soccer_asset_path / ".fill_stat_map";

	set_env(json_file_path.string().c_str());

	SOCCER referee_obj;

	std::map<std::string, std::string> stat_map = referee_obj.get_stat_map();

	REQUIRE(stat_map["45 00"].compare("first_half") == 0);
	REQUIRE(stat_map["90 00"].compare("second_half") == 0);
	// REQUIRE(referee_obj->stat_extra_first_half.compare("105 00") == 0);
	// REQUIRE(referee_obj->stat_extra_second_half.compare("120 00") == 0);
	// REQUIRE(referee_obj->stat_penalty.compare("penalty") == 0);
}

TEST_CASE("replace_team_names_with_most_similar_string finds similar team to REALMDRD", "[soccer_test.cpp]")
{
	std::vector<ocr_referee_class::match_result_struct> results;
	ocr_referee_class::match_result_struct result;

	fs::path similar_strings_path = soccer_asset_path / ".replace_team_names_with_most_similar_string";

	set_env(similar_strings_path.string().c_str());

	result.away_name.text = "REALMDRD";
	results.push_back(result);

	SOCCER::replace_team_names_with_most_similar_string(results);

	REQUIRE(results[0].away_name.text.compare("REAL MADRID") == 0);
}

TEST_CASE("initial_match_result_struct function", "[soccer_test.cpp]")
{
	fs::path json_file_path = soccer_asset_path / ".initial_match_result_struct";

	set_env(json_file_path.string().c_str());

	ocr_referee_class::frame_result_struct frame_data;
	frame_data.away_name.text = "IRAN";
	frame_data.away_result.text = "3";
	frame_data.home_name.text = "ENGLAND";
	frame_data.home_result.text = "3";
	frame_data.frame_number = 114;
	frame_data.stat = "first_half";
	cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

	SOCCER soccer_object;

	ocr_referee_class::match_result_struct match_struct_temp;
	match_struct_temp = soccer_object.initial_match_result_struct(frame_data, image);

	image.release();
	REQUIRE(match_struct_temp.all_frames_results[0].away_name.text.compare("IRAN") == 0);
	REQUIRE(match_struct_temp.stat.compare("first_half") == 0);
	REQUIRE(match_struct_temp.all_frames_results[0].away_result.text.compare("3") == 0);
}

TEST_CASE("update_match_data function", "[soccer_test.cpp]")
{
	fs::path json_file_path = soccer_asset_path / ".update_match_data";

	set_env(json_file_path.string().c_str());

	ocr_referee_class::frame_result_struct frame_data;
	frame_data.away_name.text = "IRAN";
	frame_data.away_result.text = "3";
	frame_data.home_name.text = "ENGLAND";
	frame_data.home_result.text = "3";
	frame_data.frame_number = 114;
	frame_data.stat = "first_half";
	cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

	SOCCER soccer_object;

	std::vector<ocr_referee_class::match_result_struct> match_data_temp;
	match_data_temp = soccer_object.get_matches_data();
	REQUIRE(match_data_temp.size() == 0);

	soccer_object.update_match_data(frame_data, image);
	frame_data.frame_number = 116;
	soccer_object.update_match_data(frame_data, image);
	frame_data.frame_number = 117;
	soccer_object.update_match_data(frame_data, image);

	match_data_temp = soccer_object.get_matches_data();
	REQUIRE(match_data_temp.size() == 1);

	frame_data.frame_number = 210;
	soccer_object.update_match_data(frame_data, image);

	match_data_temp = soccer_object.get_matches_data();
	REQUIRE(match_data_temp.size() == 2);
	REQUIRE(match_data_temp[1].all_frames_results[0].frame_number == 210);
}

TEST_CASE("get_matches_data function", "[soccer_test.cpp]")
{
	fs::path json_file_path = soccer_asset_path / ".update_match_data";

	set_env(json_file_path.string().c_str());

	ocr_referee_class::frame_result_struct frame_data;
	frame_data.away_name.text = "IRAN";
	frame_data.away_result.text = "3";
	frame_data.home_name.text = "ENGLAND";
	frame_data.home_result.text = "3";
	frame_data.frame_number = 114;
	frame_data.stat = "first_half";
	cv::Mat image = cv::Mat::zeros(50, 50, CV_8UC3);

	SOCCER soccer_object;

	std::vector<ocr_referee_class::match_result_struct> match_data_temp;
	soccer_object.update_match_data(frame_data, image);
	match_data_temp = soccer_object.get_matches_data();
	REQUIRE(match_data_temp.size() == 1);
	REQUIRE(match_data_temp[0].all_frames_results[0].frame_number == 114);
}

TEST_CASE("replace_team_names_with_most_similar_string finds similar team to empty string", "[soccer_test.cpp]")
{
	std::vector<ocr_referee_class::match_result_struct> results;
	ocr_referee_class::match_result_struct result;

	fs::path similar_strings_path = soccer_asset_path / ".replace_team_names_with_most_similar_string";

	set_env(similar_strings_path.string().c_str());

	result.away_name.text = "";
	results.push_back(result);

	SOCCER::replace_team_names_with_most_similar_string(results);

	REQUIRE(results[0].away_name.text.compare("") == 0);
}
