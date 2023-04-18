/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

// #include "salieri.h"

#include "wolf.hpp"
#include <opencv2/opencv.hpp>

namespace wolf::ml {

/*!
        The function appends the input as input content to the end of the text file.

        \param pFilePath the file path.
        \param pContent the input text content.
        \return (void)
*/
W_API auto write_in_file_append(_In_ std::string pFilePath, _In_ std::string pContent) -> void;

/*!
        The function gets a string as input and return the boolean representation of the input.

        \param pVariable the string input.
        \return the boolean representain of the input string.
*/
W_API auto string_2_boolean(_In_ std::string pVariable) -> bool;

W_API auto split_string(_In_ std::string input_string, _In_ char reference)
    -> std::vector<std::string>;

/*!
        The function reads all lines of the input file and returns them in a string vector.

        \param pFilePath The path of the input file.
        \return a vector of strings.
*/
W_API auto read_text_file_line_by_line(_In_ std::string pFilePath) -> std::vector<std::string>;

/*!
        The .env file may have empty or commented lines. The is_line_contains_variable functions
        use to detect these lines.

        \param pStr The input string.
        \return False, if the input string contains # or is empty.
*/
W_API auto is_line_contains_variable(const std::string pStr) -> bool;

/*!
        The function reads environment variables from the .env file and set them in the environment
        by using the putenv function.

        \param pDotEnvFilePath The path of the .env file.
        \return
*/
W_API auto set_env(_In_ const char* pDotEnvFilePath) -> void;

/*!
        The function return the value of an environment variable based on the input key.

        \param pKey The path of the .env file.
        \return the value of the variable in int.
*/
W_API auto get_env_int(_In_ const char* pKey) -> int;

/*!
        The function return the value of an environment variable based on the input key.

        \param pKey The path of the .env file.
        \return the value of the variable in float.
*/
W_API auto get_env_float(_In_ const char* pKey) -> float;

/*!
        The function return the value of an environment variable based on the input key.

        \param pKey The path of the .env file.
        \return the value of the variable in boolean.
*/
W_API auto get_env_boolean(_In_ const char* pKey) -> bool;

/*!
        The function return the value of an environment variable based on the input key.

        \param pKey The path of the .env file.
        \return the value of the variable in string.
*/
W_API auto get_env_string(_In_ const char* pKey) -> std::string;

}  // namespace wolf::ml
