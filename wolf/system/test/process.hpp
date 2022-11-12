/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <gtest.hpp> 
#include <system/w_process.hpp>

TEST(process, current_exe_path)
{
	const wolf::system::w_leak_detector _detector = {};
	using w_process = wolf::system::w_process;

	auto _path = w_process::current_exe_path();
	EXPECT_EQ(_path.has_error(), false);
	EXPECT_EQ(_path.value().empty(), false);
}