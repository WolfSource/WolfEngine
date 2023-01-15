#include "utilities.h"

#include <catch2/catch_all.hpp>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

fs::path utilities_asset_path = "../../../ocr/common_test_asset/utilities";

TEST_CASE("get_value_from_json_file_by_key gives value by key", "[utilities_test.cpp]")
{
	fs::path json_file_path = utilities_asset_path / "get_value_from_json_file_by_key.json";
	std::string key = "cpp-test-json";
	std::string value = get_value_from_json_file_by_key(json_file_path.string(), key);
	REQUIRE(value.compare("ready") == 0);
}

TEST_CASE("string_2_boolean on capitalized characters", "[utilities_test.cpp]")
{
	std::vector<std::string> list_string;
	list_string.push_back("true");
	list_string.push_back("FalSe");
	bool temp;
	temp = string_2_boolean(list_string[0]);
	REQUIRE(temp);
	temp = string_2_boolean(list_string[1]);
	REQUIRE(!temp);
}

TEST_CASE("line_of_numbers_in_string_to_vector_of_integers function", "[utilities_test.cpp]")
{
	std::string temp = "100,200,300,400";
	std::vector<int> integer_list = line_of_numbers_in_string_to_vector_of_integers(temp);
	REQUIRE(integer_list[0] == 100);
	REQUIRE(integer_list[1] == 200);
	REQUIRE(integer_list[2] == 300);
	REQUIRE(integer_list[3] == 400);
}

TEST_CASE("store_image_in_folder function", "[utilities_test.cpp]")
{
	std::vector<ocr_referee_class::match_result_struct> temp_video_result;
	ocr_referee_class::match_result_struct temp;
	ocr_referee_class::frame_result_struct frame_result;

	frame_result.home_name.text = "Home";
	frame_result.home_result.text = "2";
	frame_result.away_name.text = "Away";
	frame_result.away_result.text = "1";
	frame_result.frame_number = 3;
	frame_result.stat = "first_half";
	int height = 50;
	int width = 50;
	temp.result_image = cv::Mat::zeros(height, width, CV_8UC3);
	temp.all_frames_results.push_back(frame_result);
	temp_video_result.push_back(temp);

	std::string output_folder_path = ".";
	fs::path temp_video_path = "../test/123.mp4";
	store_image_in_folder(temp_video_result, output_folder_path, temp_video_path.string());

	fs::path file_path = "./123_0.png";

	bool is_file_exist = false;
	if (std::filesystem::remove(file_path.string()))
	{
		is_file_exist = true;
	}
	REQUIRE(is_file_exist);
}

TEST_CASE("write_results_in_file function", "[utilities_test.cpp]")
{
	std::vector<ocr_referee_class::match_result_struct> temp_video_result;
	ocr_referee_class::match_result_struct temp;
	ocr_referee_class::frame_result_struct frame_result;

	frame_result.home_name.text = "Home";
	frame_result.home_result.text = "2";
	frame_result.away_name.text = "Away";
	frame_result.away_result.text = "1";
	frame_result.frame_number = 3;
	frame_result.stat = "first_half";
	int height = 50;
	int width = 50;
	temp.result_image = cv::Mat::zeros(height, width, CV_8UC3);
	temp.all_frames_results.push_back(frame_result);
	temp_video_result.push_back(temp);

	fs::path temp_output_text_file = "./test_write_results_in_file.txt";

	write_results_in_file(temp_video_result, temp_output_text_file.string());

	std::vector<std::string> lines = read_text_file_line_by_line(temp_output_text_file.string());

	if (!std::filesystem::remove(temp_output_text_file.string()))
	{
		REQUIRE(false);
	}

	REQUIRE(lines[0].compare(temp_video_result[0].stat + "," + temp_video_result[0].home_name.text + "," + temp_video_result[0].home_result.text + "," + temp_video_result[0].away_name.text + "," + temp_video_result[0].away_result.text + "," + std::to_string(temp_video_result[0].frame_number)) == 0);
}

TEST_CASE("read_text_file_line_by_line function", "[utilities_test.cpp]")
{
	fs::path file_path = "./test_read_text_file_line_by_line.txt";
	std::string content = "This is a test!";

	write_in_file_append(file_path.string(), content);
	std::vector<std::string> lines = read_text_file_line_by_line(file_path.string());

	if (!std::filesystem::remove(file_path.string()))
	{
		REQUIRE(false);
	}

	REQUIRE(lines[0].compare(content) == 0);
}

TEST_CASE("normalized_levenshtein_similarity on one empty argument", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("", "text");
	REQUIRE(similarity == 0);
}

TEST_CASE("normalized_levenshtein_similarity on two empty argument", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("", "");
	REQUIRE(similarity == 0);
}

TEST_CASE("normalized_levenshtein_similarity on two equal string", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("test", "test");
	REQUIRE(similarity == 1);
}

TEST_CASE("normalized_levenshtein_similarity with similarity 0.5 on strings", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("test", "mast");
	REQUIRE(similarity == 0.5);
}

TEST_CASE("normalized_levenshtein_similarity on same numbers", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("1234", "1234");
	REQUIRE(similarity == 1);
}

TEST_CASE("normalized_levenshtein_similarity on strings with different length", "[utilities_test.cpp]")
{
	float similarity = normalized_levenshtein_similarity("test", "tst");
	REQUIRE(similarity == 0.75);
}

TEST_CASE("get_nearest_string with threshold 0.5 returns similar string", "[utilities_test.cpp]")
{
	fs::path similar_strings_path = utilities_asset_path / ".get_nearest_string_0_5";

	set_env(similar_strings_path.string().c_str());

	std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
	std::string output = get_nearest_string("tst1", path);
	REQUIRE(output.compare("test1") == 0);
}

TEST_CASE("get_nearest_string with threshold 0.9 returns input", "[utilities_test.cpp]")
{
	fs::path similar_strings_path = utilities_asset_path / ".get_nearest_string_0_9";

	set_env(similar_strings_path.string().c_str());

	std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
	std::string output = get_nearest_string("tst1", path);
	REQUIRE(output.compare("tst1") == 0);
}

TEST_CASE("get_nearest_string on empty string returns empty string", "[utilities_test.cpp]")
{
	fs::path similar_strings_path = utilities_asset_path / ".get_nearest_string_0_5";

	set_env(similar_strings_path.string().c_str());

	std::string path = get_env_string("SIMILAR_STRINGS_FILE_PATH");
	std::string output = get_nearest_string("", path);
	REQUIRE(output.compare("") == 0);
}

TEST_CASE("replace_string first phrase exists in text", "[utilities_test.cpp]")
{
	std::string text = "hello hamed";
	replace_string(text, "hamed", "bagher");
	REQUIRE(text.compare("hello bagher") == 0);
}

TEST_CASE("replace_string returns text if first phrase not exists in text", "[utilities_test.cpp]")
{
	std::string text = "hello hamed";
	replace_string(text, "shahoo", "bagher");
	REQUIRE(text.compare("hello hamed") == 0);
}

TEST_CASE("replace_string returns empty text if input text was empty", "[utilities_test.cpp]")
{
	std::string text = "";
	replace_string(text, "shahoo", "bagher");
	REQUIRE(text.compare("") == 0);
}

TEST_CASE("is_line_contains_variable function find empty line", "[utilities_test.cpp]")
{
	const std::string content = "";

	REQUIRE(!is_line_contains_variable(content));
}

TEST_CASE("is_line_contains_variable function find line started by #", "[utilities_test.cpp]")
{
	const std::string content = "# This is a commented line!";

	REQUIRE(!is_line_contains_variable(content));
}

TEST_CASE("set_env function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".set_env";

	set_env(dot_env_file_path.string().c_str());

	if (const char* env_p1 = getenv("V1"))
	{
		std::string temp(env_p1);
		REQUIRE(temp.compare("v1") == 0);
	}
	if (const char* env_p2 = getenv("V2"))
	{
		std::string temp(env_p2);
		REQUIRE(temp.compare("v2") == 0);
	}
	if (const char* env_p3 = getenv("V3"))
	{
		std::string temp(env_p3);
		REQUIRE(temp.compare("v3") == 0);
	}
	if (const char* env_p4 = getenv("V4"))
	{
		std::string temp(env_p4);
		REQUIRE(temp.compare("v4") == 0);
	}
}

TEST_CASE("get_env_int function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".get_env_int";

	set_env(dot_env_file_path.string().c_str());

	std::string key = "INT_VALUE";
	int value = get_env_int(key.c_str());

	REQUIRE(value == 7);
}

TEST_CASE("get_env_float function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".get_env_float";

	set_env(dot_env_file_path.string().c_str());

	std::string key = "FLOAT_VALUE";
	float value = get_env_float(key.c_str());

	REQUIRE(value == 4.5);
}

TEST_CASE("get_env_boolean function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".get_env_boolean";

	set_env(dot_env_file_path.string().c_str());

	std::string key = "BOOLEAN_VALUE";
	bool value = get_env_boolean(key.c_str());

	REQUIRE(value);
}

TEST_CASE("get_env_string function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".get_env_string";

	set_env(dot_env_file_path.string().c_str());

	std::string key = "STRING_VALUE";
	std::string value = get_env_string(key.c_str());

	REQUIRE(value.compare("this is a test! == 0"));
}

TEST_CASE("get_env_cv_rect function", "[utilities_test.cpp]")
{
	fs::path dot_env_file_path = utilities_asset_path / ".get_env_cv_rect";

	set_env(dot_env_file_path.string().c_str());

	std::string key = "CV_RECT_VALUE";
	cv::Rect value = get_env_cv_rect(key.c_str());

	REQUIRE(value.x == 313);
	REQUIRE(value.y == 110);
	REQUIRE(value.width == 72);
	REQUIRE(value.height == 14);
}

TEST_CASE("get_relative_path_to_root function", "[utilities_test.cpp]")
{
	std::string temp = get_relative_path_to_root();

	bool result = (temp.compare("../") == 0 || temp.compare("../../../") == 0);

	REQUIRE(result);
}
