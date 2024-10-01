#pragma once

#include <cstdarg>
#include <inttypes.h>
#include <filesystem>
#include <chrono>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

class TimeHelper
{
#ifdef _WIN32

	static std::time_t to_time_t(const std::filesystem::file_time_type file_time);
	static std::filesystem::file_time_type to_FileTimeType(const std::time_t time);

#else

	static std::time_t to_time_t(const std::filesystem::file_time_type file_time);
	static std::filesystem::file_time_type TimeHelper::to_FileTimeType(const std::time_t time);

#endif
};
