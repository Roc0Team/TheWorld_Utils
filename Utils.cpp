//#include "pch.h"

#define MYAPI   __declspec( dllexport )

//#define _THEWORLD_CLIENT

//#ifdef _THEWORLD_CLIENT
//	#include <Godot.hpp>
//	#include <ResourceLoader.hpp>
//	#include <File.hpp>
//#endif

#define NOMINMAX
#include <cstdio>
#include "Utils.h"
#include "Profiler.h"
#include "half.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <tgmath.h>
#include <filesystem>
#include <string>
#include <algorithm>
#include <functional>
#include <chrono>
#include <sstream>
#include <format>

#include <plog/Initializers/RollingFileInitializer.h>
//#include <Eigen/Dense>
#include <Rpc.h>
#include <cmath>
#include "FastNoiseLite.h"
		
//#define	MIN_PERCENT					0.2f
//#define	MAX_PERCENT					0.5f
////#define	MAX_STEP_MODIFIER_FACTOR	100.0f	// crescendo incrementa la curvatura
//#define	MAX_STEP_MODIFIER_FACTOR	0.0f	// crescendo incrementa la curvatura

namespace fs = std::filesystem;

namespace TheWorld_Utils
{
	std::string getModuleLoadPath(void)
	{
		char path[MAX_PATH];
		HMODULE hm = NULL;

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)getModuleLoadPath, &hm) == 0)
		{
			int ret = GetLastError();
			sprintf_s(path, sizeof(path), "GetModuleHandle failed trying to get DLL Load path, error = %d\n", ret);
			throw(std::exception((std::string(__FUNCTION__) + std::string(path)).c_str()));
		}
		if (GetModuleFileNameA(hm, path, sizeof(path)) == 0)
		{
			int ret = GetLastError();
			sprintf_s(path, sizeof(path), "GetModuleFileName failed trying to get DLL Load path, error = %d\n", ret);
			throw(std::exception((std::string(__FUNCTION__) + std::string(path)).c_str()));
		}

		int l = (int)strlen(path);
		for (int i = l; i >= 0; i--)
			if (path[i] == '\\')
			{
				path[i] = '\0';
				break;
			}

		std::string s = path;

		return s;
	}

	double deg_to_rad(double deg)
	{
		return (deg * TheWorld_Utils::kPi) / 180;
	}

	double rad_to_deg(double rad)
	{
		return (rad * 180) / TheWorld_Utils::kPi;
	}

	void to_lower_implace(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	}

	std::string to_lower(std::string s)
	{
		std::string ret = s;
		std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
		return ret;
	}

	inline std::wstring convert(const std::string& as)
	{
		// deal with trivial case of empty string
		if (as.empty())    return std::wstring();

		// determine required length of new string
		size_t reqLength = ::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0);

		// construct new string of required length
		std::wstring ret(reqLength, L'\0');

		// convert old string to new string
		::MultiByteToWideChar(CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length());

		// return new string ( compiler should optimize this away )
		return ret;
	}

	std::chrono::system_clock::time_point current_utc_time_point(void)
	{
		std::chrono::system_clock::time_point now_tp_utc = std::chrono::system_clock::now();		// UTC Time
		return now_tp_utc;
	}

	std::time_t current_utc_time_t(void)
	{
		std::chrono::system_clock::time_point now_tp_utc = current_utc_time_point();		// UTC Time
		//std::string now_str_utc = std::format("{}", now_tp_utc);							// DEBUG
		std::time_t now_time_t_utc = std::chrono::system_clock::to_time_t(now_tp_utc);
		return now_time_t_utc;
	}

	bool is_time_point_null(std::chrono::system_clock::time_point tp)
	{
		return (tp == std::chrono::system_clock::time_point());
	}

	std::chrono::system_clock::time_point last_write_time_point_utc(const std::string& path)
	{
		const std::filesystem::file_time_type fileTime_utc = std::filesystem::last_write_time(path);					// UTC Time
		//std::string fileTime_str1_utc = std::format("{}", fileTime_utc);					// DEBUG
		const auto systemTime_utc = std::chrono::clock_cast<std::chrono::system_clock>(fileTime_utc);
		return systemTime_utc;
	}

	std::time_t last_write_time_t_utc(const std::string& path)
	{
		const std::chrono::system_clock::time_point systemTime_utc = last_write_time_point_utc(path);
		std::time_t fileTime_time_t_utc = std::chrono::system_clock::to_time_t(systemTime_utc);
		return fileTime_time_t_utc;
	}

	std::string last_write_time_str_utc(const std::string& path)
	{
		// Read last modified time
		//char buffer[512];
		//std::tm tm;
		//std::time_t t = last_write_time(path);
		//localtime_s(&tm, &t);
		//std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
		//strcat_s(buffer, sizeof(buffer), ".0");
		//return buffer;	// es: 2023-09-04 19:33:24.7026392

		//std::chrono::system_clock::time_point fileTime_tp_utc = std::chrono::system_clock::from_time_t(last_write_time_t_utc(path));
		std::chrono::system_clock::time_point fileTime_tp_utc = last_write_time_point_utc(path);
		std::string fileTime_str_utc = std::format("{}", fileTime_tp_utc);
		return fileTime_str_utc;	// es: 2023-09-04 19:33:24.0000000
	}

	void last_write_time_point_utc(const std::string& path, const std::chrono::system_clock::time_point time)
	{
		const std::filesystem::file_time_type ftime = std::chrono::clock_cast<std::chrono::file_clock>(time);
		std::filesystem::last_write_time(path, ftime);
	}

	void last_write_time_t_utc(const std::string& path, const std::time_t time)
	{
		const std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
		const std::filesystem::file_time_type ftime = std::chrono::clock_cast<std::chrono::file_clock>(tp);
		std::filesystem::last_write_time(path, ftime);
	}

	void last_write_time_str_utc(const std::string& path, const std::string& time /* es: 2023-09-04 19:33:24.7026392 */)
	{
		const std::chrono::system_clock::time_point tp = str_time_to_time_point(time);
		last_write_time_point_utc(path, tp);
	}

	std::chrono::system_clock::time_point time_t_to_time_point(const std::time_t time)
	{
		const std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
		return tp;
	}

	std::time_t time_point_to_time_t(const std::chrono::system_clock::time_point time)
	{
		std::time_t t = std::chrono::system_clock::to_time_t(time);
		return t;
	}

	std::chrono::system_clock::time_point str_time_to_time_point(const std::string& time /* es: 2023-09-04 19:33:24.7026392 */)
	{
		std::chrono::system_clock::time_point tp;
		std::istringstream in{ time };
		in >> std::chrono::parse("%F %T", tp);
		//std::string tp_str = std::format("{}", tp);							// DEBUG
		return tp;
	}

	std::string time_point_to_str_time(std::chrono::system_clock::time_point time)		/* es: 2023-09-04 19:33:24.7026392 */
	{
		std::string fileTime_str_utc = std::format("{}", time);
		return fileTime_str_utc;	// es: 2023-09-04 19:33:24.0000000
	}

	std::time_t str_time_to_time_t(const std::string& time /* es: 2023-09-04 19:33:24.7026392 */)
	{
		std::time_t ret = 0;
		std::vector<std::string> v = TheWorld_Utils::Utils::split(time, '.');
		std::tm t = {};
		std::istringstream ss(v[0]);
		ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
		if (ss.fail())
			return ret;
		//ret = std::mktime(&t);	// wrong: convert to time_t according local time
		ret = _mkgmtime(&t);		// correct: correct to time_t ignoring local calendar (it assumes input time is in UTC format and mantains it)
		return ret;
	}

	std::string time_t_to_str_time(std::time_t time, bool onlyseconds)		/* es: 2023-09-04 19:33:24.7026392 */
	{
		//char buffer[512];
		//std::tm tm;
		//localtime_s(&tm, &time);
		//std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
		//return buffer;	// es: 2023-09-04 19:33:24

		std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);
		std::string str = std::format("{}", tp);
		if (onlyseconds)
		{
			std::vector<std::string> v = TheWorld_Utils::Utils::split(str, '.');
			str = v[0];
		}
		return str;
	}

	bool time_point_zero(std::chrono::system_clock::time_point tp)
	{
		if (tp.time_since_epoch() == std::chrono::system_clock::duration::zero())
			return true;
		else
			return false;
	}

	std::string milliseconds_to_hhhmmss(size_t ms)
	{
		size_t hours = ms / 3600000;					//3600000 milliseconds in an hour
		ms = ms - 3600000 * hours;
		size_t minutes = ms / 60000;					//60000 milliseconds in a minute
		ms = ms - 60000 * minutes;
		size_t seconds = ms / 1000;					//1000 milliseconds in a second
		size_t milliseconds = ms - 1000 * seconds;
		//std::stringstream buffer;	buffer << std::format("{:02d}", hours) << ":" << std::format("{:02d}", minutes) << ":" << std::format("{:02d}", seconds) << "." << std::format("{:03d}", milliseconds);
		//return buffer.str();
		return std::format("{:02d}:{:02d}:{:02d}.{:03d}", hours, minutes, seconds, milliseconds);
	}

	std::string quickAddThousandSeparators(std::string value, char thousandSep)
	{
		size_t len = value.length();
		size_t dlen = 3;

		while (len > dlen) {
			value.insert(len - dlen, 1, thousandSep);
			dlen += 4;
			len += 1;
		}
		return value;
	}

	// support negative values
	std::string addThousandSeparators(std::string value, char thousandSep, char decimalSep, char sourceDecimalSep)
	{
		size_t len = value.length();
		int negative = ((len && value[0] == '-') ? 1 : 0);
		size_t dpos = value.find_last_of(sourceDecimalSep);
		size_t dlen = 3 + (dpos == std::string::npos ? 0 : (len - dpos));

		if (dpos != std::string::npos && decimalSep != sourceDecimalSep) {
			value[dpos] = decimalSep;
		}

		while ((len - negative) > dlen) {
			value.insert(len - dlen, 1, thousandSep);
			dlen += 4;
			len += 1;
		}
		return value;
	}

	__declspec(dllexport) void padStrTo(std::string& str, const size_t num, const char paddingChar)
	{
		if (num > str.size())
			str.insert(0, num - str.size(), paddingChar);
	}

	template <typename T> __declspec(dllexport) std::string padValueTo(T val, const size_t num, const char paddingChar)
	{
		std::string str = std::to_string(val);
		padStrTo(str, num, paddingChar);
		return str;
	}

	std::string padIntValueTo(int val, const size_t num, const char paddingChar)
	{
		return padValueTo<int>(val, num, paddingChar);
	}

	MapTextureRectPos::MapTextureRectPos(void)
	{
		reset();
	}

	void MapTextureRectPos::reset(void)
	{
		m_level = 0;
		m_numQuadrantsPerSize = 0;
		m_quadrantGlobalSize = 0;
		m_quadrantWorldSize = 0;
		//m_globalSize = 0;
		//m_worldSize = 0;
		m_globalLowerPosX = 0;
		m_globalLowerPosY = 0;
		m_idxX = 0;
		m_idxY = 0;
		m_name.clear();
	}

	MapTextureRectPos::MapTextureRectPos(float globalPosX, float globalPosY, int32_t level, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize)
	{
		m_level = level;
		m_numQuadrantsPerSize = numQuadrantsPerSize;
		m_quadrantGlobalSize = quadrantGlobalSize;
		m_quadrantWorldSize = quadrantWorldSize;
		//m_globalSize = m_numQuadrantsPerSize * m_quadrantGlobalSize;
		//m_worldSize = m_numQuadrantsPerSize * m_quadrantWorldSize;
		m_globalLowerPosX = floor(globalPosX / getGlobalSize()) * getGlobalSize();
		m_globalLowerPosY = floor(globalPosY / getGlobalSize()) * getGlobalSize();
		m_idxX = (int32_t)(m_globalLowerPosX / getGlobalSize());
		m_idxY = (int32_t)(m_globalLowerPosY / getGlobalSize());
		internalSetName();
	}

	MapTextureRectPos::MapTextureRectPos(int32_t idxX, int32_t idxY, int32_t level, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize)
	{
		m_level = level;
		m_numQuadrantsPerSize = numQuadrantsPerSize;
		m_quadrantGlobalSize = quadrantGlobalSize;
		m_quadrantWorldSize = quadrantWorldSize;
		//m_globalSize = m_numQuadrantsPerSize * m_quadrantGlobalSize;
		//m_worldSize = m_numQuadrantsPerSize * m_quadrantWorldSize;
		m_idxX = idxX;
		m_idxY = idxY;
		m_globalLowerPosX = float(m_idxX * getGlobalSize());
		m_globalLowerPosY = float(m_idxY * getGlobalSize());
		internalSetName();
	}

	MapTextureRectPos::MapTextureRectPos(TheWorld_Utils::QuadrantPos& quadrantPos, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize)
	{
		m_numQuadrantsPerSize = numQuadrantsPerSize;
		m_quadrantGlobalSize = quadrantGlobalSize;
		m_quadrantWorldSize = quadrantWorldSize;

		float globalPosX, globalPosY;
		worldPosToGlobalPos(quadrantPos.getLowerXGridVertex(), quadrantPos.getLowerZGridVertex(), globalPosX, globalPosY);

		*this = MapTextureRectPos(globalPosX, globalPosY, quadrantPos.getLevel(), numQuadrantsPerSize, quadrantGlobalSize, quadrantWorldSize);
	}

	MapTextureRectPos MapTextureRectPos::getFromQuadrant(TheWorld_Utils::QuadrantPos& quadrantPos, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize)
	{
		MapTextureRectPos pos(quadrantPos, numQuadrantsPerSize, quadrantGlobalSize, quadrantWorldSize);
		return pos;
	}

	void MapTextureRectPos::internalSetName(void)
	{
		constexpr size_t NUM_PADDING_ZEROES = 12;
		std::string strPosX, strPosY, strLevel;
		int32_t posX = int32_t(m_globalLowerPosX), posY = int32_t(m_globalLowerPosY), level = int32_t(m_level);
		if (posX < 0)
			strPosX = "-" + TheWorld_Utils::padIntValueTo(-posX, NUM_PADDING_ZEROES);
		else
			strPosX = TheWorld_Utils::padIntValueTo(posX, NUM_PADDING_ZEROES);
		if (posY < 0)
			strPosY = "-" + TheWorld_Utils::padIntValueTo(-posY, NUM_PADDING_ZEROES);
		else
			strPosY = TheWorld_Utils::padIntValueTo(posY, NUM_PADDING_ZEROES);
		if (level < 0)
			strLevel = "-" + TheWorld_Utils::padIntValueTo(-level, 2);
		else
			strLevel = TheWorld_Utils::padIntValueTo(level, 2);
		m_name = "MapRect_" + strLevel + "_" + strPosX + "_" + strPosY;
	}

	bool MapTextureRectPos::operator==(const MapTextureRectPos& textureRectPos) const
	{
		if (m_globalLowerPosX == textureRectPos.m_globalLowerPosX
			and m_globalLowerPosY == textureRectPos.m_globalLowerPosY
			and m_numQuadrantsPerSize == textureRectPos.m_numQuadrantsPerSize
			and m_quadrantGlobalSize == textureRectPos.m_quadrantGlobalSize
			and m_quadrantWorldSize == textureRectPos.m_quadrantWorldSize
			and m_level == textureRectPos.m_level)
			return true;
		else
			return false;
	}
	void MapTextureRectPos::operator=(const MapTextureRectPos& textureRectPos)
	{
		m_level = textureRectPos.m_level;
		m_numQuadrantsPerSize = textureRectPos.m_numQuadrantsPerSize;
		m_quadrantGlobalSize = textureRectPos.m_quadrantGlobalSize;
		m_quadrantWorldSize = textureRectPos.m_quadrantWorldSize;
		//m_globalSize = textureRectPos.m_globalSize;
		//m_worldSize = textureRectPos.m_worldSize;
		m_idxX = textureRectPos.m_idxX;
		m_idxY = textureRectPos.m_idxY;
		m_globalLowerPosX = textureRectPos.m_globalLowerPosX;
		m_globalLowerPosY = textureRectPos.m_globalLowerPosY;
		m_name = textureRectPos.m_name;
	}
	MapTextureRectPos MapTextureRectPos::operator+(const MapTextureRectPos& textureRectPos) const
	{
		MapTextureRectPos ret(m_idxX + textureRectPos.m_idxX, m_idxY + textureRectPos.m_idxY, textureRectPos.m_level, textureRectPos.m_numQuadrantsPerSize, textureRectPos.m_quadrantGlobalSize, textureRectPos.m_quadrantWorldSize);
		return ret;
	}

	bool MapTextureRectPos::operator<(const MapTextureRectPos& textureRectPos) const
	{
		assert(initialized()
			and m_numQuadrantsPerSize == textureRectPos.m_numQuadrantsPerSize
			and m_quadrantGlobalSize == textureRectPos.m_quadrantGlobalSize
			and m_quadrantWorldSize == textureRectPos.m_quadrantWorldSize);

		if (m_level < textureRectPos.m_level)
			return true;
		if (m_level > textureRectPos.m_level)
			return false;

		if (m_globalLowerPosY < textureRectPos.m_globalLowerPosY)
			return true;
		if (m_globalLowerPosY > textureRectPos.m_globalLowerPosY)
			return false;

		return m_globalLowerPosX < textureRectPos.m_globalLowerPosX;
	}

	bool MapTextureRectPos::initialized(void) const
	{
		if (m_numQuadrantsPerSize > 0 and m_quadrantGlobalSize > 0 and m_quadrantWorldSize > 0)
			return true;
		else
			return false;
	}

	void MapTextureRectPos::globalPosToWorldPos(float globalPosX, float globalPosY, float& worldPosX, float& worldPosY) const
	{
		worldPosX = (globalPosX / getQuadrantGlobalSize()) * getQuadrantWorldSize();
		worldPosY = (globalPosY / getQuadrantGlobalSize()) * getQuadrantWorldSize();
	}

	void MapTextureRectPos::worldPosToGlobalPos(float worldPosX, float worldPosY, float& globalPosX, float& globalPosY) const
	{
		globalPosX = (worldPosX / getQuadrantWorldSize()) * getQuadrantGlobalSize();
		globalPosY = (worldPosY / getQuadrantWorldSize()) * getQuadrantGlobalSize();
	}

	void MapTextureRectPos::getAllContainedQuadrantPos(std::vector<TheWorld_Utils::QuadrantPos>& quadrantsPos, int worldViewerLevel, int numVerticesPerQuadrantSize, float gridStepInWU) const
	{
		quadrantsPos.clear();

		for (int quadrantsIdxY = 0; quadrantsIdxY < getNumQuadrantsPerSize(); quadrantsIdxY++)
		{
			for (int quadrantsIdxX = 0; quadrantsIdxX < getNumQuadrantsPerSize(); quadrantsIdxX++)
			{
				float globalPosX = getGlobalLowerPosX() + quadrantsIdxX * getQuadrantGlobalSize();
				float globalPosY = getGlobalLowerPosY() + quadrantsIdxY * getQuadrantGlobalSize();
				float worldPosX, worldPosY;
				globalPosToWorldPos(globalPosX, globalPosY, worldPosX, worldPosY);
				TheWorld_Utils::QuadrantPos pos(worldPosX, worldPosY, worldViewerLevel, numVerticesPerQuadrantSize, gridStepInWU);
				quadrantsPos.push_back(pos);
			}
		}

	}

	bool MapTextureRectPos::getQuadrantRelativeIndices(const TheWorld_Utils::QuadrantPos& quadrantPos, int32_t& idxX, int32_t& idxY) const
	{
		float globalPosX, globalPosY;
		worldPosToGlobalPos(quadrantPos.getLowerXGridVertex(), quadrantPos.getLowerZGridVertex(), globalPosX, globalPosY);

		if (quadrantPos.getSizeInWU() != getQuadrantWorldSize())
			return false;
		
		// if quadrant not contained in rect we return false
		if (globalPosX < getGlobalLowerPosX() or globalPosX + getQuadrantGlobalSize() > getGlobalHigherPosX()
			or globalPosY < getGlobalLowerPosY() or globalPosY + getQuadrantGlobalSize() > getGlobalHigherPosY())
			return false;

		float offsetX = globalPosX - getGlobalLowerPosX();
		idxX = int32_t(offsetX / getQuadrantGlobalSize());
		float offsetY = globalPosY - getGlobalLowerPosY();
		idxY = int32_t(offsetY / getQuadrantGlobalSize());

		return true;
	}

	void MapTextureRectPos::serialize(TheWorld_Utils::MemoryBuffer& buffer) const
	{
		buffer.reserve(sizeof(MapTextureRectPos));

		size_t size;
		size_t cursor = 0;

		serializeToByteStream<float>(m_globalLowerPosX, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<float>(m_globalLowerPosY, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int32_t>(m_idxX, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int32_t>(m_idxY, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int32_t>(m_level, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int32_t>(m_numQuadrantsPerSize, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<float>(m_quadrantGlobalSize, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<float>(m_quadrantWorldSize, buffer.ptr() + cursor, size);
		cursor += size;
		//serializeToByteStream<float>(m_globalSize, buffer.ptr() + cursor, size);
		//cursor += size;
		//serializeToByteStream<float>(m_worldSize, buffer.ptr() + cursor, size);
		//cursor += size;

		buffer.adjustSize(cursor);
	}

	void MapTextureRectPos::deserialize(TheWorld_Utils::MemoryBuffer& buffer)
	{
		reset();

		size_t size;
		size_t cursor = 0;

		m_globalLowerPosX = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_globalLowerPosY = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_idxX = deserializeFromByteStream<int32_t>(buffer.ptr() + cursor, size);
		cursor += size;
		m_idxY = deserializeFromByteStream<int32_t>(buffer.ptr() + cursor, size);
		cursor += size;
		m_level = deserializeFromByteStream<int32_t>(buffer.ptr() + cursor, size);
		cursor += size;
		m_numQuadrantsPerSize = deserializeFromByteStream<int32_t>(buffer.ptr() + cursor, size);
		cursor += size;
		m_quadrantGlobalSize = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_quadrantWorldSize = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		//m_globalSize = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		//cursor += size;
		//m_worldSize = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		//cursor += size;

		internalSetName();
	}

	void QuadrantPos::serialize(TheWorld_Utils::MemoryBuffer& buffer) const
	{
		buffer.reserve(sizeof(QuadrantPos) + m_tag.length() + m_name.length());

		size_t size;
		size_t cursor = 0;

		serializeToByteStream<float>(m_lowerXGridVertex, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<float>(m_lowerZGridVertex, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int>(m_numVerticesPerSize, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<int>(m_level, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<float>(m_gridStepInWU, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<bool>(m_initialized, buffer.ptr() + cursor, size);
		cursor += size;
		serializeToByteStream<size_t>(m_tag.size(), buffer.ptr() + cursor, size);
		cursor += size;
		memcpy(buffer.ptr() + cursor, m_tag.c_str(), m_tag.size());
		cursor += m_tag.size();

		buffer.adjustSize(cursor);
	}

	void QuadrantPos::deserialize(TheWorld_Utils::MemoryBuffer& buffer)
	{
		reset();

		size_t size;
		size_t cursor = 0;

		m_lowerXGridVertex = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_lowerZGridVertex = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_numVerticesPerSize = deserializeFromByteStream<int>(buffer.ptr() + cursor, size);
		cursor += size;
		m_level = deserializeFromByteStream<int>(buffer.ptr() + cursor, size);
		cursor += size;
		m_gridStepInWU = deserializeFromByteStream<float>(buffer.ptr() + cursor, size);
		cursor += size;
		m_initialized = deserializeFromByteStream<bool>(buffer.ptr() + cursor, size);
		cursor += size;
		size_t m_tagSize = deserializeFromByteStream<size_t>(buffer.ptr() + cursor, size);
		cursor += size;
		m_tag = std::string((char*)buffer.ptr() + cursor, m_tagSize);
		cursor += m_tagSize;

		m_sizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
		m_centerX = m_lowerXGridVertex + (m_sizeInWU / 2);
		m_centerZ = m_lowerZGridVertex + (m_sizeInWU / 2);
		resetIndexPos();
		resetName();
	}

	void QuadrantPos::getNeighbours(std::vector<TheWorld_Utils::QuadrantPos>& neighbours, size_t maxDistance) const
	{
		if (maxDistance < 1)
			maxDistance = 1;

		std::map<TheWorld_Utils::QuadrantPos, bool> _neighbours;
		_neighbours[getQuadrantPos(DirectionSlot::NorthZMinus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::SouthZPlus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::EastXPlus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::WestXMinus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::NortheastZMinusXPlus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::NorthwestZMinusXMinus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::SoutheastZPlusXPlus)] = true;
		_neighbours[getQuadrantPos(DirectionSlot::SouthwestZPlusXMinus)] = true;

		if (maxDistance > 1)
		{
			std::vector<TheWorld_Utils::QuadrantPos> externalRingNeighbours;
			for (auto& item : _neighbours)
				externalRingNeighbours.push_back(item.first);


			for (int distance = 2; distance <= maxDistance; distance++)
			{
				// getting new neighbours: there are many duplicates
				std::vector<TheWorld_Utils::QuadrantPos> newNeighbours;
				for (auto& item : externalRingNeighbours)
					item.getNeighbours(newNeighbours, 1);

				// erasing duplicates and obtaining new external ring neighbours
				externalRingNeighbours.clear();
				for (auto& item : newNeighbours)
				{
					if (not _neighbours.contains(item))
					{
						_neighbours[item] = true;					// growing map of new pos
						externalRingNeighbours.push_back(item);		// because current pos is a new pos it is in the new external ring
					}
				}
			}

			// removing self if contained in found neighbours
			if (_neighbours.contains(*this))
				_neighbours.erase(*this);
		}

		// reversing found neighbours in output vector
		for (auto& item : _neighbours)
			neighbours.push_back(item.first);
	}

	QuadrantPos QuadrantPos::getQuadrantPos(enum class DirectionSlot dir, int numSlot) const
	{
		QuadrantPos pos = *this;

		switch (dir)
		{
		case DirectionSlot::XMinus:
		{
			pos.m_lowerXGridVertex -= (m_sizeInWU * numSlot);
		}
		break;
		case DirectionSlot::XPlus:
		{
			pos.m_lowerXGridVertex += (m_sizeInWU * numSlot);
		}
		break;
		case DirectionSlot::ZMinus:
		{
			pos.m_lowerZGridVertex -= (m_sizeInWU * numSlot);
		}
		break;
		case DirectionSlot::ZPlus:
		{
			pos.m_lowerZGridVertex += (m_sizeInWU * numSlot);
		}
		break;
		case DirectionSlot::ZMinusXMinus:
		{
			QuadrantPos pos1 = getQuadrantPos(DirectionSlot::ZMinus, numSlot);
			pos = pos1.getQuadrantPos(DirectionSlot::XMinus, numSlot);
		}
		break;
		case DirectionSlot::ZMinusXPlus:
		{
			QuadrantPos pos1 = getQuadrantPos(DirectionSlot::ZMinus, numSlot);
			pos = pos1.getQuadrantPos(DirectionSlot::XPlus, numSlot);
		}
		break;
		case DirectionSlot::ZPlusXMinus:
		{
			QuadrantPos pos1 = getQuadrantPos(DirectionSlot::ZPlus, numSlot);
			pos = pos1.getQuadrantPos(DirectionSlot::XMinus, numSlot);
		}
		break;
		case DirectionSlot::ZPlusXPlus:
		{
			QuadrantPos pos1 = getQuadrantPos(DirectionSlot::ZPlus, numSlot);
			pos = pos1.getQuadrantPos(DirectionSlot::XPlus, numSlot);
		}
		break;
		}

		pos.resetIndexPos();
		pos.resetName();

		return pos;
	}

	size_t QuadrantPos::distanceInPerimeter(QuadrantPos& q) const
	{
		size_t distanceOnX = (size_t)ceil(abs(q.getLowerXGridVertex() - getLowerXGridVertex()) / q.m_sizeInWU);
		size_t distanceOnZ = (size_t)ceil(abs(q.getLowerZGridVertex() - getLowerZGridVertex()) / q.m_sizeInWU);
		if (distanceOnX > distanceOnZ)
			return distanceOnX;
		else
			return distanceOnZ;
	}

	int QuadrantPos::distanceInPerimeter(QuadrantPos& fromQuadrant, QuadrantPos::DirectionSlot dir) const
	{
		int ret = -1;
		switch (dir)
		{
			case DirectionSlot::WestXMinus:
				{
					ret = (int)ceil((fromQuadrant.getLowerXGridVertex() - getLowerXGridVertex()) / fromQuadrant.m_sizeInWU);
				}
				break;
			case DirectionSlot::EastXPlus:
				{
					ret = (int)ceil((getLowerXGridVertex() - fromQuadrant.getLowerXGridVertex()) / fromQuadrant.m_sizeInWU);
				}
				break;
			case DirectionSlot::NorthZMinus:
				{
					ret = (int)ceil((fromQuadrant.getLowerZGridVertex() - getLowerZGridVertex()) / fromQuadrant.m_sizeInWU);
				}
				break;
			case DirectionSlot::SouthZPlus:
				{
					ret = (int)ceil((getLowerZGridVertex() - fromQuadrant.getLowerZGridVertex()) / fromQuadrant.m_sizeInWU);
				}
				break;
		}
		return ret;
	}

	void TerrainEdit::generateGroundImage(MemoryBuffer& albedoBumpImage, MemoryBuffer& normalRoughnessImage, std::string groundTypeName, size_t imageSize, bool flipY, MemoryBuffer& colorImage, MemoryBuffer& bumpImage, MemoryBuffer& normalImage, MemoryBuffer& roughImage)
	{
		size_t width, heigth;
		double d;

		// Color image formal controls
		d = sqrt(colorImage.size() / sizeof(struct TheWorld_Utils::_RGB));
		if (colorImage.size() == 0)
			throw(std::exception((std::string(__FUNCTION__) + std::string(" Color image empty")).c_str()));
		if (d * d * sizeof(struct TheWorld_Utils::_RGB) != colorImage.size())
			throw(std::exception((std::string(__FUNCTION__) + std::string(" Color image length not valid")).c_str()));
		width = heigth = (size_t)d;

		// Bump/Displ/Depth image formal controls
		if (bumpImage.size() > 0)
		{
			d = sqrt(bumpImage.size());
			if (d * d != bumpImage.size())
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Normal image length not valid")).c_str()));
			if ((size_t)d != width)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Normal image differs from color image size")).c_str()));
		}

		// Normal image formal controls
		if (normalImage.size() > 0)
		{
			d = sqrt(normalImage.size() / sizeof(struct TheWorld_Utils::_RGB));
			if (d * d * sizeof(struct TheWorld_Utils::_RGB) != normalImage.size())
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Normal image length not valid")).c_str()));
			if ((size_t)d != width)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Normal image differs from color image size")).c_str()));
		}

		// Rough image formal controls
		if (roughImage.size() > 0)
		{
			d = sqrt(roughImage.size());
			if (d * d != roughImage.size())
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Rough image length not valid")).c_str()));
			if ((size_t)d != width)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Rough image differs from color image size")).c_str()));
		}

		size_t bufferSize = width * heigth * sizeof(struct TheWorld_Utils::_RGBA);

		albedoBumpImage.clear(__FUNCTION__);
		albedoBumpImage.reserve(bufferSize);
		albedoBumpImage.adjustSize(bufferSize);
		struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)albedoBumpImage.ptr();
		struct TheWorld_Utils::_RGB* rgb = (struct TheWorld_Utils::_RGB*)colorImage.ptr();
		BYTE* a = bumpImage.size() > 0 ? bumpImage.ptr() : nullptr;
		for (size_t row = 0; row < heigth; row++)
			for (size_t col = 0; col < width; col++)
			{
				rgba->r = rgb->r;
				rgba->g = rgb->g;
				rgba->b = rgb->b;

				if (a != nullptr)
				{
					rgba->a = *a;
					a++;
				}
				else
					rgba->a = 255;

				rgba++;
				rgb++;
			}

		normalRoughnessImage.clear(__FUNCTION__);
		if (normalImage.size() > 0 || roughImage.size() > 0)
		{
			normalRoughnessImage.reserve(bufferSize);
			normalRoughnessImage.adjustSize(bufferSize);
			struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)normalRoughnessImage.ptr();
			struct TheWorld_Utils::_RGB* rgb = normalImage.size() > 0 ? (struct TheWorld_Utils::_RGB*)normalImage.ptr() : nullptr;
			BYTE* a = roughImage.size() > 0 ? roughImage.ptr() : nullptr;
			for (size_t row = 0; row < heigth; row++)
				for (size_t col = 0; col < width; col++)
				{
					if (rgb != nullptr)
					{
						rgba->r = rgb->r;
						if (flipY)
							rgba->g = 255 - rgb->g;
						else
							rgba->g = rgb->g;
						rgba->b = rgb->b;
						rgb++;
					}
					else
					{
						rgba->r = 127;
						rgba->g = 127;
						rgba->b = 255;
					}

					if (a != nullptr)
					{
						rgba->a = *a;
						a++;
					}
					else
						rgba->a = 255;

					rgba++;
				}
		}
	}

	void TerrainEdit::generateGroundImage(std::string outdir, std::string groundTypeName, size_t imageSize, bool flipY, MemoryBuffer& colorImage, MemoryBuffer& bumpImage, MemoryBuffer& normalImage, MemoryBuffer& roughImage)
	{
		MemoryBuffer albedoBumpImage;
		MemoryBuffer normalRoughnessImage;

		generateGroundImage(albedoBumpImage, normalRoughnessImage, groundTypeName, imageSize, flipY, colorImage, bumpImage, normalImage, roughImage);

		if (outdir.substr(outdir.size() - 1, 1) != "\\")
			outdir += "\\";
		FILE* outFile = nullptr;

		std::string albedoBumpImagePath = outdir + groundTypeName + "_albedo_bump.ground";
		errno_t err = fopen_s(&outFile, albedoBumpImagePath.c_str(), "wb");
		if (err != 0)
		{
			throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Open ") + albedoBumpImagePath.c_str() + " in errore - Err=" + std::to_string(err)).c_str()));
		}

		if (fwrite(albedoBumpImage.ptr(), albedoBumpImage.size(), 1, outFile) != 1)
		{
			fclose(outFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Error writing ") + albedoBumpImagePath.c_str()).c_str()));
		}

		fclose(outFile);

		std::string normalRoughnessImagePath = outdir + groundTypeName + "_normal_roughness.ground";
		err = fopen_s(&outFile, normalRoughnessImagePath.c_str(), "wb");
		if (err != 0)
		{
			throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Open ") + normalRoughnessImagePath.c_str() + " in errore - Err=" + std::to_string(err)).c_str()));
		}

		if (fwrite(normalRoughnessImage.ptr(), normalRoughnessImage.size(), 1, outFile) != 1)
		{
			fclose(outFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Error writing ") + normalRoughnessImagePath.c_str()).c_str()));
		}

		fclose(outFile);
	}

	TerrainEdit::TerrainEdit(enum class TerrainEdit::TerrainType terrainType)
	{
		size = sizeof(TerrainEdit);
		needUploadToServer = false;
		normalsNeedRegen = true;

		minHeight = 0;
		maxHeight = 0;

		eastSideXPlus.needBlend = true;
		eastSideXPlus.minHeight = 0.0f;
		eastSideXPlus.maxHeight = 0.0f;

		westSideXMinus.needBlend = true;
		westSideXMinus.minHeight = 0.0f;
		westSideXMinus.maxHeight = 0.0f;

		southSideZPlus.needBlend = true;
		southSideZPlus.minHeight = 0.0f;
		southSideZPlus.maxHeight = 0.0f;

		northSideZMinus.needBlend = true;
		northSideZMinus.minHeight = 0.0f;
		northSideZMinus.maxHeight = 0.0f;

		TerrainEdit::terrainType = terrainType;
		init(terrainType);

		//memset(extraValues.lowElevationTexName_r, 0, sizeof(extraValues.lowElevationTexName_r));
		//memset(extraValues.highElevationTexName_g, 0, sizeof(extraValues.highElevationTexName_g));
		//memset(extraValues.dirtTexName_b, 0, sizeof(extraValues.dirtTexName_b));
		//memset(extraValues.rocksTexName_a, 0, sizeof(extraValues.rocksTexName_a));
		m_splatmapNeedRegen = true;
		colormapNeedRegen = true;
		globalmapNeedRegen = true;

		memset(extraValues1.filler1, -1, sizeof(extraValues1.filler1));
		memset(extraValues1.filler2, '-', sizeof(extraValues1.filler2));
	}

	bool TerrainEdit::operator==(const TerrainEdit& o) const
	{
		if (size != o.size)
			return false;
		if (needUploadToServer != o.needUploadToServer)
			return false;
		if (normalsNeedRegen != o.normalsNeedRegen)
			return false;
		if (terrainType != o.terrainType)
			return false;
		if (minHeight != o.minHeight)
			return false;
		if (maxHeight != o.maxHeight)
			return false;
		if (!(eastSideXPlus == o.eastSideXPlus) )
			return false;
		if (!(westSideXMinus == o.westSideXMinus) )
			return false;
		if ( !(southSideZPlus == o.southSideZPlus) )
			return false;
		if ( !(northSideZMinus == o.northSideZMinus) )
			return false;
		if ( !(noise == o.noise) )
			return false;
		if (strlen(extraValues.lowElevationTexName_r) != strlen(o.extraValues.lowElevationTexName_r))
			return false;
		if (strcmp(extraValues.lowElevationTexName_r, o.extraValues.lowElevationTexName_r) != 0)
			return false;
		if (strlen(extraValues.highElevationTexName_g) != strlen(o.extraValues.highElevationTexName_g))
			return false;
		if (strcmp(extraValues.highElevationTexName_g, o.extraValues.highElevationTexName_g) != 0)
			return false;
		if (strlen(extraValues.dirtTexName_b) != strlen(o.extraValues.dirtTexName_b))
			return false;
		if (strcmp(extraValues.dirtTexName_b, o.extraValues.dirtTexName_b) != 0)
			return false;
		if (strlen(extraValues.rocksTexName_a) != strlen(o.extraValues.rocksTexName_a))
			return false;
		if (strcmp(extraValues.rocksTexName_a, o.extraValues.rocksTexName_a) != 0)
			return false;
		if (m_splatmapNeedRegen != o.m_splatmapNeedRegen)
			return false;
		if (colormapNeedRegen != o.colormapNeedRegen)
			return false;
		if (globalmapNeedRegen != o.globalmapNeedRegen)
			return false;
		if (extraValues.slopeVerticalFactor != o.extraValues.slopeVerticalFactor)
			return false;
		if (extraValues.slopeFlatFactor != o.extraValues.slopeFlatFactor)
			return false;
		if (extraValues.dirtOnRocksFactor != o.extraValues.dirtOnRocksFactor)
			return false;
		if (extraValues.lowElevationFactor != o.extraValues.lowElevationFactor)
			return false;
		if (extraValues.splatMapMode != o.extraValues.splatMapMode)
			return false;
		if (extraValues.amplitudeBlendFactor != o.extraValues.amplitudeBlendFactor)
			return false;
		if (extraValues.postGenerateElevation != o.extraValues.postGenerateElevation)
			return false;
		if (extraValues.ignoreDesideredMinHeight != o.extraValues.ignoreDesideredMinHeight)
			return false;
		if (extraValues.numBlendingCycles != o.extraValues.numBlendingCycles)
			return false;
		if (extraValues.blendingBendingFactor != o.extraValues.blendingBendingFactor)
			return false;
		if (extraValues.minRandomBendingExtension != o.extraValues.minRandomBendingExtension)
			return false;
		if (extraValues.maxRandomBendingExtension != o.extraValues.maxRandomBendingExtension)
			return false;
		if (heightsNeedRegen != o.heightsNeedRegen)
			return false;
		if (extraValues.seaInfoSet != o.extraValues.seaInfoSet)
			return false;
		if (extraValues.canContainSea != o.extraValues.canContainSea)
			return false;
		if (deprecated_eastNeighbourXPlus != o.deprecated_eastNeighbourXPlus)
			return false;
		if (deprecated_westNeighbourXMinus != o.deprecated_westNeighbourXMinus)
			return false;
		if (deprecated_southNeighbourZPlus != o.deprecated_southNeighbourZPlus)
			return false;
		if (deprecated_northNeighbourZMinus != o.deprecated_northNeighbourZMinus)
			return false;
		if (extraValues1.continentId != o.extraValues1.continentId)
			return false;
		return true;
	}

	std::string TerrainEdit::terrainTypeString(enum class TerrainEdit::TerrainType terrainType)
	{
		switch (terrainType)
		{
		case TerrainEdit::TerrainType::noise_1:
		{
			return "noise_1";
		}
		break;
		case TerrainEdit::TerrainType::flat:
		{
			return "flat";
		}
		break;
		case TerrainEdit::TerrainType::empty:
		{
			return "empty";
		}
		break;
		case TerrainEdit::TerrainType::campaign_1:
		{
			return "campaign_1";
		}
		break;
		case TerrainEdit::TerrainType::plateau_1:
		{
			return "plateau_1";
		}
		break;
		case TerrainEdit::TerrainType::low_hills:
		{
			return "low_hills";
		}
		break;
		case TerrainEdit::TerrainType::high_hills:
		{
			return "high_hills";
		}
		break;
		case TerrainEdit::TerrainType::low_mountains:
		{
			return "low_mountains";
		}
		break;
		case TerrainEdit::TerrainType::low_mountains_grow:
		{
			return "low_mountains_grow";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1:
		{
			return "high_mountains_1";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1_grow:
		{
			return "high_mountains_1_grow";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_2:
		{
			return "high_mountains_2";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_2_grow:
		{
			return "high_mountains_2_grow";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_3:
		{
			return "high_mountains_3";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_3_grow:
		{
			return "high_mountains_3_grow";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_4:
		{
			return "high_mountains_4";
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_4_grow:
		{
			return "high_mountains_4_grow";
		}
		break;
		case TerrainEdit::TerrainType::coastal:
		{
			return "coastal";
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_1:
		{
			return "sea_abyss_1";
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_2:
		{
			return "sea_abyss_2";
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_3:
		{
			return "sea_abyss_3";
		}
		break;
		case TerrainEdit::TerrainType::desert_flat_1:
		{
			return "desert_flat_1";
		}
		break;
		case TerrainEdit::TerrainType::desert_low_dunes:
		{
			return "desert_low_dunes";
		}
		break;
		case TerrainEdit::TerrainType::desert_low_mountains:
		{
			return "desert_low_mountains";
		}
		break;
		case TerrainEdit::TerrainType::desert_low_mountains_grow:
		{
			return "desert_low_mountains_grow";
		}
		break;
		case TerrainEdit::TerrainType::desert_high_mountains_1:
		{
			return "desert_high_mountains_1";
		}
		break;
		case TerrainEdit::TerrainType::desert_high_mountains_1_grow:
		{
			return "desert_high_mountains_1_grow";
		}
		break;
		default:
		{
			return "notset";
		}
		break;
		}
	}

	enum class TerrainEdit::TerrainType TerrainEdit::terrainTypeEnum(std::string& terrainType)
	{
		if (terrainType == "noise_1")
			return TerrainEdit::TerrainType::noise_1;
		else if (terrainType == "empty")
			return TerrainEdit::TerrainType::empty;
		else if (terrainType == "flat")
			return TerrainEdit::TerrainType::flat;
		else if (terrainType == "campaign_!")
			return TerrainEdit::TerrainType::campaign_1;
		else if (terrainType == "plateau_1")
			return TerrainEdit::TerrainType::plateau_1;
		else if (terrainType == "low_hills")
			return TerrainEdit::TerrainType::low_hills;
		else if (terrainType == "high_hills")
			return TerrainEdit::TerrainType::high_hills;
		else if (terrainType == "low_mountains")
			return TerrainEdit::TerrainType::low_mountains;
		else if (terrainType == "low_mountains_grow")
			return TerrainEdit::TerrainType::low_mountains_grow;
		else if (terrainType == "high_mountains_1")
			return TerrainEdit::TerrainType::high_mountains_1;
		else if (terrainType == "high_mountains_1_grow")
			return TerrainEdit::TerrainType::high_mountains_1_grow;
		else if (terrainType == "high_mountains_2")
			return TerrainEdit::TerrainType::high_mountains_2;
		else if (terrainType == "high_mountains_2_grow")
			return TerrainEdit::TerrainType::high_mountains_2_grow;
		else if (terrainType == "high_mountains_3")
			return TerrainEdit::TerrainType::high_mountains_3;
		else if (terrainType == "high_mountains_3_grow")
			return TerrainEdit::TerrainType::high_mountains_3_grow;
		else if (terrainType == "high_mountains_4")
			return TerrainEdit::TerrainType::high_mountains_4;
		else if (terrainType == "high_mountains_4_grow")
			return TerrainEdit::TerrainType::high_mountains_4_grow;
		else if (terrainType == "coastal")
			return TerrainEdit::TerrainType::coastal;
		else if (terrainType == "sea_abyss_1")
			return TerrainEdit::TerrainType::sea_abyss_1;
		else if (terrainType == "sea_abyss_2")
			return TerrainEdit::TerrainType::sea_abyss_2;
		else if (terrainType == "sea_abyss_3")
			return TerrainEdit::TerrainType::sea_abyss_3;
		else if (terrainType == "desert_flat_1")
			return TerrainEdit::TerrainType::desert_flat_1;
		else if (terrainType == "desert_low_dunes")
			return TerrainEdit::TerrainType::desert_low_dunes;
		else if (terrainType == "desert_low_mountains")
			return TerrainEdit::TerrainType::desert_low_mountains;
		else if (terrainType == "desert_low_mountains_grow")
			return TerrainEdit::TerrainType::desert_low_mountains_grow;
		else if (terrainType == "desert_high_mountains_1")
			return TerrainEdit::TerrainType::desert_high_mountains_1;
		else if (terrainType == "desert_high_mountains_1_grow")
			return TerrainEdit::TerrainType::desert_high_mountains_1_grow;
		else
			return TerrainEdit::TerrainType::notset;
	}

	void TerrainEdit::serialize(TheWorld_Utils::MemoryBuffer& buffer)
	{
		buffer.reserve(sizeof(TerrainEdit));
		buffer.set((BYTE*)this, sizeof(TerrainEdit));
	}

	std::string TerrainEdit::getNewTextureName(std::string name)
	{
		if (name == "grass")
			return "grass_2";
		else if (name == "Ground003_2K")
			return "grass_1";
		else if (name == "Ground037_1K")
			return "grass_3";
		else if (name == "sand")
			return "beach_1";
		else if (name == "Ground049C_1K")
			return "beach_2";
		else if (name == "Ground080_1K")
			return "desert_1";
		else if (name == "leaves")
			return "dirt_1";
		else if (name == "PaintedPlaster017_1K")
			return "snow_1";
		else if (name == "Rock029_1K")
			return "desert_2";
		else if (name == "rocks07")
			return "rocks_1";
		else if (name == "Rocks017_1K")
			return "rocks_2";
		return "";
	}

	void TerrainEdit::changeOldTextureNames(void)
	{
		std::string s = getNewTextureName(extraValues.lowElevationTexName_r);
		if (s.length() > 0)
		{
			memset(extraValues.lowElevationTexName_r, 0, sizeof(extraValues.lowElevationTexName_r));
			strcpy_s(extraValues.lowElevationTexName_r, sizeof(extraValues.lowElevationTexName_r), s.c_str());
		}
		s = getNewTextureName(extraValues.highElevationTexName_g);
		if (s.length() > 0)
		{
			memset(extraValues.highElevationTexName_g, 0, sizeof(extraValues.highElevationTexName_g));
			strcpy_s(extraValues.highElevationTexName_g, sizeof(extraValues.highElevationTexName_g), s.c_str());
		}
		s = getNewTextureName(extraValues.dirtTexName_b);
		if (s.length() > 0)
		{
			memset(extraValues.dirtTexName_b, 0, sizeof(extraValues.dirtTexName_b));
			strcpy_s(extraValues.dirtTexName_b, sizeof(extraValues.dirtTexName_b), s.c_str());
		}
		s = getNewTextureName(extraValues.rocksTexName_a);
		if (s.length() > 0)
		{
			memset(extraValues.rocksTexName_a, 0, sizeof(extraValues.rocksTexName_a));
			strcpy_s(extraValues.rocksTexName_a, sizeof(extraValues.rocksTexName_a), s.c_str());
		}
	}

	void TerrainEdit::deserialize(TheWorld_Utils::MemoryBuffer& buffer)
	{
		//size_t i1 = sizeof(TerrainEdit);				// DEBUG
		//size_t i2 = buffer.size();					// DEBUG
		//if (buffer.size() != size)					// DEBUG
		//	assert(buffer.size() != size);				// DEBUG
		size_t minLen = (buffer.size() <= size ? buffer.size() : size);
		size_t saveSize = size;
		TerrainEdit* p = (TerrainEdit*)buffer.ptr();
		init(p->terrainType);
		memcpy(this, buffer.ptr(), minLen);
		size = saveSize;

		changeOldTextureNames();
	}

	//bool TerrainEdit::detectSeaInQuadrant(void)
	//{
	//	bool done = false;

	//	//if ((extraValues.seaInfoSet != QUADRANT_SEA_INFO_SET_FALSE && extraValues.seaInfoSet != QUADRANT_SEA_INFO_SET_TRUE)
	//	//	|| (!heightsNeedRegen && extraValues.seaInfoSet == QUADRANT_SEA_INFO_SET_FALSE))
	//	//{
	//	//	if (!heightsNeedRegen)
	//	//	{
	//	//		extraValues.canContainSea = QUADRANT_SEA_INFO_SET_FALSE;

	//	//		//if (seaQuadrant())
	//	//		//{
	//	//		if (minHeight <= THEWORLD_SEA_LEVEL)
	//	//		{
	//	//			extraValues.canContainSea = QUADRANT_SEA_INFO_SET_TRUE;
	//	//		}
	//	//		//}
	//	//	}

	//	//	extraValues.seaInfoSet = QUADRANT_SEA_INFO_SET_TRUE;
	//	//	done = true;
	//	//}

	//	return done;
	//}

	bool TerrainEdit::canContainSea(void) const
	{
		//if (terrainType == TerrainType::empty || terrainType == TerrainType::coastal || terrainType == TerrainType::sea_abyss_1 || terrainType == TerrainType::sea_abyss_2 || terrainType == TerrainType::sea_abyss_3)
		//	return true;
		//else
		//	return false;

		if (minHeight <= (THEWORLD_SEA_LEVEL + THEWORLD_SEA_MAX_WAVE_HEIGHT))
			return true;
		else
			return false;
	}

	void TerrainEdit::adjustValues(TerrainEdit* northSideZMinus, TerrainEdit* southSideZPlus, TerrainEdit* westSideXMinus, TerrainEdit* eastSideXPlus)
	{
		if (!noise.desideredMinHeigthMandatory)
		{
			int num = 0;
			float averageHeight = 0;

			if (northSideZMinus != nullptr)
			{
				if (not northSideZMinus->southSideZPlus.needBlend)
				{
					averageHeight += (northSideZMinus->southSideZPlus.maxHeight + northSideZMinus->southSideZPlus.minHeight) / 2;
					num++;
				}
			}
			if (southSideZPlus != nullptr)
			{
				if (not southSideZPlus->northSideZMinus.needBlend)
				{
					averageHeight += (southSideZPlus->northSideZMinus.maxHeight + southSideZPlus->northSideZMinus.minHeight) / 2;
					num++;
				}
			}
			if (westSideXMinus != nullptr)
			{
				if (not westSideXMinus->eastSideXPlus.needBlend)
				{
					averageHeight += (westSideXMinus->eastSideXPlus.maxHeight + westSideXMinus->eastSideXPlus.minHeight) / 2;
					num++;
				}
			}
			if (eastSideXPlus != nullptr)
			{
				if (not eastSideXPlus->westSideXMinus.needBlend)
				{
					averageHeight += (eastSideXPlus->westSideXMinus.maxHeight + eastSideXPlus->westSideXMinus.minHeight) / 2;
					num++;
				}
			}

			if (num != 0)
				noise.desideredMinHeight = averageHeight / num;
		}

	}

	MemoryBuffer::MemoryBuffer(void)
	{
		m_ptr = nullptr;
		m_size = 0;
		m_bufferSize = 0;
	}

	MemoryBuffer::MemoryBuffer(BYTE* in, size_t len)
	{
		m_ptr = nullptr;
		m_size = 0;
		m_bufferSize = 0;

		set(in, len);
	}

	MemoryBuffer::~MemoryBuffer(void)
	{
		clear("destructor");
	}

	void MemoryBuffer::allocationSlotSize(size_t _allocationSlotSize)
	{
		m_allocationSlotSize = _allocationSlotSize;
	}

	void MemoryBuffer::toString(std::string& buffer)
	{
		size_t sz = size();
		buffer.clear();
		buffer.reserve(sz);
		buffer.append((char*)ptr(), sz);
		//buffer = std::string((char*)ptr(), sz);
	}

	void MemoryBuffer::set(BYTE* in, size_t size)
	{
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MemorybufferSet 1 ") + std::to_string(len) + " " + __FUNCTION__, "reserve");
			reserve(size);
		}
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MemorybufferSet 2 ") + std::to_string(len) + " " + __FUNCTION__, "memcpy");
			memcpy(m_ptr, in, size);
		}
		m_size = size;
	}

	void MemoryBuffer::append(BYTE* in, size_t size)
	{
		size_t newLen = m_size + size;
		reserve(newLen);
		memcpy(m_ptr + m_size, in, size);
		m_size += size;
	}

	void MemoryBuffer::reserve(size_t newLen)
	{
		//size_t newLen = size;
		if (newLen > m_bufferSize)
		{
			size_t numSlots = size_t(ceil(double(newLen) / double(m_allocationSlotSize)));
			size_t newLenCalc = numSlots * m_allocationSlotSize;
			assert(newLenCalc >= newLen);
			BYTE* ptr = (BYTE*)calloc(1, newLenCalc);
			if (ptr == nullptr)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Allocation error")).c_str()));
			m_bufferSize = newLenCalc;

			if (m_ptr != nullptr)
			{
				memcpy(ptr, m_ptr, m_size);
				::free(m_ptr);
			}
			m_ptr = ptr;
		}
	}

	size_t MemoryBuffer::reserved(void) const
	{
		return m_bufferSize;
	}

	void MemoryBuffer::reset(void)
	{
		m_size = 0;
	}

	void MemoryBuffer::clear(std::string lastActor)
	{
		if (m_ptr != nullptr)
		{
			::free(m_ptr);
			m_ptr = nullptr;
			m_size = 0;
			m_bufferSize = 0;
			m_lastActor = lastActor;
		}
	}
	
	BYTE* MemoryBuffer::ptr()
	{
		return m_ptr;
	}
	size_t MemoryBuffer::size(void) const
	{
		return m_size;
	}
	bool MemoryBuffer::empty(void) const
	{
		return m_size == 0;
	}
	void MemoryBuffer::adjustSize(size_t size)
	{
		m_size = size;
	}

	void MemoryBuffer::adjustCursorAndSize(size_t& cursor, size_t size)
	{
		cursor += size;
		adjustSize(cursor);
	}

	void MemoryBuffer::copyFrom(MemoryBuffer& inBuffer, std::string lastActor)
	{
		clear(lastActor);
		reserve(inBuffer.reserved());
		memcpy(m_ptr, inBuffer.ptr(), inBuffer.size());
		m_size = inBuffer.size();
	}

	void MemoryBuffer::populateFloatVector(std::vector<float>& v) const
	{
		size_t numElements = size() / sizeof(float);
		my_assert(size() == numElements * sizeof(float));
		v.assign((float*)m_ptr, (float*)m_ptr + numElements);
	}

	void MemoryBuffer::populateUint16Vector(std::vector<uint16_t>& v) const
	{
		size_t numElements = size() / sizeof(uint16_t);
		my_assert(size() == numElements * sizeof(uint16_t));
		v.assign((uint16_t*)m_ptr, (uint16_t*)m_ptr + numElements);
	}

	void Utils::plogInit(plog::Severity sev, plog::IAppender* appender)
	{
		plog::init(sev, appender);

		PLOG(plog::get()->getMaxSeverity()) << "***************";
		PLOG(plog::get()->getMaxSeverity()) << "Log initilized!";
		PLOG(plog::get()->getMaxSeverity()) << "***************";
	}

	void Utils::plogDenit(void)
	{
		PLOG(plog::get()->getMaxSeverity()) << "*****************";
		PLOG(plog::get()->getMaxSeverity()) << "Log Terminated!";
		PLOG(plog::get()->getMaxSeverity()) << "*****************";
	}

	void Utils::plogSetMaxSeverity(plog::Severity sev)
	{
		plog::get()->setMaxSeverity(sev);
		PLOG(plog::get()->getMaxSeverity()) << "Log severity changed to: " << std::to_string(sev);
	}

	MeshCacheBuffer::MeshCacheBuffer(void)
	{
		m_gridStepInWU = -1;
		m_numVerticesPerSize = -1;
		m_level = 0;
		m_lowerXGridVertex = 0;
		m_lowerZGridVertex = 0;
	}

	MeshCacheBuffer::~MeshCacheBuffer(void)
	{
	}
		
	MeshCacheBuffer::MeshCacheBuffer(std::string _cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level, float lowerXGridVertex, float lowerZGridVertex)
	{
		m_mapName = mapName;
		m_gridStepInWU = gridStepInWU;
		m_numVerticesPerSize = numVerticesPerSize;
		m_level = level;
		m_lowerXGridVertex = lowerXGridVertex;
		m_lowerZGridVertex = lowerZGridVertex;

		m_cacheDir = MeshCacheBuffer::cacheDir(_cacheDir, mapName, gridStepInWU, numVerticesPerSize, level);
		if (m_cacheDir.length() < 10)
		{
			std::string msg = std::string("Beccato " + _cacheDir + " " + mapName + " " + std::to_string(gridStepInWU) + " " + std::to_string(numVerticesPerSize) + " " + std::to_string(level) + " " + std::to_string(lowerXGridVertex) + " " + std::to_string(lowerZGridVertex));
			PLOG_ERROR << msg;
			throw(GDN_TheWorld_Exception(__FUNCTION__, msg.c_str()));
		}
		if (!fs::exists(m_cacheDir))
		{
			fs::create_directories(m_cacheDir);
		}
		if (!fs::exists(m_cacheDir + "\\Work"))
		{
			fs::create_directories(m_cacheDir + "\\Work");
		}
		std::string cacheFileName = "X-" + std::to_string(lowerXGridVertex) + "_Z-" + std::to_string(lowerZGridVertex) + ".mesh";
		m_cacheFilePath = m_cacheDir + "\\" + cacheFileName;
		m_cacheWorkFilePath = getWorkCacheDirPathStatic(m_cacheDir) + cacheFileName;
	}
	
	MeshCacheBuffer::MeshCacheBuffer(const MeshCacheBuffer& c)
	{
		*this = c;
	}

	MeshCacheBuffer::MeshCacheBuffer(std::string& cacheDir, std::string& mapName, TheWorld_Utils::QuadrantPos& quadrantPos)
	{
		*this = MeshCacheBuffer(cacheDir, mapName, quadrantPos.getGridStepInWU(), quadrantPos.getNumVerticesPerSize(), quadrantPos.getLevel(), quadrantPos.getLowerXGridVertex(), quadrantPos.getLowerZGridVertex());
	}

	std::string MeshCacheBuffer::cacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level)
	{
		return std::string(cacheDir) + "\\" + "Cache\\maps\\" + mapName + "\\" + "ST-" + std::to_string(gridStepInWU) + "_SZ-" + std::to_string(numVerticesPerSize) + "\\L-" + std::to_string(level);
	}
		
	void MeshCacheBuffer::operator=(const MeshCacheBuffer& c)
	{
		m_cacheFilePath = c.m_cacheFilePath;
		m_cacheWorkFilePath = c.m_cacheWorkFilePath;
		m_cacheDir = c.m_cacheDir;
		m_meshId = c.m_meshId;
		//m_buffer = c.m_buffer;
		m_mapName = c.m_mapName;
		m_gridStepInWU = c.m_gridStepInWU;
		m_numVerticesPerSize = c.m_numVerticesPerSize;
		m_level = c.m_level;
		m_lowerXGridVertex = c.m_lowerXGridVertex;
		m_lowerZGridVertex = c.m_lowerZGridVertex;
	}

	bool MeshCacheBuffer::operator<(const MeshCacheBuffer& c) const
	{
		float quadrantSize = m_gridStepInWU * m_numVerticesPerSize;
		float _quadrantSize = c.m_gridStepInWU * c.m_numVerticesPerSize;

		if (quadrantSize < _quadrantSize)
			return true;
		if (quadrantSize > _quadrantSize)
			return false;

		if (m_level < c.m_level)
			return true;
		if (m_level > c.m_level)
			return false;

		if (m_lowerZGridVertex < c.m_lowerZGridVertex)
			return true;
		if (m_lowerZGridVertex > c.m_lowerZGridVertex)
			return false;

		return m_lowerXGridVertex < c.m_lowerXGridVertex;
	}

	bool MeshCacheBuffer::operator==(const MeshCacheBuffer& c) const
	{
		if (m_cacheFilePath == c.m_cacheFilePath
			&& m_cacheWorkFilePath == c.m_cacheWorkFilePath
			&& m_cacheDir == c.m_cacheDir
			&& m_meshId == c.m_meshId
			//&& m_buffer == c.m_buffer
			&& m_mapName == c.m_mapName
			&& m_gridStepInWU == c.m_gridStepInWU
			&& m_numVerticesPerSize == c.m_numVerticesPerSize
			&& m_level == c.m_level
			&& m_lowerXGridVertex == c.m_lowerXGridVertex
			&& m_lowerZGridVertex == c.m_lowerZGridVertex)
			return true;
		else
			return false;
	}

	std::string MeshCacheBuffer::generateNewMeshId(void)
	{
		GUID newId;
		RPC_STATUS ret_val = ::UuidCreate(&newId);
		if (ret_val != RPC_S_OK)
		{
			std::string msg = "UuidCreate in error with rc " + std::to_string(ret_val);
			PLOG_ERROR << msg;
			throw(GDN_TheWorld_Exception(__FUNCTION__, msg.c_str()));
		}

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::string str = std::format("{}", now);			// DEBUG
		now = current_utc_time_point();
		str = std::format("{}", now);			// DEBUG
		BYTE shortBuffer[256 + 1];
		size_t size = 0;
		TheWorld_Utils::serializeToByteStream<std::chrono::system_clock::time_point>(now, shortBuffer, size);
		
		//std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
		//tm local_tm;
		//localtime_s(&local_tm, &now_time_t);
		//std::string year = std::to_string(local_tm.tm_year + 1900);
		//std::string month = std::to_string(local_tm.tm_mon + 1);
		//size_t n_zero = 2;
		//month = std::string(n_zero - std::min(n_zero, month.length()), '0') + month;
		//std::string day = std::to_string(local_tm.tm_mday);
		//day = std::string(n_zero - std::min(n_zero, day.length()), '0') + day;
		//std::string hours = std::to_string(local_tm.tm_hour);
		//hours = std::string(n_zero - std::min(n_zero, hours.length()), '0') + hours;
		//std::string minutes = std::to_string(local_tm.tm_min);
		//minutes = std::string(n_zero - std::min(n_zero, minutes.length()), '0') + minutes;
		//std::string seconds = std::to_string(local_tm.tm_sec);
		//seconds = std::string(n_zero - std::min(n_zero, seconds.length()), '0') + seconds;
		//std::string meshId = ToString(&newId)
		//	+ ":" + year + "-" + month  + "-" + day + "_" + hours + "-" + minutes + "-" + seconds
		//	+ ":" + std::string((char*)shortBuffer, size);
		//return meshId;

		std::time_t now_time_t_utc = std::chrono::system_clock::to_time_t(now);
		std::string now_str = TheWorld_Utils::time_t_to_str_time(now_time_t_utc);
		std::replace(now_str.begin(), now_str.end(), ' ', '_');
		std::replace(now_str.begin(), now_str.end(), ':', '-');
		std::string meshId = ToString(&newId)
			+ ":" + now_str
			+ ":" + std::string((char*)shortBuffer, size);

		//assert(meshId.length() == meshId1.length());
		//assert(meshId == meshId1);

		return meshId;
	}

	bool MeshCacheBuffer::isEmptyBuffer(std::string meshId)
	{
		my_assert(meshId.size() > 0);
		if (meshId.size() == 0)
			return true;

		std::vector<std::string> v = TheWorld_Utils::Utils::split(meshId, ':');
		my_assert(v.size() >= 3);
		if (v.size() < 3)
			return true;

		std::string temp = v[1].substr(0, 4);
		if (v[1].substr(0, 4) == "1970")
			return true;

		return false;
	}

	std::string MeshCacheBuffer::generateNewMeshIdForEmptyBuffer(void)
	{
		GUID newId;
		RPC_STATUS ret_val = ::UuidCreate(&newId);
		if (ret_val != RPC_S_OK)
		{
			std::string msg = "UuidCreate in error with rc " + std::to_string(ret_val);
			PLOG_ERROR << msg;
			throw(GDN_TheWorld_Exception(__FUNCTION__, msg.c_str()));
		}

		std::chrono::system_clock::time_point timeZeroTime;
		std::string str = std::format("{}", timeZeroTime);			// DEBUG
		BYTE shortBuffer[256 + 1];
		size_t size = 0;
		TheWorld_Utils::serializeToByteStream<std::chrono::system_clock::time_point>(timeZeroTime, shortBuffer, size);
		
		//std::time_t time_zero_time_t = std::chrono::system_clock::to_time_t(timeZeroTime);
		//tm local_tm;
		//localtime_s(&local_tm, &time_zero_time_t);
		//std::string year = std::to_string(local_tm.tm_year + 1900);
		//std::string month = std::to_string(local_tm.tm_mon + 1);
		//size_t n_zero = 2;
		//month = std::string(n_zero - std::min(n_zero, month.length()), '0') + month;
		//std::string day = std::to_string(local_tm.tm_mday);
		//day = std::string(n_zero - std::min(n_zero, day.length()), '0') + day;
		//std::string hours = std::to_string(local_tm.tm_hour);
		//hours = std::string(n_zero - std::min(n_zero, hours.length()), '0') + hours;
		//std::string minutes = std::to_string(local_tm.tm_min);
		//minutes = std::string(n_zero - std::min(n_zero, minutes.length()), '0') + minutes;
		//std::string seconds = std::to_string(local_tm.tm_sec);
		//seconds = std::string(n_zero - std::min(n_zero, seconds.length()), '0') + seconds;
		//std::string meshId = ToString(&newId)
		//	+ ":" + year + "-" + month + "-" + day + "_" + hours + "-" + minutes + "-" + seconds
		//	+ ":" + std::string((char*)shortBuffer, size);
		//return meshId;

		std::time_t now_time_t_utc = std::chrono::system_clock::to_time_t(timeZeroTime);
		std::string now_str = TheWorld_Utils::time_t_to_str_time(now_time_t_utc);
		std::replace(now_str.begin(), now_str.end(), ' ', '_');
		std::replace(now_str.begin(), now_str.end(), ':', '-');
		std::string meshId = ToString(&newId)
			+ ":" + now_str
			+ ":" + std::string((char*)shortBuffer, size);

		//assert(meshId.length() == meshId1.length());
		//assert(meshId == meshId1);

		return meshId;
	}

	std::chrono::system_clock::time_point MeshCacheBuffer::getTimeFromMeshId(std::string meshId)
	{
		std::string meshIdWithoutBinaryTime;
		return getTimeFromMeshId(meshId, meshIdWithoutBinaryTime);
	}

	std::chrono::system_clock::time_point MeshCacheBuffer::getTimeFromMeshId(std::string& meshId, std::string& meshIdWithoutBinaryTime)
	{
		size_t size = 0;
		std::chrono::system_clock::time_point time;
		meshIdWithoutBinaryTime = "";

		std::vector<std::string> v = TheWorld_Utils::Utils::split(meshId, ':');
		my_assert(v.size() >= 1);
		if (v.size() > 1)	// else time is equal to std::chrono::system_clock::time_point()
		{
			if (v.size() >= 4)
				assert(v.size() >= 4);	// DEBUGBREAKPOINT
			my_assert(v.size() > 2);
			size_t headerToDiscardSize = v[0].size() + v[1].size() + 2;
			meshIdWithoutBinaryTime = meshId.substr(0, headerToDiscardSize);
			std::string s = meshId.substr(headerToDiscardSize);
			size_t sz = s.size();
			time = TheWorld_Utils::deserializeFromByteStream<std::chrono::system_clock::time_point>((BYTE*)s.c_str(), size);
			std::string time_str = std::format("{}", time);									// DEBUG
		}
				
		return time;
	}

	std::string MeshCacheBuffer::meshIdToPrint1(std::string meshId)
	{
		return meshIdToPrint(meshId);
	}

	std::string MeshCacheBuffer::meshIdToPrint(std::string& meshId)
	{
		std::string ret;
		if (meshId.length() == 0)
			return ret;
		std::chrono::system_clock::time_point tp = getTimeFromMeshId(meshId, ret);
		ret += std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count());
		return ret;
	}

	bool MeshCacheBuffer::firstMeshIdMoreRecent(std::string& firstMeshId, std::string& secondMeshId)
	{
		size_t size = 0;
		//BYTE shortBuffer[256 + 1];

		std::chrono::system_clock::time_point firstTime;
		//std::string sFirstTime;	// DEBUG
		std::vector<std::string> v = TheWorld_Utils::Utils::split(firstMeshId, ':');
		my_assert(v.size() >= 1);
		if (v.size() > 1)
		{
			//if (v.size() >= 4)
			//	assert(v.size() >= 4);	// DEBUGBREAKPOINT
			my_assert(v.size() > 2);
			size_t headerToDiscardSize = v[0].size() + v[1].size() + 2;
			std::string s = firstMeshId.substr(headerToDiscardSize);
			size_t sz = s.size();
			firstTime = TheWorld_Utils::deserializeFromByteStream<std::chrono::system_clock::time_point>((BYTE*)s.c_str(), size);
			// DEBUG
			//{
			//	time_t t = std::chrono::system_clock::to_time_t(firstTime);
			//	std::tm t1;	localtime_s(&t1, &t);
			//	char buff[70];	strftime(buff, sizeof buff, "%Y %m %e %T", &t1);
			//	sFirstTime = buff;
			//}
			// DEBUG
		}

		////time_t firstTime = 0;
		//size_t pos = firstMeshId.find(":");
		//if (pos != std::string::npos && pos + 1 < firstMeshId.size())
		//{
		//	memcpy(shortBuffer, &firstMeshId[pos + 1], firstMeshId.size() - pos - 1);
		//	//firstTime = TheWorld_Utils::deserializeFromByteStream<time_t>(shortBuffer, size);
		//	firstTime = TheWorld_Utils::deserializeFromByteStream<std::chrono::system_clock::time_point>(shortBuffer, size);
		//}

		std::chrono::system_clock::time_point secondTime;
		//std::string sSecondTime;	// DEBUG
		v = TheWorld_Utils::Utils::split(secondMeshId, ':');
		my_assert(v.size() >= 1);
		if (v.size() > 1)
		{
			if (v.size() >= 4)
				assert(v.size() >= 4);	// DEBUGBREAKPOINT
			my_assert(v.size() > 2);
			size_t headerToDiscardSize = v[0].size() + v[1].size() + 2;
			std::string s = secondMeshId.substr(headerToDiscardSize);
			size_t sz = s.size();
			secondTime = TheWorld_Utils::deserializeFromByteStream<std::chrono::system_clock::time_point>((BYTE*)s.c_str(), size);
			// DEBUG
			//{
			//	time_t t = std::chrono::system_clock::to_time_t(secondTime);
			//	std::tm t1;	localtime_s(&t1, &t);
			//	char buff[70];	strftime(buff, sizeof buff, "%Y %m %e %T", &t1);
			//	sSecondTime = buff;
			//}
			// DEBUG
		}

		////time_t secondTime = 0;
		//pos = secondMeshId.find(":");
		//if (pos != std::string::npos && pos + 1 < secondMeshId.size())
		//{
		//	memcpy(shortBuffer, &secondMeshId[pos + 1], secondMeshId.size() - pos - 1);
		//	//secondTime = TheWorld_Utils::deserializeFromByteStream<time_t>(shortBuffer, size);
		//	secondTime = TheWorld_Utils::deserializeFromByteStream<std::chrono::system_clock::time_point>(shortBuffer, size);
		//}

		//if (secondTime == 0)
		if (secondTime == std::chrono::system_clock::time_point())
			return true;

		//if (firstTime == 0)
		if (firstTime == std::chrono::system_clock::time_point())
			return false;

		if (firstTime == secondTime)
			return true;

		if (firstTime > secondTime)
			return true;
		else
			return false;

		//double diff = difftime(firstTime, secondTime);
		//size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(firstTime - secondTime).count();

		//if (diff > 0)
		//	return true;
		//else
		//	return false;
	}

	void MeshCacheBuffer::clearWorkCacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level)
	{
		std::string dirPath = MeshCacheBuffer::cacheDir(cacheDir, mapName, gridStepInWU, numVerticesPerSize, level);
		std::string workDirPath = getWorkCacheDirPathStatic(dirPath);
		if (workDirPath[workDirPath.size() - 1] == '\\')
			workDirPath.pop_back();
		if (fs::exists(workDirPath))
			fs::remove_all(workDirPath);
		fs::create_directories(workDirPath);
	}

	void MeshCacheBuffer::getAllDiskCache(std::string _cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level, std::map<TheWorld_Utils::QuadrantPos, TheWorld_Utils::MeshCacheBuffer>& cacheMap, std::time_t& fromFileTimeType, bool workDir)
	{
		std::string dirPath  = MeshCacheBuffer::cacheDir(_cacheDir, mapName, gridStepInWU, numVerticesPerSize, level);
		if (workDir)
			dirPath = getWorkCacheDirPathStatic(dirPath);

		if (!fs::exists(dirPath))
			return;

		std::time_t _fromFileTimeType = fromFileTimeType;
		fromFileTimeType = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		
		for (const auto& entry : fs::directory_iterator(dirPath))
			if (entry.is_regular_file())
			{
				auto fileTimeType = entry.last_write_time();
				const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTimeType);
				const auto fileTime = std::chrono::system_clock::to_time_t(systemTime);
				if (fileTime > _fromFileTimeType)
				{
					std::string filename = entry.path().filename().generic_string();
					size_t pos = filename.find_last_of('.');
					if (pos != std::string::npos)
					{
						std::string filenameNoExt = filename.substr(0, pos);
						std::vector<std::string> v1 = TheWorld_Utils::Utils::split(filenameNoExt, '_');
						if (v1.size() == 2)
						{
							float lowerXGridVertex = FLT_MAX, lowerZGridVertex = FLT_MAX;

							pos = v1[0].find_first_of('-');
							if (pos != std::string::npos)
							{
								std::string coord = v1[0].substr(0, pos);
								std::string value = v1[0].substr(pos + 1);
								if (coord == "X")
									lowerXGridVertex = std::stof(value);
							}

							pos = v1[1].find_first_of('-');
							if (pos != std::string::npos)
							{
								std::string coord = v1[1].substr(0, pos);
								std::string value = v1[1].substr(pos + 1);
								if (coord == "Z")
									lowerZGridVertex = std::stof(value);
							}

							if (lowerXGridVertex != FLT_MAX && lowerZGridVertex != FLT_MAX)
							{
								TheWorld_Utils::QuadrantPos pos(lowerXGridVertex, lowerZGridVertex, level, int(numVerticesPerSize), gridStepInWU);
								cacheMap[pos] = MeshCacheBuffer(_cacheDir, mapName, gridStepInWU, numVerticesPerSize, level, lowerXGridVertex, lowerZGridVertex);
							}
						}
					}
				}
			}
	}
	
	std::string MeshCacheBuffer::getMeshIdFromDisk(bool workDir, TerrainEdit* terrainEdit, bool updateLastAccessTime)
	{
		//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer getMeshIdFromCache 1") + __FUNCTION__, "ALL");

		if (m_meshId.size() > 0)
			return m_meshId;
		
		std::string cacheFilePath = getCacheFilePath();
		if (workDir)
			cacheFilePath = getWorkCacheFilePath();

		if (!fs::exists(cacheFilePath))
			return "";
		
		BYTE shortBuffer[256 + 1];
		size_t size;

		// get size of a size_t
		size_t size_t_size = sizeof(size_t);

		FILE* inFile = nullptr;
		errno_t err = fopen_s(&inFile, cacheFilePath.c_str(), "rb");
		if (err != 0)
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Open " + cacheFilePath + " in errore - Err=" + std::to_string(err)).c_str()));

		if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t bufferSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		if (fread(shortBuffer, 1, 1, inFile) != 1)	// "0"
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 1!").c_str()));
		}

		// read the serialized size of the mesh id
		if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t meshIdSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		// read the mesh id
		if (fread(shortBuffer, meshIdSize, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 3!").c_str()));
		}
		// and deserialize it
		m_meshId = std::string((char*)shortBuffer, meshIdSize);

		if (terrainEdit != nullptr)
		{
			// read the serialized size of TerrainEdit
			if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
			{
				fclose(inFile);
				throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
			}
			// and deserialize it
			size_t terrainEditSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

			// read TerrainEdit
			TheWorld_Utils::MemoryBuffer terrainEditBuffer;
			terrainEditBuffer.reserve(terrainEditSize);
			if (fread(terrainEditBuffer.ptr(), terrainEditSize, 1, inFile) != 1)
			{
				fclose(inFile);
				throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
			}
			terrainEditBuffer.adjustSize(terrainEditSize);
			terrainEdit->deserialize(terrainEditBuffer);
		}

		fclose(inFile);

		if (updateLastAccessTime)
		{
			HANDLE hFile = CreateFile(convert(cacheFilePath).c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				SYSTEMTIME st;	GetSystemTime(&st); // Gets the current system time
				FILETIME ft;	SystemTimeToFileTime(&st, &ft); // Converts the current system time to file time format
				// set new LastAccess Time
				DWORD dwLastErr = ERROR_SUCCESS;
				bool bResult = false;
				if (!(bResult = SetFileTime(hFile, NULL, &ft, NULL)))
					dwLastErr = GetLastError();
				CloseHandle(hFile);
				SetLastError(dwLastErr);
			}

			// Read last acces time
			//errno_t err = fopen_s(&inFile, cacheFilePath.c_str(), "rb");
			//struct _stat stat;
			//int result = _stat(cacheFilePath.c_str(), &stat);
			//fclose(inFile);
			//char buffer1[512];																// DEBUG ONLY
			//ctime_s(buffer1, sizeof(buffer1), &stat.st_mtime);								// DEBUG ONLY
			//time_t ltime;	time(&ltime); //Gets the system time elapse in secs
			//double timeDifference = difftime(ltime, stat.st_mtime);			//get the time difference between current time and file last modified time		
			//double timeDiffInDays = (timeDifference / 60 / 60 / 24);		//get time difference in days
		}
		
		return m_meshId;
	}

	std::string MeshCacheBuffer::getMeshIdStr(void)
	{
		return meshIdToPrint(m_meshId);
	}
		
	std::string MeshCacheBuffer::getCacheIdStr(void)
	{
		return "WORLD QUADRANT-" + m_mapName + "_ST-" + std::to_string(m_gridStepInWU) + "_SZ-" + std::to_string(m_numVerticesPerSize) + "_L-" + std::to_string(m_level) + "_X-" + std::to_string(m_lowerXGridVertex) + "_Z-" + std::to_string(m_lowerZGridVertex) + " Hash=" + meshIdToPrint(m_meshId);
	}

	bool MeshCacheBuffer::refreshMapsFromDisk(size_t numVerticesPerSize, float gridStepInWU, std::string _meshId, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1 ") + __FUNCTION__, "ALL");

		TheWorld_Utils::MemoryBuffer buffer;

		bool empty = false;
		
		if (_meshId.length() == 0)
		{
			size_t s = _meshId.length();	// Trap
			s = _meshId.length();
		}
		
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.1  ") + __FUNCTION__, "readBufferFromDisk");
			readBufferFromDisk(_meshId, buffer);
		}

		if (buffer.size() == 0)
		{
			TheWorld_Utils::MemoryBuffer tempBuffer;

			{
				TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.2 ") + __FUNCTION__, "setEmptyBuffer");
				setEmptyBuffer(numVerticesPerSize, _meshId, tempBuffer);
			}

			{
				TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.3 ") + __FUNCTION__, "readMapsFromMeshCache");

				refreshMapsFromBuffer(tempBuffer, _meshId, terrainEditValues, minAltitude, maxAltitude, float16HeigthsBuffer, float32HeigthsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, false);
			}

			empty = true;
		}
		else
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.4  ") + __FUNCTION__, "refreshMapsFromBuffer");
			std::string meshIdFromBuffer;
			refreshMapsFromBuffer(buffer, meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float16HeigthsBuffer, float32HeigthsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, false);
			//assert(_meshId.size() == 0 || meshIdFromBuffer == _meshId);
			if (_meshId.size() > 0 && meshIdFromBuffer != _meshId)
			{
				PLOG_DEBUG << "meshId from cache buffer (" << meshIdFromBuffer << ") not equal to meshId in input (" << _meshId;
				//throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("meshId from cache buffer (") + meshIdFromBuffer + ") not equal to meshId in input (" + _meshId).c_str()));
			}
		}

		return empty;
	}

	void MeshCacheBuffer::refreshMapsFromBuffer(const BYTE* buffer, const size_t bufferSize, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromBuffer 1 ") + __FUNCTION__, "ALL");

		if (bufferSize == 0)
		{
			meshIdFromBuffer.clear();
			return;
		}

		size_t size = 0;

		const char* movingStreamBuffer = (const char*)buffer;
		const char* endOfBuffer = movingStreamBuffer + bufferSize;

		movingStreamBuffer++;	// bypass "0"

		size_t meshIdSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
		movingStreamBuffer += size;

		meshIdFromBuffer = std::string(movingStreamBuffer, meshIdSize);
		movingStreamBuffer += meshIdSize;

		m_meshId = meshIdFromBuffer;

		size_t terrainEditValuesSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
		movingStreamBuffer += size;

		if (terrainEditValuesSize > 0)
		{
			terrainEditValues.set((BYTE*)movingStreamBuffer, terrainEditValuesSize);
			TerrainEdit terrainEdit;
			terrainEdit.deserialize(terrainEditValues);
			terrainEditValues.set((BYTE*)&terrainEdit, terrainEdit.size);
			movingStreamBuffer += terrainEditValuesSize;
		}
		else
			terrainEditValues.reset();

		size_t vectSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
		movingStreamBuffer += size;

		size_t uint16_t_size = sizeof(uint16_t);	// the size of an half ==> float_16

		size_t float_size = sizeof(float);

		if (vectSize > 0)
		{
			minAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;

			maxAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;

			size_t float16HeightMapSize = vectSize * uint16_t_size;
			float16HeigthsBuffer.set((BYTE*)movingStreamBuffer, float16HeightMapSize);
			movingStreamBuffer += float16HeightMapSize;

			size_t float32HeightMapSize = vectSize * float_size;
			float32HeigthsBuffer.set((BYTE*)movingStreamBuffer, float32HeightMapSize);
			movingStreamBuffer += float32HeightMapSize;

			struct TheWorld_Utils::_RGB* rgb = (struct TheWorld_Utils::_RGB*)movingStreamBuffer;
			if (rgb->r == 0 && rgb->g == 0 && rgb->b == 0)
			{
				normalsBuffer.clear(__FUNCTION__);
				movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGB);
			}
			else
			{
				size_t normalMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGB);
				normalsBuffer.set((BYTE*)movingStreamBuffer, normalMapSize);
				movingStreamBuffer += normalMapSize;
			}

			if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGBA) <= endOfBuffer)
			{
				struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)movingStreamBuffer;
				if (rgba->r == 0 && rgba->g == 0 && rgba->b == 0 && rgba->a == 0)
				{
					splatmapBuffer.clear(__FUNCTION__);
					movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
				}
				else
				{
					size_t splatmapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGBA);
					splatmapBuffer.set((BYTE*)movingStreamBuffer, splatmapMapSize);
					movingStreamBuffer += splatmapMapSize;
				}
			}
			else
			{
				splatmapBuffer.clear(__FUNCTION__);
			}
			
			if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGBA) <= endOfBuffer)
			{
				struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)movingStreamBuffer;
				if ((rgba->r == 0 && rgba->g == 0 && rgba->b == 0 && rgba->a == 0) /* || (rgba->r == 255 && rgba->g == 255 && rgba->b == 255 && rgba->a == 255)*/)
				{
					colormapBuffer.clear(__FUNCTION__);
					movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
				}
				else
				{
					size_t colormapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGBA);
					colormapBuffer.set((BYTE*)movingStreamBuffer, colormapMapSize);
					movingStreamBuffer += colormapMapSize;
				}
			}
			else
			{
				colormapBuffer.clear(__FUNCTION__);
			}

			if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGB) <= endOfBuffer)
			{
				struct TheWorld_Utils::_RGB* rgb = (struct TheWorld_Utils::_RGB*)movingStreamBuffer;
				if ((rgb->r == 0 && rgb->g == 0 && rgb->b == 0) /* || (rgb->r == 255 && rgb->g == 255 && rgb->b == 255)*/)
				{
					globalmapBuffer.clear(__FUNCTION__);
					movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGB);
				}
				else
				{
					size_t globalmapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGB);
					globalmapBuffer.set((BYTE*)movingStreamBuffer, globalmapMapSize);
					movingStreamBuffer += globalmapMapSize;
				}
			}
			else
			{
				globalmapBuffer.clear(__FUNCTION__);
			}

			if (updateCache)
			{
				std::string meshId;
				writeBufferToDiskCache(buffer, bufferSize, meshId);
			}
		}
		else
		{
			minAltitude = maxAltitude = 0;
			float16HeigthsBuffer.clear(__FUNCTION__);
			float32HeigthsBuffer.clear(__FUNCTION__);
			normalsBuffer.clear(__FUNCTION__);
			splatmapBuffer.clear(__FUNCTION__);
			colormapBuffer.clear(__FUNCTION__);
			globalmapBuffer.clear(__FUNCTION__);
		}
	}

	void MeshCacheBuffer::refreshMapsFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache)
	{
		refreshMapsFromBuffer(buffer.ptr(), buffer.size(), meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float16HeigthsBuffer, float32HeigthsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, updateCache);
	}

	void MeshCacheBuffer::refreshMapsFromBuffer(std::string& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache)
	{
		refreshMapsFromBuffer((BYTE*)buffer.c_str(), buffer.size(), meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float16HeigthsBuffer, float32HeigthsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, updateCache);
	}

	void MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(const BYTE* buffer, const size_t bufferSize, CacheQuadrantData& cacheQuadrantData, bool updateCache)
	{
		refreshMapsFromBuffer(buffer, bufferSize, cacheQuadrantData.meshId, *cacheQuadrantData.terrainEditValues, cacheQuadrantData.minHeight, cacheQuadrantData.maxHeight, *cacheQuadrantData.heights16Buffer, *cacheQuadrantData.heights32Buffer, *cacheQuadrantData.normalsBuffer, *cacheQuadrantData.splatmapBuffer, *cacheQuadrantData.colormapBuffer, *cacheQuadrantData.globalmapBuffer, updateCache);
	}

	void MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache)
	{
		refreshCacheQuadrantDataFromBuffer(buffer.ptr(), buffer.size(), cacheQuadrantData, updateCache);
	}

	void MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(std::string& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache)
	{
		refreshCacheQuadrantDataFromBuffer((BYTE*)buffer.c_str(), buffer.size(), cacheQuadrantData, updateCache);
	}

	void MeshCacheBuffer::refreshInternalCacheQuadrantDataFromDisk(bool workDir, bool force, bool renewMeshId)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshInternalCacheQuadrantDataFromDisk 1 ") + __FUNCTION__, "ALL");

		if (force || !m_internalCacheQuadrantData.buffersAllocated())
		{
			std::string buffer;
			readBufferFromDisk(m_meshId, buffer, workDir);
			m_internalCacheQuadrantData.allocateBuffers();
			bool renewed = false;
			if (m_meshId.length() == 0 || buffer.length() == 0)
			{
				std::string meshid;
				setEmptyBuffer(getNumVerticesPerSize(), meshid, buffer);
				renewed = true;
			}
			refreshCacheQuadrantDataFromBuffer(buffer, m_internalCacheQuadrantData, false);
			if (renewMeshId && !renewed)
			{
				m_internalCacheQuadrantData.meshId = generateNewMeshId();
				m_meshId = m_internalCacheQuadrantData.meshId;
			}
			//m_internalCacheQuadrantData.terrainData()->deserialize(*m_internalCacheQuadrantData.terrainEditValues);
		}
	}

	MeshCacheBuffer::CacheQuadrantData* MeshCacheBuffer::internalCacheQuadrantData(void)
	{
		return &m_internalCacheQuadrantData;
	}

	
	void MeshCacheBuffer::readBufferFromDisk(std::string _meshId, TheWorld_Utils::MemoryBuffer& buffer, bool workDir, TerrainEdit* terrainEdit)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer readBufferFromDisk 1 ") + __FUNCTION__, "ALL");

		BYTE shortBuffer[256 + 1];
		size_t size;

		std::string cacheFilePath = getCacheFilePath();
		if (workDir)
			cacheFilePath = getWorkCacheFilePath();

		// read vertices from local cache
		if (!fs::exists(cacheFilePath))
		{
			buffer.clear(__FUNCTION__);
			return;
			//throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("not found current quadrant in cache").c_str()));
		}

		size_t size_t_size = sizeof(size_t);	// get size of a size_t
		size_t uint16_t_size = sizeof(uint16_t);	// the size of an half ==> float_16
		size_t float_size = sizeof(float);

		FILE* inFile = nullptr;
		errno_t err = fopen_s(&inFile, cacheFilePath.c_str(), "rb");
		if (err != 0)
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Open " + cacheFilePath + " in errore - Err=" + std::to_string(err)).c_str()));

		// read the buffer size
		if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t streamBufferSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		if (fread(shortBuffer, 1, 1, inFile) != 1)	// "0"
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 1!").c_str()));
		}

		// read the serialized size of the mesh id
		if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t meshIdSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		// read the mesh id
		if (fread(shortBuffer, meshIdSize, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 3!").c_str()));
		}
		// and deserialize it
		std::string meshId((char*)shortBuffer, meshIdSize);

		if (_meshId.length() > 0 && meshId != _meshId)
		{
			PLOG_DEBUG << "meshId from cache (" << meshId << ") not equal to meshId from server(" << _meshId;
			//fclose(inFile);
			//throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("meshId from cache (") + meshId + ") not equal to meshId from server (" + _meshId).c_str()));
		}

		m_meshId = meshId;

		if (terrainEdit != nullptr)
		{
			// read the serialized size of TerrainEdit
			if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
			{
				fclose(inFile);
				throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
			}
			// and deserialize it
			size_t terrainEditSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);
			
			// read TerrainEdit
			TheWorld_Utils::MemoryBuffer terrainEditBuffer;
			terrainEditBuffer.reserve(terrainEditSize);
			if (fread(terrainEditBuffer.ptr(), terrainEditSize, 1, inFile) != 1)
			{
				fclose(inFile);
				throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
			}
			terrainEditBuffer.adjustSize(terrainEditSize);
			terrainEdit->deserialize(terrainEditBuffer);
		}
		
		buffer.reserve(streamBufferSize);
		BYTE* streamBuffer = buffer.ptr();

		// reposition to the beginning of file
		int ret = fseek(inFile, 0, SEEK_SET);
		if (ret != 0)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("fseek to beginning of file error!").c_str()));
		}

		// bypass buffer size
		if (fread(shortBuffer, size_t_size, 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}

		// read the whoole buffer
		size_t s = fread(streamBuffer, streamBufferSize, 1, inFile);

		fclose(inFile);

		buffer.adjustSize(streamBufferSize);
	}

	void MeshCacheBuffer::readBufferFromDisk(std::string _meshId, std::string& _buffer, bool workDir, TerrainEdit* terrainEdit)
	{
		TheWorld_Utils::MemoryBuffer buffer;

		readBufferFromDisk(_meshId, buffer, workDir, terrainEdit);
		
		size_t size = buffer.size();

		_buffer.clear();
		_buffer.reserve(size);
		_buffer.append((char*)buffer.ptr(), size);
	}
		
	std::string MeshCacheBuffer::writeBufferToDiskCache2(std::string& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache((BYTE*)buffer.c_str(), buffer.size(), writtenMeshId, renewMeshId, workDir, terrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(std::string& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit) const
	{
		std::string bufferMeshId = writeBufferToDiskCache((BYTE*)buffer.c_str(), buffer.size(), writtenMeshId, renewMeshId, workDir, terrainEdit);
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache2(TheWorld_Utils::MemoryBuffer& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer.ptr(), buffer.size(), writtenMeshId, renewMeshId, workDir, terrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(TheWorld_Utils::MemoryBuffer& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit) const
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer.ptr(), buffer.size(), writtenMeshId, renewMeshId, workDir, terrainEdit);
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache2(const BYTE* buffer, const size_t bufferSize, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer, bufferSize, writtenMeshId, renewMeshId, workDir, terrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(const BYTE* buffer, const size_t bufferSize, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* terrainEdit) const
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer writeBufferToCache 1 ") + __FUNCTION__, "ALL");

		std::string meshFilePath = getCacheFilePath();
		if (workDir)
			meshFilePath = getWorkCacheFilePath();
		std::string tempPath = meshFilePath + ".tmp";
		std::string path = meshFilePath;

		BYTE shortBuffer[256 + 1];
		size_t bufferSize_size = 0;	// get size of a size_t

		char* movingStreamBuffer = (char*)buffer + 1;	// bypass "0"
		size_t size = 0;

		std::string bufferMeshId;
		
		if (renewMeshId)
		{
			size_t meshIdSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;			// bypass MeshId size
			bufferMeshId = std::string(movingStreamBuffer, meshIdSize);
			std::string newMeshId = generateNewMeshId();
			my_assert(meshIdSize == newMeshId.size())
			memcpy(movingStreamBuffer, newMeshId.c_str(), meshIdSize);	// replace MeshId
			movingStreamBuffer += meshIdSize;	// bypass MeshId
			writtenMeshId = newMeshId;
		}
		else
		{
			size_t meshIdSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;						// bypass MeshId size
			bufferMeshId = std::string(movingStreamBuffer, meshIdSize);
			movingStreamBuffer += meshIdSize;				// bypass MeshId
			writtenMeshId = bufferMeshId;
		}

		if (terrainEdit != nullptr)
		{
			size_t terrainEditSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;			// bypass TerrainEdit size
			TheWorld_Utils::MemoryBuffer terrainEditBuffer((BYTE*)movingStreamBuffer, terrainEditSize);
			terrainEdit->deserialize(terrainEditBuffer);
		}
				
		TheWorld_Utils::serializeToByteStream<size_t>(bufferSize, shortBuffer, bufferSize_size);

		FILE* outFile = nullptr;
		errno_t err = fopen_s(&outFile, tempPath.c_str(), "wb");
		if (err != 0)
		{
			char errMsg[512];
			strerror_s(errMsg, sizeof(errMsg), err);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Open " + tempPath + " in errore - Err=" + std::to_string(err) + " " + errMsg).c_str()));
		}

		if (fwrite(shortBuffer, bufferSize_size, 1, outFile) != 1)
		{
			fclose(outFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Write error 3!").c_str()));
		}

		if (fwrite(buffer, bufferSize, 1, outFile) != 1)
		{
			fclose(outFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Write error 3!").c_str()));
		}

		fclose(outFile);

		if (fs::exists(path.c_str()))
			if (remove(path.c_str()) != 0)
			{
				int err = errno;
				char errMsg[512];
				strerror_s(errMsg, sizeof(errMsg), err);
				throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Remove error! errno=") + std::to_string(err) + " " + errMsg + " - _doserrno=" + std::to_string(_doserrno)).c_str()));
			}

		if (rename(tempPath.c_str(), path.c_str()) != 0)
		{
			int err = errno;
			char errMsg[512];
			strerror_s(errMsg, sizeof(errMsg), err);
			throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("Rename error! errno=") + std::to_string(err) + " " + errMsg + " _doserrno=" + std::to_string(_doserrno)).c_str()));
		}

		return bufferMeshId;
	}

	void MeshCacheBuffer::generateHeightsWithNoise(size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1 ") + __FUNCTION__, "ALL");

		vectGridHeights.clear();
		vectGridHeights.resize(numVerticesPerSize * numVerticesPerSize);

		bool adjustHeights = true;
		
		if (terrainEdit->terrainType == TheWorld_Utils::TerrainEdit::TerrainType::flat)
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1.2 ") + __FUNCTION__, "Generate flat Heights");

			terrainEdit->minHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->maxHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->eastSideXPlus.minHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->eastSideXPlus.maxHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->westSideXMinus.minHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->westSideXMinus.maxHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->southSideZPlus.minHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->southSideZPlus.maxHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->northSideZMinus.minHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->northSideZMinus.maxHeight = FLAT_QUADRANTS_TERRAIN_HEIGHT;

			float altitude = FLAT_QUADRANTS_TERRAIN_HEIGHT;
			size_t maxIndex = numVerticesPerSize - 1;
			size_t idx = 0;
			for (int z = 0; z < numVerticesPerSize; z++)
				for (int x = 0; x < numVerticesPerSize; x++)
				{
					vectGridHeights[idx] = altitude;
					idx++;
				}
		}
		else if (terrainEdit->terrainType == TheWorld_Utils::TerrainEdit::TerrainType::empty)
		{
			terrainEdit->minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->eastSideXPlus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->eastSideXPlus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->westSideXMinus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->westSideXMinus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->southSideZPlus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->southSideZPlus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->northSideZMinus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			terrainEdit->northSideZMinus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			adjustHeights = false;
		}
		else
		{
			//{
			//	//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1.1 ") + __FUNCTION__, "Generate jag");

			//	TheWorld_Utils::TerrainEdit terrainEdit(TheWorld_Utils::TerrainEdit::TerrainType::jagged_1);

			//	FastNoiseLite noiseLite(terrainEdit.noise.noiseSeed);
			//	noiseLite.SetNoiseType((enum FastNoiseLite::NoiseType)terrainEdit.noise.noiseType);
			//	noiseLite.SetFrequency(terrainEdit.noise.frequency);
			//	noiseLite.SetFractalType((enum FastNoiseLite::FractalType)terrainEdit.noise.fractalType);
			//	noiseLite.SetFractalOctaves(terrainEdit.noise.fractalOctaves);
			//	noiseLite.SetFractalLacunarity(terrainEdit.noise.fractalLacunarity);
			//	noiseLite.SetFractalGain(terrainEdit.noise.fractalGain);
			//	noiseLite.SetFractalWeightedStrength(terrainEdit.noise.fractalWeightedStrength);
			//	noiseLite.SetFractalPingPongStrength(terrainEdit.noise.fractalPingPongStrength);

			//	size_t idx = 0;
			//	for (int z = 0; z < numVerticesPerSize; z++)
			//		for (int x = 0; x < numVerticesPerSize; x++)
			//		{
			//			float altitude = 0.0f;

			//			{
			//				//TheWorld_Utils::GuardProfiler profiler(std::string("EditGenerate 1.1.2 ") + __FUNCTION__, "GetNoise");

			//				float xf = (lowerXGridVertex + (x * gridStepInWU));
			//				float zf = (lowerZGridVertex + (z * gridStepInWU));
			//				altitude = noiseLite.GetNoise(xf, zf);
			//				// noises are value in range -1 to 1 we need to interpolate with heightsAmplitude
			//				altitude *= (terrainEdit.noise.heightsAmplitude / 2);
			//			}

			//			vectGridHeights[idx] = altitude;

			//			idx++;
			//		}
			//}

			{
				//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1.2 ") + __FUNCTION__, "Generate Heights");

				FastNoiseLite noiseLite(terrainEdit->noise.noiseSeed);
				noiseLite.SetNoiseType((enum FastNoiseLite::NoiseType)terrainEdit->noise.noiseType);
				//noiseLite.SetRotationType3D();
				noiseLite.SetFrequency(terrainEdit->noise.frequency);
				noiseLite.SetFractalType((enum FastNoiseLite::FractalType)terrainEdit->noise.fractalType);
				noiseLite.SetFractalOctaves(terrainEdit->noise.fractalOctaves);
				noiseLite.SetFractalLacunarity(terrainEdit->noise.fractalLacunarity);
				noiseLite.SetFractalGain(terrainEdit->noise.fractalGain);
				noiseLite.SetFractalWeightedStrength(terrainEdit->noise.fractalWeightedStrength);
				noiseLite.SetFractalPingPongStrength(terrainEdit->noise.fractalPingPongStrength);
				//noiseLite.SetCellularDistanceFunction();
				//noiseLite.SetCellularReturnType();
				//noiseLite.SetCellularJitter()

				terrainEdit->minHeight = FLT_MAX;
				terrainEdit->maxHeight = -FLT_MAX;
				terrainEdit->eastSideXPlus.minHeight = FLT_MAX;
				terrainEdit->eastSideXPlus.maxHeight = -FLT_MAX;
				terrainEdit->westSideXMinus.minHeight = FLT_MAX;
				terrainEdit->westSideXMinus.maxHeight = -FLT_MAX;
				terrainEdit->southSideZPlus.minHeight = FLT_MAX;
				terrainEdit->southSideZPlus.maxHeight = -FLT_MAX;
				terrainEdit->northSideZMinus.minHeight = FLT_MAX;
				terrainEdit->northSideZMinus.maxHeight = -FLT_MAX;

				size_t maxIndex = numVerticesPerSize - 1;

				size_t idx = 0;
				for (int z = 0; z < numVerticesPerSize; z++)
					for (int x = 0; x < numVerticesPerSize; x++)
					{
						float altitude = 0.0f;

						{
							//TheWorld_Utils::GuardProfiler profiler(std::string("EditGenerate 1.2.2 ") + __FUNCTION__, "GetNoise main");

							float xf = (lowerXGridVertex + (x * gridStepInWU)) * terrainEdit->noise.scaleFactor_JagIntensity;
							float zf = (lowerZGridVertex + (z * gridStepInWU)) * terrainEdit->noise.scaleFactor_JagIntensity;
							altitude = noiseLite.GetNoise(xf, zf);
							// noises are value in range -1 to 1 we need to interpolate with heightsAmplitude
							altitude *= (float)(terrainEdit->noise.heightsAmplitude / 2);

							if (altitude < terrainEdit->minHeight)
								terrainEdit->minHeight = altitude;
							if (altitude > terrainEdit->maxHeight)
								terrainEdit->maxHeight = altitude;

							if (x == 0)
							{
								if (altitude < terrainEdit->westSideXMinus.minHeight)
									terrainEdit->westSideXMinus.minHeight = altitude;
								if (altitude > terrainEdit->westSideXMinus.maxHeight)
									terrainEdit->westSideXMinus.maxHeight = altitude;
							}

							if (z == 0)
							{
								if (altitude < terrainEdit->northSideZMinus.minHeight)
									terrainEdit->northSideZMinus.minHeight = altitude;
								if (altitude > terrainEdit->northSideZMinus.maxHeight)
									terrainEdit->northSideZMinus.maxHeight = altitude;
							}

							if (x == maxIndex)
							{
								if (altitude < terrainEdit->eastSideXPlus.minHeight)
									terrainEdit->eastSideXPlus.minHeight = altitude;
								if (altitude > terrainEdit->eastSideXPlus.maxHeight)
									terrainEdit->eastSideXPlus.maxHeight = altitude;
							}

							if (z == maxIndex)
							{
								if (altitude < terrainEdit->southSideZPlus.minHeight)
									terrainEdit->southSideZPlus.minHeight = altitude;
								if (altitude > terrainEdit->southSideZPlus.maxHeight)
									terrainEdit->southSideZPlus.maxHeight = altitude;
							}
						}

						vectGridHeights[idx] += altitude;

						idx++;
					}
			}
		}

		if (adjustHeights)
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("EditGenerate 1.3 ") + __FUNCTION__, "Adjust desidered min height");

			if ((terrainEdit->noise.desideredMinHeight != FLT_MIN && !terrainEdit->extraValues.ignoreDesideredMinHeight)
				|| terrainEdit->extraValues.postGenerateElevation != 0)
			{
				float corrector = 0;

				if (terrainEdit->noise.desideredMinHeight != FLT_MIN && !terrainEdit->extraValues.ignoreDesideredMinHeight)
					corrector += terrainEdit->noise.desideredMinHeight - terrainEdit->minHeight;

				if (terrainEdit->extraValues.postGenerateElevation != 0)
					corrector += terrainEdit->extraValues.postGenerateElevation;

				size_t idx = 0;
				for (int z = 0; z < numVerticesPerSize; z++)
					for (int x = 0; x < numVerticesPerSize; x++)
					{
						vectGridHeights[idx] += corrector;
						idx++;
					}

				terrainEdit->minHeight += corrector;
				terrainEdit->maxHeight += corrector;

				terrainEdit->eastSideXPlus.minHeight += corrector;
				terrainEdit->eastSideXPlus.maxHeight += corrector;

				terrainEdit->westSideXMinus.minHeight += corrector;
				terrainEdit->westSideXMinus.maxHeight += corrector;

				terrainEdit->southSideZPlus.minHeight += corrector;
				terrainEdit->southSideZPlus.maxHeight += corrector;

				terrainEdit->northSideZMinus.minHeight += corrector;
				terrainEdit->northSideZMinus.maxHeight += corrector;
			}
		}
	}
		
	void MeshCacheBuffer::applyWorldModifier(int level, size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights, WorldModifier& wm)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer applyWorldModifier 1 ") + __FUNCTION__, "ALL");

		if (level != wm.getPos().getLevel())
			return;

		float sizeInWU = (numVerticesPerSize - 1) * gridStepInWU;

		float AOE = wm.getAOE();
		float wmX = wm.getPos().getPosX();
		float wmZ = wm.getPos().getPosZ();
		float wmMin = wm.getMin();
		float wmMax = wm.getMax();
		enum class TheWorld_Utils::WMType wmType = wm.getPos().getType();
		enum class TheWorld_Utils::WMFunctionType wmFunctionType = wm.getFunctionType();
		enum class TheWorld_Utils::WMOrder wmOrder = wm.getOrder();
		float wmMaxExtension = wmMax - wmMin;
		float wmStrength = wmMax - wmMin;

		if (wmType == TheWorld_Utils::WMType::none)
			return;

		if (AOE > 0)
		{
			float d1 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex, lowerZGridVertex);
			float d2 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex + sizeInWU, lowerZGridVertex);
			float d3 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex, lowerZGridVertex + sizeInWU);
			float d4 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex + sizeInWU, lowerZGridVertex + sizeInWU);

			if (d1 > AOE && d2 > AOE && d3 > AOE && d4 > AOE)
				return;
		}

		terrainEdit->eastSideXPlus.minHeight = FLT_MAX;
		terrainEdit->eastSideXPlus.maxHeight = -FLT_MAX;

		terrainEdit->westSideXMinus.minHeight = FLT_MAX;
		terrainEdit->westSideXMinus.maxHeight = -FLT_MAX;

		terrainEdit->southSideZPlus.minHeight = FLT_MAX;
		terrainEdit->southSideZPlus.maxHeight = -FLT_MAX;

		terrainEdit->northSideZMinus.minHeight = FLT_MAX;
		terrainEdit->northSideZMinus.maxHeight = -FLT_MAX;

		size_t maxIndex = numVerticesPerSize - 1;

		size_t idx = -1;
		for (int z = 0; z < numVerticesPerSize; z++)
			for (int x = 0; x < numVerticesPerSize; x++)
			{
				idx++;

				if (x == maxIndex && z == 0)
				{
					assert(x == maxIndex && z == 0);
				}
				if (x == 0 && z == maxIndex)
				{
					assert(x == 0 && z == maxIndex);
				}
				if (x == maxIndex && z == maxIndex)
				{
					assert(x == maxIndex && z == maxIndex);
				}

				float dist = TheWorld_Utils::Utils::getDistance(wmX, wmZ, (float)x * gridStepInWU + lowerXGridVertex, (float)z * gridStepInWU + lowerZGridVertex);

				if (dist > AOE)
				{
					if (idx == 0)
					{
						terrainEdit->minHeight = terrainEdit->maxHeight = vectGridHeights[idx];
					}
					else
					{
						if (vectGridHeights[idx] < terrainEdit->minHeight)
							terrainEdit->minHeight = vectGridHeights[idx];
						if (vectGridHeights[idx] > terrainEdit->maxHeight)
							terrainEdit->maxHeight = vectGridHeights[idx];
					}
					
					if (x == 0)
					{
						if (vectGridHeights[idx] < terrainEdit->westSideXMinus.minHeight)
							terrainEdit->westSideXMinus.minHeight = vectGridHeights[idx];
						if (vectGridHeights[idx] > terrainEdit->westSideXMinus.maxHeight)
							terrainEdit->westSideXMinus.maxHeight = vectGridHeights[idx];
					}

					if (z == 0)
					{
						if (vectGridHeights[idx] < terrainEdit->northSideZMinus.minHeight)
							terrainEdit->northSideZMinus.minHeight = vectGridHeights[idx];
						if (vectGridHeights[idx] > terrainEdit->northSideZMinus.maxHeight)
							terrainEdit->northSideZMinus.maxHeight = vectGridHeights[idx];
					}

					if (x == maxIndex)
					{
						if (vectGridHeights[idx] < terrainEdit->eastSideXPlus.minHeight)
							terrainEdit->eastSideXPlus.minHeight = vectGridHeights[idx];
						if (vectGridHeights[idx] > terrainEdit->eastSideXPlus.maxHeight)
							terrainEdit->eastSideXPlus.maxHeight = vectGridHeights[idx];
					}

					if (z == maxIndex)
					{
						if (vectGridHeights[idx] < terrainEdit->southSideZPlus.minHeight)
							terrainEdit->southSideZPlus.minHeight = vectGridHeights[idx];
						if (vectGridHeights[idx] > terrainEdit->southSideZPlus.maxHeight)
							terrainEdit->southSideZPlus.maxHeight = vectGridHeights[idx];
					}

					continue;
				}

				switch (wmType)
				{
				case TheWorld_Utils::WMType::elevator:
				{
					// TheWorld_Utils::WMType::elevator
					switch (wmFunctionType)
					{
						// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax
					case TheWorld_Utils::WMFunctionType::ConsiderMinMax:
					{
						switch (wmOrder)
						{
							// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax - TheWorld_Utils::WMOrder::MaxEffectOnWM
						case TheWorld_Utils::WMOrder::MaxEffectOnWM:
						{
							float coeff = (AOE - dist) / AOE;	// 0 on perimeter, 1 on WM
							float addition = wmMaxExtension * coeff + wmMin;
							vectGridHeights[idx] += addition;
						}
						break;
						// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax - TheWorld_Utils::WMOrder::MinEffectOnWM
						case TheWorld_Utils::WMOrder::MinEffectOnWM:
						{
							float coeff = dist / AOE;			// 1 on perimeter, 0 on WM
							float addition = wmMaxExtension * coeff + wmMin;
							vectGridHeights[idx] += addition;
						}
						break;
						}
					}
					break;

					// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength
					case TheWorld_Utils::WMFunctionType::ConsiderStrength:
					{
						switch (wmOrder)
						{
							// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength - TheWorld_Utils::WMOrder::MaxEffectOnWM
						case TheWorld_Utils::WMOrder::MaxEffectOnWM:
						{
							float coeff = (AOE - dist) / AOE;	// 0 on perimeter, 1 on WM
							float addition = wmStrength * coeff;
							vectGridHeights[idx] += addition;
						}
						break;

						// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength - TheWorld_Utils::WMOrder::MinEffectOnWM
						case TheWorld_Utils::WMOrder::MinEffectOnWM:
						{
							float coeff = dist / AOE;			// 1 on perimeter, 0 on WM
							float addition = wmMaxExtension * coeff;
							vectGridHeights[idx] += addition;
						}
						break;
						}
					}
					break;
					}
				}
				break;
				}

				if (idx == 0)
				{
					terrainEdit->minHeight = terrainEdit->maxHeight = vectGridHeights[idx];
				}
				else
				{
					if (vectGridHeights[idx] < terrainEdit->minHeight)
						terrainEdit->minHeight = vectGridHeights[idx];
					if (vectGridHeights[idx] > terrainEdit->maxHeight)
						terrainEdit->maxHeight = vectGridHeights[idx];
				}

				if (x == 0)
				{
					if (vectGridHeights[idx] < terrainEdit->westSideXMinus.minHeight)
						terrainEdit->westSideXMinus.minHeight = vectGridHeights[idx];
					if (vectGridHeights[idx] > terrainEdit->westSideXMinus.maxHeight)
						terrainEdit->westSideXMinus.maxHeight = vectGridHeights[idx];
				}

				if (z == 0)
				{
					if (vectGridHeights[idx] < terrainEdit->northSideZMinus.minHeight)
						terrainEdit->northSideZMinus.minHeight = vectGridHeights[idx];
					if (vectGridHeights[idx] > terrainEdit->northSideZMinus.maxHeight)
						terrainEdit->northSideZMinus.maxHeight = vectGridHeights[idx];
				}

				if (x == maxIndex)
				{
					if (vectGridHeights[idx] < terrainEdit->eastSideXPlus.minHeight)
						terrainEdit->eastSideXPlus.minHeight = vectGridHeights[idx];
					if (vectGridHeights[idx] > terrainEdit->eastSideXPlus.maxHeight)
						terrainEdit->eastSideXPlus.maxHeight = vectGridHeights[idx];
				}

				if (z == maxIndex)
				{
					if (vectGridHeights[idx] < terrainEdit->southSideZPlus.minHeight)
						terrainEdit->southSideZPlus.minHeight = vectGridHeights[idx];
					if (vectGridHeights[idx] > terrainEdit->southSideZPlus.maxHeight)
						terrainEdit->southSideZPlus.maxHeight = vectGridHeights[idx];
				}
			}
	}
		
	void MeshCacheBuffer::generateNormalsForBlendedQuadrants(size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& east_float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& south_float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, bool& requestedExit)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateNormals 1 ") + __FUNCTION__, "ALL");

		size_t numVertices = float32HeigthsBuffer.size() / sizeof(float);
		my_assert(float32HeigthsBuffer.size() == numVertices * sizeof(float));
		my_assert(numVertices == 0 || numVertices == numVerticesPerSize * numVerticesPerSize);

		size_t requiredBufferSize = numVertices == 0 ? sizeof(struct TheWorld_Utils::_RGB) : numVertices * sizeof(struct TheWorld_Utils::_RGB);
		normalsBuffer.clear(__FUNCTION__);
		normalsBuffer.reserve(requiredBufferSize);
		normalsBuffer.adjustSize(requiredBufferSize);

		struct TheWorld_Utils::_RGB* _tempNormalmapBuffer = (struct TheWorld_Utils::_RGB*)normalsBuffer.ptr();

		size_t maxIndex = numVerticesPerSize - 1;

		if (numVertices > 0)
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateNormals 1.2 ") + __FUNCTION__, "generation");

			for (int z = 0; z < numVerticesPerSize; z++)			// m_heightMapImage->get_height()
			{
				for (int x = 0; x < numVerticesPerSize; x++)		// m_heightMapImage->get_width()
				{
					if (requestedExit)
						return;
					
					float h = float32HeigthsBuffer.at<float>(x, z, numVerticesPerSize);

					Eigen::Vector3d normal;

					{
						//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateNormals 1.2.1 ") + __FUNCTION__, "calc vector");

						// h = height of the point for which we are computing the normal
						// hr = height of the point on the rigth (x growing)
						// hl = height of the point on the left (x lessening)
						// hf = height of the forward point (z growing)
						// hb = height of the backward point (z lessening)
						// step = step in WUs between points
						// we compute normal normalizing the vector (h - hr, step, h - hf) or (hl - h, step, hb - h)
						// according to https://hterrain-plugin.readthedocs.io/en/latest/ section "Procedural generation" it should be (h - hr, step, hf - h)
						
						//Eigen::Vector3d P(float(x) * gridStepInWU, h, float(z) * gridStepInWU);	// Verify
						
						if (x < maxIndex && z < maxIndex)
						{
							float hr = float32HeigthsBuffer.at<float>((size_t)(x + 1), (size_t)z, numVerticesPerSize);
							float hf = float32HeigthsBuffer.at<float>((size_t)x, (size_t)(z + 1), numVerticesPerSize);
							normal = Eigen::Vector3d(h - hr, gridStepInWU, h - hf).normalized();
																																							//normal = Eigen::Vector3d(h - hr, gridStepInWU, hf - h).normalized();
							//{	// Verify
							//	float nx = float(normal.x());
							//	float ny = float(normal.y());
							//	float nz = float(normal.z());
							//	Eigen::Vector3d PR(float(x) * gridStepInWU + gridStepInWU, hr, float(z) * gridStepInWU);
							//	Eigen::Vector3d PF(float(x) * gridStepInWU, hf, float(z) * gridStepInWU + gridStepInWU);
							//	Eigen::Vector3d normal1 = (PF - P).cross(PR - P).normalized();
							//	float n1x = float(normal1.x());
							//	float n1y = float(normal1.y());
							//	float n1z = float(normal1.z());
							//	if (!TheWorld_Utils::Utils::isEqualVectorWithLimitedPrecision(normal1, normal, 7))	// DEBUGRIC
							//		normal1 = normal;
							//}	// Verify
						}
						else if (x == maxIndex && z == maxIndex)
						{
							if (east_float32HeigthsBuffer.size() > 0 && south_float32HeigthsBuffer.size() > 0)
							{
								float hr = east_float32HeigthsBuffer.at<float>(1, z, numVerticesPerSize);
								float hf = south_float32HeigthsBuffer.at<float>(x, 1, numVerticesPerSize);
								normal = Eigen::Vector3d(h - hr, gridStepInWU, h - hf).normalized();
							}
							else
							{
								float hl = float32HeigthsBuffer.at<float>((size_t)(x - 1), (size_t)z, numVerticesPerSize);
								float hb = float32HeigthsBuffer.at<float>((size_t)x, (size_t)(z - 1), numVerticesPerSize);
								normal = Eigen::Vector3d(hl - h, gridStepInWU, hb - h).normalized();
							}
						}
						else if (x == maxIndex)	// && z < maxIndex
						{
							if (east_float32HeigthsBuffer.size() > 0)
							{
								float hr = east_float32HeigthsBuffer.at<float>(1, z, numVerticesPerSize);
								float hf = float32HeigthsBuffer.at<float>((size_t)x, (size_t)z + 1, numVerticesPerSize);
								normal = Eigen::Vector3d(h - hr, gridStepInWU, h - hf).normalized();
							}
							else
							{
								float hl = float32HeigthsBuffer.at<float>((size_t)x - 1, (size_t)z, numVerticesPerSize);
								float hf = float32HeigthsBuffer.at<float>((size_t)x, (size_t)z + 1, numVerticesPerSize);
								normal = Eigen::Vector3d(hl - h, gridStepInWU, h - hf).normalized();
							}
						}
						else	// x < maxIndex && z == maxIndex
						{
							if (south_float32HeigthsBuffer.size() > 0)
							{
								float hr = float32HeigthsBuffer.at<float>((size_t)x + 1, (size_t)z, numVerticesPerSize);
								float hf = south_float32HeigthsBuffer.at<float>((size_t)x, 1, numVerticesPerSize);
								normal = Eigen::Vector3d(h - hr, gridStepInWU, h - hf).normalized();
							}
							else
							{
								float hr = float32HeigthsBuffer.at<float>((size_t)x + 1, (size_t)z, numVerticesPerSize);
								float hb = float32HeigthsBuffer.at<float>((size_t)x, (size_t)z - 1, numVerticesPerSize);
								normal = Eigen::Vector3d(h - hr, gridStepInWU, hb - h).normalized();
							}
						}
					}

					{
						//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateNormals 1.2.3 ") + __FUNCTION__, "RGB");

								//float nx2 = float(normal.x());		// DEBUG ONLY
								//float ny2 = float(normal.y());		// DEBUG ONLY
								//float nz2 = float(normal.z());		// DEBUG ONLY
						Eigen::Vector3d packedNormal = packNormal(normal);
								//float nx = float(packedNormal.x());	// DEBUG ONLY
								//float ny = float(packedNormal.y());	// DEBUG ONLY
								//float nz = float(packedNormal.z());	// DEBUG ONLY
						//Eigen::Vector3d unpackedNormal = unpackNormal(packedNormal);	// DEBUG ONLY
						//		float nx1 = float(unpackedNormal.x());	// DEBUG ONLY
						//		float ny1 = float(unpackedNormal.y());	// DEBUG ONLY
						//		float nz1 = float(unpackedNormal.z());	// DEBUG ONLY
						struct _RGB rgb(0, 0, 0);
						rgb.r = (BYTE)(packedNormal.x() * 255);	// normals coords range from 0 to 1 but if expressed as color in a normalmap range from 0 to 255
						rgb.g = (BYTE)(packedNormal.y() * 255);
						rgb.b = (BYTE)(packedNormal.z() * 255);
						*_tempNormalmapBuffer = rgb;
						_tempNormalmapBuffer++;
					}
				}
			}
		}
		else
		{
			struct _RGB rgb(0, 0, 0);
			rgb.r = rgb.g = rgb.b = 0;
			*_tempNormalmapBuffer = rgb;
			_tempNormalmapBuffer++;
		}

		my_assert((BYTE*)_tempNormalmapBuffer - normalsBuffer.ptr() == normalsBuffer.size());
	}

	void MeshCacheBuffer::generateSplatmap(size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer,
											float slopeVerticalFactor,	// raising it slope will have higher maximum and so rocks will be more diffuse on slopes
											float slopeFlatFactor,		// raising it slope will have lesser mininum and maximum so rocks will be less diffuse on slopes
											float dirtOnRocksFactor,	// raising it dirt will be more diffuse on rocks
											float highElevationFactor,	// raising it high elevation amount will be greater at lower altitude
											float lowElevationFactor,	// raising it low elevation amount will be greater at higher altitude
											size_t splatMapMode)
	{
		assert(float32HeigthsBuffer.size() == numVerticesPerSize * numVerticesPerSize * sizeof(float));
		assert(normalsBuffer.size() == numVerticesPerSize * numVerticesPerSize * sizeof(struct TheWorld_Utils::_RGB));
		
		splatmapBuffer.clear(__FUNCTION__);
		size_t splatmapSize = numVerticesPerSize * numVerticesPerSize * sizeof(struct TheWorld_Utils::_RGBA);
		splatmapBuffer.reserve(splatmapSize);
		splatmapBuffer.adjustSize(splatmapSize);

		float diffAltitude = terrainEdit->maxHeight - terrainEdit->minHeight;
		
		for (size_t z = 0; z < numVerticesPerSize; z++)
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float h = float32HeigthsBuffer.at<float>(x, z, numVerticesPerSize);
				struct TheWorld_Utils::_RGB rgbNormal = normalsBuffer.at<TheWorld_Utils::_RGB>(x, z, numVerticesPerSize);
				// normals coords range from 0 to 255 in normals buffer we need they range from 0.0 to 1.0
				Eigen::Vector3d packedNormal((const double)(double(rgbNormal.r) / 255), (const double)(double(rgbNormal.g) / 255), (const double)(double(rgbNormal.b) / 255));
						//float nx = (float)packedNormal.x();	// DEBUG ONLY
						//float ny = (float)packedNormal.y();	// DEBUG ONLY
						//float nz = (float)packedNormal.z();	// DEBUG ONLY
				Eigen::Vector3d normal = unpackNormal(packedNormal);
						//nx = (float)normal.x();				// DEBUG ONLY
						//ny = (float)normal.y();				// DEBUG ONLY
						//nz = (float)normal.z();				// DEBUG ONLY
				//normal = normal.normalized();					// DEBUG ONLY
						//nx = (float)normal.x();				// DEBUG ONLY
						//ny = (float)normal.y();				// DEBUG ONLY
						//nz = (float)normal.z();				// DEBUG ONLY
				Eigen::Vector3d up(0.0f, 1.0f, 0.0f);
				
				// component on up direction of the normal (it is the y component of the normal if up is classic up == 0, 1, 0) - It ranges from 0=vertical terrain to 1=flat terrain
				float upComponent = float(normal.dot(up));
				
				// (1.0f - upComponent) ranges from 0=flat terrain to 1=vertical terrain so slope ranges from 0-slopeFlatFactor=flat terrain to slopeVerticalFactor-slopeFlatFactor=vertical terrain
				float slope = (slopeVerticalFactor * (1.0f - upComponent)) - slopeFlatFactor;

				// amounts range from 0.0f to 1.0f
				float rocksAmount = TheWorld_Utils::Utils::clamp<float>(slope, 0.0f, 1.0f);								// rocksAmount is max when slope is greater than 1 (vertical enough) and is null when slope is less than 0 (flat enough), otherwise it grows with the slope (if slope between 0 and 1)
				float dirtAmount = TheWorld_Utils::Utils::clamp<float>(rocksAmount * dirtOnRocksFactor, 0.0f, 1.0f);	// dirt grows on rocks
				float highElevationAmount = highElevationFactor * ((h - terrainEdit->minHeight) / diffAltitude) - lowElevationFactor;		// ranges from -lowElevationFactor to highElevationFactor-lowElevationFactor
				highElevationAmount = TheWorld_Utils::Utils::clamp<float>(highElevationAmount, 0.0f, 1.0f);				// highElevationAmount is capped from 0 and 1
				float lowElevationAmount = 1.0f - highElevationAmount;													// lowElevationAmount is the complement of highElevationAmount in range 0/1

				//if (rocksAmount == 0.0f)
				//	rocksAmount = 0.0f;

				struct TheWorld_Utils::_RGBA* rgba = splatmapBuffer.at_ptr<TheWorld_Utils::_RGBA>(x, z, numVerticesPerSize);

				float r, g, b, a;
				if (splatMapMode == 1)
				{
					r = lowElevationAmount;
					r = std::lerp(r, 0.0f, highElevationAmount);	// with higher priority: if highElevationAmount=0 r is unchanged, if highElevationAmount==>1 r==>0 (highElevationAmount can lower lowElevationAmount)
					r = std::lerp(r, 0.0f, dirtAmount);				// with higher priority: if dirtAmount=0 r is unchanged, if dirtAmount==>1 r==>0 (dirtAmount can lower lowElevationAmount)
					r = std::lerp(r, 0.0f, rocksAmount);			// with higher priority: if rocksAmount=0 r is unchanged, if rocksAmount==>1 r==>0 (rocksAmount can lower lowElevationAmount)

					g = highElevationAmount;
					g = std::lerp(g, 0.0f, dirtAmount);				// with higher priority: if dirtAmount=0 g is unchanged, if dirtAmount==>1 g==>0 (dirtAmount can lower highElevationAmount)
					g = std::lerp(g, 0.0f, rocksAmount);			// with higher priority: if rocksAmount=0 g is unchanged, if rocksAmount==>1 g==>0 (rocksAmount can lower highElevationAmount)

					b = dirtAmount;
					b = std::lerp(b, 0.0f, rocksAmount);			// with higher priority: if rocksAmount=0 b is unchanged, if rocksAmount==>1 b==>0 (rocksAmount can lower dirtAmount)

					a = rocksAmount;
				}
				else
				{
					a = rocksAmount;
					a = std::lerp(a, 0.0f, dirtAmount);
					a = std::lerp(a, 0.0f, highElevationAmount);
					a = std::lerp(a, 0.0f, lowElevationAmount);

					b = dirtAmount;
					b = std::lerp(b, 0.0f, highElevationAmount);
					b = std::lerp(b, 0.0f, lowElevationAmount);

					g = highElevationAmount;
					g = std::lerp(g, 0.0f, lowElevationAmount);

					r = lowElevationAmount;
				}

				rgba->r = BYTE(r * 255);
				rgba->g = BYTE(g * 255);
				rgba->b = BYTE(b * 255);
				rgba->a = BYTE(a * 255);
			}
	}

	void MeshCacheBuffer::deprecated_generateNormals(size_t numVerticesPerSize, float gridStepInWU, std::vector<float>& vectGridHeights, BYTE* normalsBuffer, const size_t normalsBufferSize, size_t& usedBufferSize)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals1 1 ") + __FUNCTION__, "ALL");

		usedBufferSize = 0;

		size_t requiredBufferSize = vectGridHeights.size() == 0 ? sizeof(struct TheWorld_Utils::_RGB) : vectGridHeights.size() * sizeof(struct TheWorld_Utils::_RGB);

		my_assert(vectGridHeights.size() == 0 || vectGridHeights.size() == numVerticesPerSize * numVerticesPerSize);
		my_assert(normalsBufferSize >= requiredBufferSize);

		struct TheWorld_Utils::_RGB* _tempNormalmapBuffer = (struct TheWorld_Utils::_RGB*)normalsBuffer;

		size_t maxIndex = numVerticesPerSize - 1;

		if (vectGridHeights.size() > 0)
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals1 1.2 ") + __FUNCTION__, "generation");

			size_t idx = 0;
			for (int z = 0; z < numVerticesPerSize; z++)			// m_heightMapImage->get_height()
			{
				for (int x = 0; x < numVerticesPerSize; x++)		// m_heightMapImage->get_width()
				{
					float h = vectGridHeights[idx];

					Eigen::Vector3d v;

					{
						//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals1 1.2.1 ") + __FUNCTION__, "calc vector");

						// h = height of the point for which we are computing the normal
						// hr = height of the point on the rigth
						// hl = height of the point on the left
						// hf = height of the forward point (z growing)
						// hb = height of the backward point (z lessening)
						// step = step in WUs between points
						// we compute normal normalizing the vector (h - hr, step, h - hf) or (hl - h, step, hb - h)
						// according to https://hterrain-plugin.readthedocs.io/en/latest/ section "Procedural generation" it should be (h - hr, step, hf - h)
						//Eigen::Vector3d P((float)x, h, (float)z);	// Verify
						if (x < maxIndex && z < maxIndex)
						{
							float hr = vectGridHeights[(size_t)z * numVerticesPerSize + (size_t)x + 1];
							float hf = vectGridHeights[((size_t)z + 1) * numVerticesPerSize + (size_t)x];
							v = Eigen::Vector3d(h - hr, gridStepInWU, h - hf).normalized();
							//{		// Verify
							//	Eigen::Vector3d PR((float)(x + gridStepInWU), hr, (float)z);
							//	Eigen::Vector3d PF((float)x, hf, (float)(z + gridStepInWU));
							//	Eigen::Vector3d normal1 = (PF - P).cross(PR - P).normalized();
							//	if (!equal(normal1, normal))	// DEBUGRIC
							//		m_viewer->Globals()->debugPrint("Normal=" + String(normal) + " - Normal1= " + String(normal1));
							//}
						}
						else
						{
							if (x == maxIndex && z == 0)
							{
								float hf = vectGridHeights[((size_t)z + 1) * numVerticesPerSize + x];
								float hl = vectGridHeights[(size_t)z * numVerticesPerSize + x - 1];
								v = Eigen::Vector3d(hl - h, gridStepInWU, h - hf).normalized();
								//{		// Verify
								//	Eigen::Vector3d PL((float)(x - gridStepInWU), hl, (float)z);
								//	Eigen::Vector3d PF((float)x, hf, (float)(z + gridStepInWU));
								//	Eigen::Vector3d normal1 = (PL - P).cross(PF - P).normalized();
								//	if (!equal(normal1, normal))	// DEBUGRIC
								//		m_viewer->Globals()->debugPrint("Normal=" + String(normal) + " - Normal1= " + String(normal1));
								//}
							}
							else if (x == 0 && z == maxIndex)
							{
								float hr = vectGridHeights[(size_t)z * numVerticesPerSize + x + 1];
								float hb = vectGridHeights[(size_t)(z - 1) * numVerticesPerSize + x];
								v = Eigen::Vector3d(h - hr, gridStepInWU, hb - h).normalized();
								//{		// Verify
								//	Eigen::Vector3d PR((float)(x + gridStepInWU), hr, (float)z);
								//	Eigen::Vector3d PB((float)(x), hb, (float)(z - gridStepInWU));
								//	Eigen::Vector3d normal1 = (PR - P).cross(PB - P).normalized();
								//	if (!equal(normal1, normal))	// DEBUGRIC
								//		m_viewer->Globals()->debugPrint("Normal=" + String(normal) + " - Normal1= " + String(normal1));
								//}
							}
							else
							{
								float hl = vectGridHeights[(size_t)z * numVerticesPerSize + x - 1];
								float hb = vectGridHeights[(size_t)(z - 1) * numVerticesPerSize + x];
								v = Eigen::Vector3d(hl - h, gridStepInWU, hb - h).normalized();
								//{		// Verify
								//	Eigen::Vector3d PB((float)x, hb, (float)(z - gridStepInWU));
								//	Eigen::Vector3d PL((float)(x - gridStepInWU), hl, (float)z);
								//	Eigen::Vector3d normal1 = (PB - P).cross(PL - P).normalized();
								//	if (!equal(normal1, normal))	// DEBUGRIC
								//		m_viewer->Globals()->debugPrint("Normal=" + String(normal) + " - Normal1= " + String(normal1));
								//}
							}
						}
					}
					
					Eigen::Vector3d normal;
					
					{
						//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals1 1.2.2 ") + __FUNCTION__, "normalize");

						normal = v.normalized();
					}

					{
						//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals1 1.2.3 ") + __FUNCTION__, "RGB");

						Eigen::Vector3d packNormal(Eigen::Vector3d normal);
						Eigen::Vector3d packedNormal = packNormal(normal);
						struct _RGB rgb;
						rgb.r = (BYTE)(packedNormal.x() * 255);	// normals coord are from 0 to 1 but if expressed as color in a normlamap are from 0 to 255
						rgb.g = (BYTE)(packedNormal.y() * 255);
						rgb.b = (BYTE)(packedNormal.z() * 255);
						*_tempNormalmapBuffer = rgb;
						_tempNormalmapBuffer++;
					}

					idx++;
				}
			}
		}
		else
		{
			struct _RGB rgb(0, 0, 0);
			rgb.r = rgb.g = rgb.b = 0;
			*_tempNormalmapBuffer = rgb;
			_tempNormalmapBuffer++;
		}

		usedBufferSize = (BYTE*)_tempNormalmapBuffer - normalsBuffer;
	}

	void MeshCacheBuffer::deprecated_generateNormals(size_t numVerticesPerSize, float gridStepInWU, std::vector<float>& vectGridHeights, TheWorld_Utils::MemoryBuffer& normalsBuffer)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer deprecated_generateNormals2 1 ") + __FUNCTION__, "serialize normals");

		my_assert(vectGridHeights.size() == 0 || vectGridHeights.size() == numVerticesPerSize * numVerticesPerSize);

		size_t requiredBufferSize = vectGridHeights.size() == 0 ? sizeof(struct TheWorld_Utils::_RGB) : vectGridHeights.size() * sizeof(struct TheWorld_Utils::_RGB);

		normalsBuffer.reserve(requiredBufferSize);

		size_t usedBufferSize = 0;
		deprecated_generateNormals(numVerticesPerSize, gridStepInWU, vectGridHeights, normalsBuffer.ptr(), normalsBuffer.reserved(), usedBufferSize);

		normalsBuffer.adjustSize(usedBufferSize);
	}

	void MeshCacheBuffer::setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, std::string& buffer)
	{
		TheWorld_Utils::MemoryBuffer _buffer;
		setBufferFromCacheQuadrantData(numVerticesPerSize, cacheQuadrantData, _buffer);
		buffer.clear();
		buffer.reserve(_buffer.size());
		buffer.assign((char*)_buffer.ptr(), _buffer.size());
	}

	void MeshCacheBuffer::setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1 ") + __FUNCTION__, "ALL");

		size_t size = 0;

		size_t numHeights16 = cacheQuadrantData.heights16Buffer != nullptr ? cacheQuadrantData.heights16Buffer->size() / sizeof(uint16_t) : 0;
		my_assert(cacheQuadrantData.heights16Buffer == nullptr || cacheQuadrantData.heights16Buffer->size() == numHeights16 * sizeof(uint16_t));
		size_t numHeights32 = cacheQuadrantData.heights32Buffer != nullptr ? cacheQuadrantData.heights32Buffer->size() / sizeof(float) : 0;
		my_assert(cacheQuadrantData.heights32Buffer == nullptr || cacheQuadrantData.heights32Buffer->size() == numHeights32 * sizeof(float));
		size_t numNormals = cacheQuadrantData.normalsBuffer != nullptr ? cacheQuadrantData.normalsBuffer->size() / sizeof(TheWorld_Utils::_RGB) : 0;
		my_assert(cacheQuadrantData.normalsBuffer == nullptr || cacheQuadrantData.normalsBuffer->size() == numNormals * sizeof(TheWorld_Utils::_RGB));
		size_t numSplatmapVertices = cacheQuadrantData.splatmapBuffer != nullptr ? cacheQuadrantData.splatmapBuffer->size() / sizeof(TheWorld_Utils::_RGBA) : 0;
		my_assert(cacheQuadrantData.splatmapBuffer == nullptr || cacheQuadrantData.splatmapBuffer->size() == numSplatmapVertices * sizeof(TheWorld_Utils::_RGBA));
		size_t numColormapVertices = cacheQuadrantData.colormapBuffer != nullptr ? cacheQuadrantData.colormapBuffer->size() / sizeof(TheWorld_Utils::_RGBA) : 0;
		my_assert(cacheQuadrantData.colormapBuffer == nullptr || cacheQuadrantData.colormapBuffer->size() == numColormapVertices * sizeof(TheWorld_Utils::_RGBA));
		size_t numGlobalmapVertices = cacheQuadrantData.globalmapBuffer != nullptr ? cacheQuadrantData.globalmapBuffer->size() / sizeof(TheWorld_Utils::_RGB) : 0;
		my_assert(cacheQuadrantData.globalmapBuffer == nullptr || cacheQuadrantData.globalmapBuffer->size() == numGlobalmapVertices * sizeof(TheWorld_Utils::_RGB));

		my_assert(numHeights16 == numHeights32);
		my_assert(numHeights16 == 0 || numHeights16 == numVerticesPerSize * numVerticesPerSize);
		my_assert(numNormals == 0 || numNormals == numVerticesPerSize * numVerticesPerSize);
		my_assert(numSplatmapVertices == 0 || numSplatmapVertices == numVerticesPerSize * numVerticesPerSize);
		my_assert(numColormapVertices == 0 || numColormapVertices == numVerticesPerSize * numVerticesPerSize);
		my_assert(numGlobalmapVertices == 0 || numGlobalmapVertices == numVerticesPerSize * numVerticesPerSize);

		static std::recursive_mutex s_mtxEmptyBuffer;
		static TheWorld_Utils::MemoryBuffer s_emptyFloat16HeightsBuffer;
		TheWorld_Utils::MemoryBuffer* _float16HeightsBuffer = cacheQuadrantData.heights16Buffer;
		static TheWorld_Utils::MemoryBuffer s_emptyFloat32HeightsBuffer;
		TheWorld_Utils::MemoryBuffer* _float32HeightsBuffer = cacheQuadrantData.heights32Buffer;
		static TheWorld_Utils::MemoryBuffer s_emptyNormalsBuffer;
		TheWorld_Utils::MemoryBuffer* _normalsBuffer = cacheQuadrantData.normalsBuffer;
		static TheWorld_Utils::MemoryBuffer s_emptySplatmapBuffer;
		TheWorld_Utils::MemoryBuffer* _splatmapBuffer = cacheQuadrantData.splatmapBuffer;

		if (numHeights16 == 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.1 ") + __FUNCTION__, "gen empty quadrants heights");

			_float16HeightsBuffer = &s_emptyFloat16HeightsBuffer;
			_float32HeightsBuffer = &s_emptyFloat32HeightsBuffer;

			numHeights32 = numHeights16 = numVerticesPerSize * numVerticesPerSize;

			{
				std::lock_guard<std::recursive_mutex> lock(s_mtxEmptyBuffer);
				
				size_t float16HeightsBufferSize = numVerticesPerSize * numVerticesPerSize * sizeof(uint16_t);
				size_t float32HeightsBufferSize = numVerticesPerSize * numVerticesPerSize * sizeof(float);

				if (s_emptyFloat16HeightsBuffer.size() == 0 || s_emptyFloat32HeightsBuffer.size() == 0)
				{
					s_emptyFloat16HeightsBuffer.reserve(float16HeightsBufferSize);
					uint16_t* float16HeightsBufferPtr = (uint16_t*)s_emptyFloat16HeightsBuffer.ptr();
					s_emptyFloat32HeightsBuffer.reserve(float32HeightsBufferSize);
					float* float32HeightsBufferPtr = (float*)s_emptyFloat32HeightsBuffer.ptr();

					TheWorld_Utils::FLOAT_32 f(EMPTY_QUADRANTS_TERRAIN_HEIGHT);
					uint16_t emptyHalf = half_from_float(f.u32);
					{		// first impl.
						for (size_t z = 0; z < numVerticesPerSize; z++)
							for (size_t x = 0; x < numVerticesPerSize; x++)
							{
								*float16HeightsBufferPtr = emptyHalf;
								float16HeightsBufferPtr++;
								*float32HeightsBufferPtr = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
								float32HeightsBufferPtr++;
							}
					}

					assert((BYTE*)float16HeightsBufferPtr - s_emptyFloat16HeightsBuffer.ptr() == float16HeightsBufferSize);
					s_emptyFloat16HeightsBuffer.adjustSize(float16HeightsBufferSize);
					assert((BYTE*)float32HeightsBufferPtr - s_emptyFloat32HeightsBuffer.ptr() == float32HeightsBufferSize);
					s_emptyFloat32HeightsBuffer.adjustSize(float32HeightsBufferSize);
				}
				else
				{
					assert(float16HeightsBufferSize == _float16HeightsBuffer->size());
					assert(float32HeightsBufferSize == _float32HeightsBuffer->size());
				}

				//// useless as texture are filled in ShaderTerrainData::resetMaterialParams if they are empty
				//_flatNormalsBuffer = &s_flatNormalsBuffer;
				//numNormals = numVerticesPerSize * numVerticesPerSize;
				//size_t floatNormalsBufferSize = numVerticesPerSize * numVerticesPerSize * sizeof(struct TheWorld_Utils::_RGB);
				//if (s_flatNormalsBuffer.size() == 0)
				//{
				//	s_flatNormalsBuffer.reserve(floatNormalsBufferSize);
				//	struct TheWorld_Utils::_RGB* _normalBufferPtr = (struct TheWorld_Utils::_RGB*)s_flatNormalsBuffer.ptr();

				//	Eigen::Vector3d normal(0.0f, 1.0f, 0.0f);
				//	//float nx = float(normal.x());		// DEBUG ONLY
				//	//float ny = float(normal.y());		// DEBUG ONLY
				//	//float nz = float(normal.z());		// DEBUG ONLY
				//	Eigen::Vector3d packedNormal = packNormal(normal);
				//	//float pnx = float(packedNormal.x());		// DEBUG ONLY
				//	//float pny = float(packedNormal.y());		// DEBUG ONLY
				//	//float pnz = float(packedNormal.z());		// DEBUG ONLY
				//	struct TheWorld_Utils::_RGB flatBufferNormal;
				//	flatBufferNormal.r = (BYTE)(packedNormal.x() * 255);
				//	flatBufferNormal.g = (BYTE)(packedNormal.y() * 255);
				//	flatBufferNormal.b = (BYTE)(packedNormal.z() * 255);
				//	for (size_t z = 0; z < numVerticesPerSize; z++)
				//		for (size_t x = 0; x < numVerticesPerSize; x++)
				//		{
				//			*_normalBufferPtr = flatBufferNormal;
				//			_normalBufferPtr++;
				//		}

				//	assert((BYTE*)_normalBufferPtr - s_flatNormalsBuffer.ptr() == floatNormalsBufferSize);
				//	s_flatNormalsBuffer.adjustSize(floatNormalsBufferSize);
				//}
				//else
				//{
				//	assert(floatNormalsBufferSize == s_flatNormalsBuffer.size());
				//}

				//// useless as texture are filled in ShaderTerrainData::resetMaterialParams if they are empty
				//_flatSplatmapBuffer = &s_flatSplatmapBuffer;
				//numSplatmapVertices = numVerticesPerSize * numVerticesPerSize;
				//size_t floatSplatmapBufferSize = numVerticesPerSize * numVerticesPerSize * sizeof(struct TheWorld_Utils::_RGBA);
				//if (s_flatSplatmapBuffer.size() == 0)
				//{
				//	s_flatSplatmapBuffer.reserve(floatSplatmapBufferSize);
				//	struct TheWorld_Utils::_RGBA* _splatmapBufferPtr = (struct TheWorld_Utils::_RGBA*)s_flatSplatmapBuffer.ptr();

				//	struct TheWorld_Utils::_RGBA flatBufferSplatmap;	flatBufferSplatmap.r = 255;	flatBufferSplatmap.g = 0;	flatBufferSplatmap.b = 0;	flatBufferSplatmap.a = 0;
				//	for (size_t z = 0; z < numVerticesPerSize; z++)
				//		for (size_t x = 0; x < numVerticesPerSize; x++)
				//		{
				//			*_splatmapBufferPtr = flatBufferSplatmap;
				//			_splatmapBufferPtr++;
				//		}

				//	assert((BYTE*)_splatmapBufferPtr - s_flatSplatmapBuffer.ptr() == floatSplatmapBufferSize);
				//	s_flatSplatmapBuffer.adjustSize(floatSplatmapBufferSize);
				//}
				//else
				//{
				//	assert(floatSplatmapBufferSize == s_flatSplatmapBuffer.size());
				//}
			}
		}

		size_t float16HeightmapSize = numHeights16 * sizeof(uint16_t);
		size_t float32HeightmapSize = numHeights32 * sizeof(float);
		size_t normalmapSize = numNormals > 0 ? numNormals * sizeof(TheWorld_Utils::_RGB) : sizeof(TheWorld_Utils::_RGB);
		size_t splatmapSize = numSplatmapVertices > 0 ? numSplatmapVertices * sizeof(TheWorld_Utils::_RGBA) : sizeof(TheWorld_Utils::_RGBA);
		size_t colormapSize = numColormapVertices > 0 ? numColormapVertices * sizeof(TheWorld_Utils::_RGBA) : sizeof(TheWorld_Utils::_RGBA);
		size_t globalmapSize = numGlobalmapVertices > 0 ? numGlobalmapVertices * sizeof(TheWorld_Utils::_RGB) : sizeof(TheWorld_Utils::_RGB);

		size_t terrainEditValueSize = cacheQuadrantData.terrainEditValues == nullptr ? 0 : cacheQuadrantData.terrainEditValues->size();
		size_t streamBufferSize = 1 /* "0" */
			+ sizeof(size_t) + cacheQuadrantData.meshId.length()
			+ sizeof(size_t) + terrainEditValueSize
			+ sizeof(size_t); /* numheights */
		if (numHeights16 > 0)
		{
			streamBufferSize = streamBufferSize
				+ sizeof(float) /*min_altitude*/ 
				+ sizeof(float) /*max_altitude*/ 
				+ float16HeightmapSize 
				+ float32HeightmapSize 
				+ normalmapSize
				+ splatmapSize
				+ colormapSize
				+ globalmapSize;
		}
		
		buffer.reserve(streamBufferSize);
		BYTE* _streamBuffer = buffer.ptr();

		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.2 ") + __FUNCTION__, "copy meshId/edit values");

			memcpy(_streamBuffer, "0", 1);
			_streamBuffer++;

			TheWorld_Utils::serializeToByteStream<size_t>(cacheQuadrantData.meshId.length(), _streamBuffer, size);
			_streamBuffer += size;

			memcpy(_streamBuffer, cacheQuadrantData.meshId.c_str(), cacheQuadrantData.meshId.length());
			_streamBuffer += cacheQuadrantData.meshId.length();

			TheWorld_Utils::serializeToByteStream<size_t>(terrainEditValueSize, _streamBuffer, size);
			_streamBuffer += size;

			if (terrainEditValueSize > 0)
			{
				TerrainEdit terrainEdit;
				terrainEdit.deserialize(*cacheQuadrantData.terrainEditValues);
				if (numHeights16 > 0 && terrainEdit.terrainType == TerrainEdit::TerrainType::empty)
					terrainEdit.heightsNeedRegen = false;
				//memcpy(_streamBuffer, cacheQuadrantData.terrainEditValues->ptr(), terrainEditValueSize);
				memcpy(_streamBuffer, &terrainEdit, terrainEditValueSize);
				_streamBuffer += terrainEditValueSize;
				cacheQuadrantData.minHeight = terrainEdit.minHeight;
				cacheQuadrantData.maxHeight = terrainEdit.maxHeight;
			}
		}

		size = 0;
		TheWorld_Utils::serializeToByteStream<size_t>(numHeights16, _streamBuffer, size);
		_streamBuffer += size;

		if (numHeights16 > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.3 ") + __FUNCTION__, "copy heights");

			TheWorld_Utils::serializeToByteStream<float>(cacheQuadrantData.minHeight, _streamBuffer, size);
			_streamBuffer += size;

			TheWorld_Utils::serializeToByteStream<float>(cacheQuadrantData.maxHeight, _streamBuffer, size);
			_streamBuffer += size;

			memcpy(_streamBuffer, _float16HeightsBuffer->ptr(), float16HeightmapSize);
			_streamBuffer += float16HeightmapSize;

			memcpy(_streamBuffer, _float32HeightsBuffer->ptr(), float32HeightmapSize);
			_streamBuffer += float32HeightmapSize;
		}

		if (numNormals > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.4 ") + __FUNCTION__, "copy normals");

			memcpy(_streamBuffer, _normalsBuffer->ptr(), normalmapSize);
			_streamBuffer += normalmapSize;
		}
		else
		{
			struct TheWorld_Utils::_RGB rgb(0, 0, 0);
			rgb.r = rgb.g = rgb.b = 0;
			memcpy(_streamBuffer, (BYTE*)&rgb, sizeof(struct TheWorld_Utils::_RGB));
			_streamBuffer += sizeof(struct TheWorld_Utils::_RGB);
		}

		if (numSplatmapVertices > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.4 ") + __FUNCTION__, "copy splatmap");

			memcpy(_streamBuffer, _splatmapBuffer->ptr(), splatmapSize);
			_streamBuffer += splatmapSize;
		}
		else
		{
			struct TheWorld_Utils::_RGBA rgba(0, 0, 0, 0);
			rgba.r = rgba.g = rgba.b = rgba.a = 0;
			memcpy(_streamBuffer, (BYTE*)&rgba, sizeof(struct TheWorld_Utils::_RGBA));
			_streamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
		}

		if (numColormapVertices > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.4 ") + __FUNCTION__, "copy colormap");

			memcpy(_streamBuffer, cacheQuadrantData.colormapBuffer->ptr(), colormapSize);
			_streamBuffer += colormapSize;
		}
		else
		{
			struct TheWorld_Utils::_RGBA rgba;
			rgba.r = rgba.g = rgba.b = rgba.a = 0;
			memcpy(_streamBuffer, (BYTE*)&rgba, sizeof(struct TheWorld_Utils::_RGBA));
			_streamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
		}

		if (numGlobalmapVertices > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData 1.4 ") + __FUNCTION__, "copy globalmap");

			memcpy(_streamBuffer, cacheQuadrantData.globalmapBuffer->ptr(), globalmapSize);
			_streamBuffer += globalmapSize;
		}
		else
		{
			struct TheWorld_Utils::_RGB rgb;
			rgb.r = rgb.g = rgb.b = 0;
			memcpy(_streamBuffer, (BYTE*)&rgb, sizeof(struct TheWorld_Utils::_RGB));
			_streamBuffer += sizeof(struct TheWorld_Utils::_RGB);
		}

		assert(_streamBuffer - buffer.ptr() == streamBufferSize);
		buffer.adjustSize(streamBufferSize);
	}

	void MeshCacheBuffer::setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, std::string& buffer)
	{
		TheWorld_Utils::MemoryBuffer _buffer;
		setEmptyBuffer(numVerticesPerSize, meshId, _buffer);
		buffer.clear();
		buffer.reserve(_buffer.size());
		buffer.assign((char*)_buffer.ptr(), _buffer.size());
	}
			
	void MeshCacheBuffer::setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, TheWorld_Utils::MemoryBuffer& buffer)
	{
		meshId = generateNewMeshIdForEmptyBuffer();
		
		TheWorld_Utils::MeshCacheBuffer::CacheQuadrantData cacheQuadrantData;
		cacheQuadrantData.meshId = meshId;
		//BYTE shortBuffer[256 + 1];	size_t size = 0;
		//TheWorld_Utils::serializeToByteStream<size_t>(sizeof(size_t), shortBuffer, size);
		TheWorld_Utils::TerrainEdit terrainEdit;
		terrainEdit.heightsNeedRegen = true;
		terrainEdit.normalsNeedRegen = true;
		terrainEdit.splatmapNeedRegen(true);
		terrainEdit.globalmapNeedRegen = true;
		terrainEdit.colormapNeedRegen = true;
		TheWorld_Utils::MemoryBuffer terrainEditValuesBuffer((BYTE*)&terrainEdit, terrainEdit.size);
		cacheQuadrantData.minHeight = 0.0f;
		cacheQuadrantData.maxHeight = 0.0f;
		cacheQuadrantData.terrainEditValues = &terrainEditValuesBuffer;
		TheWorld_Utils::MemoryBuffer emptyFloat16HeightsBuffer;
		TheWorld_Utils::MemoryBuffer emptyFloat32HeightsBuffer;
		TheWorld_Utils::MemoryBuffer emptyNormalBuffer;
		TheWorld_Utils::MemoryBuffer emptySplatmapBuffer;
		TheWorld_Utils::MemoryBuffer emptyColormapBuffer;
		TheWorld_Utils::MemoryBuffer emptyGlobalmapBuffer;
		cacheQuadrantData.heights16Buffer = &emptyFloat16HeightsBuffer;
		cacheQuadrantData.heights32Buffer = &emptyFloat32HeightsBuffer;
		cacheQuadrantData.normalsBuffer = &emptyNormalBuffer;
		cacheQuadrantData.splatmapBuffer = &emptySplatmapBuffer;
		cacheQuadrantData.colormapBuffer = &emptyColormapBuffer;
		cacheQuadrantData.globalmapBuffer = &emptyGlobalmapBuffer;

		setBufferFromCacheQuadrantData(numVerticesPerSize, cacheQuadrantData, buffer);
	}

	uint16_t MeshCacheBuffer::halfFromFloat(uint32_t f)
	{
		return half_from_float(f);
	}
	uint32_t MeshCacheBuffer::halfToFloat(uint16_t f)
	{
		return half_to_float(f);
	}


	void MeshCacheBuffer::setBufferFromHeights(std::string meshId, size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::MemoryBuffer& terrainEditValuesBuffer, std::vector<float>& vectGridHeights, std::string& buffer, float& minAltitude, float& maxAltitude, bool generateNormals)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1 ") + __FUNCTION__, "ALL");

		// get size of a size_t
		size_t size_t_size = sizeof(size_t);
		size_t uint16_t_size = sizeof(uint16_t);	// the size of an half ==> float_16
		size_t float_size = sizeof(float);

		size_t size = 0;

		size_t vectSize = vectGridHeights.size();
		size_t float16HeightmapSize = vectSize * uint16_t_size;
		size_t float32HeightmapSize = vectSize * float_size;
		size_t normalmapSize = generateNormals ? vectSize * sizeof(struct _RGB) : sizeof(struct _RGB);
		size_t streamBufferSize = 1 /* "0" */ 
			+ size_t_size + meshId.length()
			+ size_t_size + terrainEditValuesBuffer.size()
			+ size_t_size /* numheights */ + float_size /*min_altitude*/ + float_size /*max_altitude*/ + float16HeightmapSize + float32HeightmapSize + normalmapSize;

		BYTE* streamBuffer = nullptr;
		BYTE* _streamBuffer = nullptr;
		BYTE* tempFloat16HeithmapBuffer = nullptr;
		uint16_t* _tempFloat16HeithmapBuffer = nullptr;
		BYTE* tempNormalmapBuffer = nullptr;
		struct _RGB* _tempNormalmapBuffer = nullptr;

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.1 ") + __FUNCTION__, "calloc");

			_streamBuffer = streamBuffer = (BYTE*)calloc(1, streamBufferSize);
			if (streamBuffer == nullptr)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Allocation error")).c_str()));

			if (float16HeightmapSize > 0)
			{
				_tempFloat16HeithmapBuffer = (uint16_t*)(tempFloat16HeithmapBuffer = (BYTE*)calloc(1, float16HeightmapSize));
				if (tempFloat16HeithmapBuffer == nullptr)
				{
					free(streamBuffer);
					throw(std::exception((std::string(__FUNCTION__) + std::string(" Allocation error")).c_str()));
				}
			}

			if (normalmapSize > 0)
			{
				_tempNormalmapBuffer = (struct _RGB*)(tempNormalmapBuffer = (BYTE*)calloc(1, normalmapSize));
				if (tempNormalmapBuffer == nullptr)
				{
					free(streamBuffer);
					free(tempFloat16HeithmapBuffer);
					throw(std::exception((std::string(__FUNCTION__) + std::string(" Allocation error")).c_str()));
				}
			}
		}

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.2 ") + __FUNCTION__, "meshId/edit values");

			memcpy(_streamBuffer, "0", 1);
			_streamBuffer++;

			TheWorld_Utils::serializeToByteStream<size_t>(meshId.length(), _streamBuffer, size);
			_streamBuffer += size;

			memcpy(_streamBuffer, meshId.c_str(), meshId.length());
			_streamBuffer += meshId.length();

			TheWorld_Utils::serializeToByteStream<size_t>(terrainEditValuesBuffer.size(), _streamBuffer, size);
			_streamBuffer += size;

			if (terrainEditValuesBuffer.size() > 0)
			{
				memcpy(_streamBuffer, terrainEditValuesBuffer.ptr(), terrainEditValuesBuffer.size());
				_streamBuffer += terrainEditValuesBuffer.size();
			}
		}

		size = 0;
		TheWorld_Utils::serializeToByteStream<size_t>(vectSize, _streamBuffer, size);
		_streamBuffer += size;

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.3 ") + __FUNCTION__, "loop heights");
		
			minAltitude = 0, maxAltitude = 0;
			bool first = true;
			size_t idx = 0;
			if (vectSize > 0 && _tempFloat16HeithmapBuffer != nullptr)
			{
				{
					//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.3.1 ") + __FUNCTION__, "serialize heights");

					for (int z = 0; z < numVerticesPerSize; z++)			// m_heightMapImage->get_height()
					{
						for (int x = 0; x < numVerticesPerSize; x++)		// m_heightMapImage->get_width()
						{
							TheWorld_Utils::FLOAT_32 f(0);
							{

								// supposing heights are ordered line by line
								f.f32 = vectGridHeights[idx];

								*_tempFloat16HeithmapBuffer = half_from_float(f.u32);
								_tempFloat16HeithmapBuffer++;
							}

							if (first)
							{
								first = false;
								minAltitude = maxAltitude = f.f32;
							}
							else
							{
								if (f.f32 > maxAltitude)
									maxAltitude = f.f32;
								if (f.f32 < minAltitude)
									minAltitude = f.f32;
							}

							idx++;
						}
					}
				}

				std::vector<float> heightsEmptyVector;
				std::vector<float>* heightsVectorPointer = &heightsEmptyVector;
				if (generateNormals)
					heightsVectorPointer = &vectGridHeights;

				size_t usedBufferSize = 0;
				this->deprecated_generateNormals(numVerticesPerSize, gridStepInWU, *heightsVectorPointer, (BYTE*)_tempNormalmapBuffer, normalmapSize, usedBufferSize);
				_tempNormalmapBuffer = (struct TheWorld_Utils::_RGB*)((BYTE*)_tempNormalmapBuffer + usedBufferSize);
				my_assert(normalmapSize == usedBufferSize);

				{
					//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.3.3 ") + __FUNCTION__, "memcpy");

					TheWorld_Utils::serializeToByteStream<float>(minAltitude, _streamBuffer, size);
					_streamBuffer += size;

					TheWorld_Utils::serializeToByteStream<float>(maxAltitude, _streamBuffer, size);
					_streamBuffer += size;

					if (float16HeightmapSize > 0)
					{
						size_t tempFloat16HeithmapBufferSize = (BYTE*)_tempFloat16HeithmapBuffer - tempFloat16HeithmapBuffer;
						assert(float16HeightmapSize == tempFloat16HeithmapBufferSize);
						memcpy(_streamBuffer, tempFloat16HeithmapBuffer, float16HeightmapSize);	// append normal map
						_streamBuffer += float16HeightmapSize;
					}

					if (float32HeightmapSize > 0)
					{
						memcpy(_streamBuffer, &vectGridHeights[0], float32HeightmapSize);	// append normal map
						_streamBuffer += float32HeightmapSize;
					}

					if (normalmapSize > 0)
					{
						size_t tempNormalmapBufferSize = (BYTE*)_tempNormalmapBuffer - tempNormalmapBuffer;
						assert(normalmapSize == tempNormalmapBufferSize);
						memcpy(_streamBuffer, tempNormalmapBuffer, normalmapSize);	// append normal map
						_streamBuffer += normalmapSize;
					}

					assert(streamBufferSize == _streamBuffer - streamBuffer);
				}
			}
		}

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.4 ") + __FUNCTION__, "copy out buffer");
			buffer = std::string((char*)streamBuffer, streamBufferSize);
		}
		
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.5 ") + __FUNCTION__, "free");
			if (float16HeightmapSize > 0)
				free(tempFloat16HeithmapBuffer);
			if (normalmapSize > 0)
				free(tempNormalmapBuffer);
			free(streamBuffer);
		}

		m_meshId = meshId;
	}

	//bool MeshCacheBuffer::checkBlendingQuadrantNeeded(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
	//	CacheQuadrantData& data,
	//	CacheQuadrantData& northData,
	//	CacheQuadrantData& southData,
	//	CacheQuadrantData& westData,
	//	CacheQuadrantData& eastData,
	//	CacheQuadrantData& northwestData,
	//	CacheQuadrantData& northeastData,
	//	CacheQuadrantData& southwestData,
	//	CacheQuadrantData& southeastData)
	//{
	//	return true;
	//}

//#define AMPLITUDE_BLEND_FACTOR	2.0f

	bool MeshCacheBuffer::blendQuadrant(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& data,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		//if (!terrainEdit->northSideZMinus.needBlend && !terrainEdit->southSideZPlus.needBlend && !terrainEdit->eastSideXPlus.needBlend && !terrainEdit->westSideXMinus.needBlend)
		//	return updated;
		
		//	NW	N	NE
		//		_
		//	W	D	E
		//
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(numVerticesPerSize, gridStepInWU, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D	E
		//		-
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(numVerticesPerSize, gridStepInWU, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	|D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnWestSide(numVerticesPerSize, gridStepInWU, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D|	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnEastSide(numVerticesPerSize, gridStepInWU, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		CacheQuadrantData emptyData;

		//	NW	|N	NE
		//		
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnWestSide(numVerticesPerSize, gridStepInWU, lastPhase, northData,
			emptyData /*N*/, data /*S*/, northwestData /*W*/, northeastData /*E*/, emptyData /*NW*/, emptyData /*NE*/, westData /*SW*/, eastData /*SE*/))
			updated = true;
		//	NW	N|	NE
		//		
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnEastSide(numVerticesPerSize, gridStepInWU, lastPhase, northData,
			emptyData /*N*/, data /*S*/, northwestData /*W*/, northeastData /*E*/, emptyData /*NW*/, emptyData /*NE*/, westData /*SW*/, eastData /*SE*/))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D	E
		//		
		//	SW	|S	SE
		if (blendQuadrantOnWestSide(numVerticesPerSize, gridStepInWU, lastPhase, southData,
			data /*N*/, emptyData /*S*/, southwestData /*W*/, southeastData /*E*/, westData /*NW*/, eastData /*NE*/, emptyData /*SW*/, emptyData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//		
		//	SW	S|	SE
		if (blendQuadrantOnEastSide(numVerticesPerSize, gridStepInWU, lastPhase, southData,
			data /*N*/, emptyData /*S*/, southwestData /*W*/, southeastData /*E*/, westData /*NW*/, eastData /*NE*/, emptyData /*SW*/, emptyData /*SE*/))
			updated = true;

		//	NW	N	NE
		//	_	
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(numVerticesPerSize, gridStepInWU, lastPhase, westData,
			northwestData /*N*/, southwestData /*S*/, emptyData /*W*/, data /*E*/, emptyData /*NW*/, northData /*NE*/, emptyData /*SW*/, southData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//	-	
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(numVerticesPerSize, gridStepInWU, lastPhase, westData,
			northwestData /*N*/, southwestData /*S*/, emptyData /*W*/, data /*E*/, emptyData /*NW*/, northData /*NE*/, emptyData /*SW*/, southData /*SE*/))
			updated = true;

		//	NW	N	NE
		//			_
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(numVerticesPerSize, gridStepInWU, lastPhase, eastData,
			northeastData /*N*/, southeastData /*S*/, data /*W*/, emptyData /*E*/, northData /*NW*/, emptyData /*NE*/, southData /*SW*/, emptyData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//			-
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(numVerticesPerSize, gridStepInWU, lastPhase, eastData,
			northeastData /*N*/, southeastData /*S*/, data /*W*/, emptyData /*E*/, northData /*NW*/, emptyData /*NE*/, southData /*SW*/, emptyData /*SE*/))
			updated = true;

		if (lastPhase)
		{
			if (blendBorders(numVerticesPerSize, gridStepInWU, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
				updated = true;
		}

		if (lastPhase or data.heightsUpdated)
		{
			TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, terrainEdit, *data.heights32Buffer);
		}

		if ((lastPhase or northData.heightsUpdated) and northData.terrainEditValues != nullptr and northData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, northTerrainEdit, *northData.heights32Buffer);
		}

		if ((lastPhase or southData.heightsUpdated) and southData.terrainEditValues != nullptr and southData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, southTerrainEdit, *southData.heights32Buffer);
		}

		if ((lastPhase or westData.heightsUpdated) and westData.terrainEditValues != nullptr and westData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, westTerrainEdit, *westData.heights32Buffer);
		}

		if ((lastPhase or eastData.heightsUpdated) and eastData.terrainEditValues != nullptr and eastData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, eastTerrainEdit, *eastData.heights32Buffer);
		}

		if ((lastPhase or northwestData.heightsUpdated) and northwestData.terrainEditValues != nullptr and northwestData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, northwestTerrainEdit, *northwestData.heights32Buffer);
		}

		if ((lastPhase or northeastData.heightsUpdated) and northeastData.terrainEditValues != nullptr and northeastData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, northeastTerrainEdit, *northeastData.heights32Buffer);
		}

		if ((lastPhase or southwestData.heightsUpdated) and southwestData.terrainEditValues != nullptr and southwestData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* southwesthTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, southwesthTerrainEdit, *southwestData.heights32Buffer);
		}

		if ((lastPhase or southeastData.heightsUpdated) and southeastData.terrainEditValues != nullptr and southeastData.terrainEditValues->ptr() != nullptr)
		{
			TerrainEdit* southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();
			calcMinMaxHeight(numVerticesPerSize, southeastTerrainEdit, *southeastData.heights32Buffer);
		}

		return updated;
	}

	bool MeshCacheBuffer::blendBorders(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& currentData,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		if (currentData.heights32Buffer == nullptr || currentData.heights32Buffer->size() == 0
			|| currentData.heights16Buffer == nullptr || currentData.heights16Buffer->size() == 0)
			return updated;

		size_t maxIndex = numVerticesPerSize - 1;

		// North/West corner of current quadrant
		for (size_t idx = 0; idx < numVerticesPerSize; idx++)
		{
			
			//	NW	N	NE
			//		-
			//	W	D	E
			//			
			//	SW	S	SE
			if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northData.heights16Buffer;
				CacheQuadrantData* data1 = &northData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;
				
				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = currentData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = currentData.heights16Buffer;
				CacheQuadrantData* data2 = &currentData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//		-	
			//	SW	S	SE
			if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = currentData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = currentData.heights16Buffer;
				CacheQuadrantData* data1 = &currentData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southData.heights16Buffer;
				CacheQuadrantData* data2 = &southData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D |	E
			//			
			//	SW	S	SE
			if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = currentData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = currentData.heights16Buffer;
				CacheQuadrantData* data1 = &currentData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = eastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = eastData.heights16Buffer;
				CacheQuadrantData* data2 = &eastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W | D	E
			//			
			//	SW  S	SE
			if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = westData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = westData.heights16Buffer;
				CacheQuadrantData* data1 = &westData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = currentData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = currentData.heights16Buffer;
				CacheQuadrantData* data2 = &currentData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW |N	NE
			//		
			//	W	D	E
			//			
			//	SW	S	SE
			if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
				&& northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0 
				&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northwestData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northwestData.heights16Buffer;
				CacheQuadrantData* data1 = &northwestData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = northData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = northData.heights16Buffer;
				CacheQuadrantData* data2 = &northData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N|	NE
			//		
			//	W	D	E
			//			
			//	SW	S	SE
			if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				&& northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
				&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0
				&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northData.heights16Buffer;
				CacheQuadrantData* data1 = &northData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = northeastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = northeastData.heights16Buffer;
				CacheQuadrantData* data2 = &northeastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//	-	
			//	W	D	E
			//			
			//	SW	S	SE
			if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
				&& westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0
				&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northwestData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northwestData.heights16Buffer;
				CacheQuadrantData* data1 = &northwestData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = westData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = westData.heights16Buffer;
				CacheQuadrantData* data2 = &westData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//			-
			//	W	D	E
			//			
			//	SW	S	SE
			if (northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
				&& eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0
				&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northeastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northeastData.heights16Buffer;
				CacheQuadrantData* data1 = &northeastData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = eastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = eastData.heights16Buffer;
				CacheQuadrantData* data2 = &eastData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//	-		
			//	SW	S	SE
			if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				&& southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
				&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0
				&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = westData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = westData.heights16Buffer;
				CacheQuadrantData* data1 = &westData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southwestData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southwestData.heights16Buffer;
				CacheQuadrantData* data2 = &southwestData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			-		
			//	SW	S	SE
			if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				&& southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
				&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0
				&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = eastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = eastData.heights16Buffer;
				CacheQuadrantData* data1 = &eastData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southeastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southeastData.heights16Buffer;
				CacheQuadrantData* data2 = &southeastData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			
			//	SW	|S	SE
			if (southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
				&& southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0
				&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = southwestData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = southwestData.heights16Buffer;
				CacheQuadrantData* data1 = &southwestData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southData.heights16Buffer;
				CacheQuadrantData* data2 = &southData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			
			//	SW	S|	SE
			if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				&& southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
				&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0
				&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = southData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data1heights16Buffer = southData.heights16Buffer;
				CacheQuadrantData* data1 = &southData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southeastData.heights32Buffer;
				TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southeastData.heights16Buffer;
				CacheQuadrantData* data2 = &southeastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->at<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->at<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					TheWorld_Utils::FLOAT_32 f(h);
					uint16_t half = half_from_float(f.u32);
					data1heights32Buffer->at<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->at<float>(border2X, border2Z, numVerticesPerSize) = h;
					data1heights16Buffer->at<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->at<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}
		}

		float northWestCornerHeight = currentData.heights32Buffer->at<float>(0, 0, numVerticesPerSize);
		if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
			&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			northData.heights32Buffer->at<float>(0, maxIndex, numVerticesPerSize) = northWestCornerHeight;
			northData.heights16Buffer->at<uint16_t>(0, maxIndex, numVerticesPerSize) = half;
		}
		if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
			&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			northwestData.heights32Buffer->at<float>(maxIndex, maxIndex, numVerticesPerSize) = northWestCornerHeight;
			northwestData.heights16Buffer->at<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
		}
		if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
			&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			westData.heights32Buffer->at<float>(maxIndex, 0, numVerticesPerSize) = northWestCornerHeight;
			westData.heights16Buffer->at<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
		}

		float northEastCornerHeight = currentData.heights32Buffer->at<float>(maxIndex, 0, numVerticesPerSize);
		if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
			&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			northData.heights32Buffer->at<float>(maxIndex, maxIndex, numVerticesPerSize) = northEastCornerHeight;
			northData.heights16Buffer->at<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
		}
		if (northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
			&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			northeastData.heights32Buffer->at<float>(0, maxIndex, numVerticesPerSize) = northEastCornerHeight;
			northeastData.heights16Buffer->at<uint16_t>(0, maxIndex, numVerticesPerSize) = half;
		}
		if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
			&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			eastData.heights32Buffer->at<float>(0, 0, numVerticesPerSize) = northEastCornerHeight;
			eastData.heights16Buffer->at<uint16_t>(0, 0, numVerticesPerSize) = half;
		}

		float southWestCornerHeight = currentData.heights32Buffer->at<float>(0, maxIndex, numVerticesPerSize);
		if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
			&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			southData.heights32Buffer->at<float>(0, 0, numVerticesPerSize) = southWestCornerHeight;
			southData.heights16Buffer->at<uint16_t>(0, 0, numVerticesPerSize) = half;
		}
		if (southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
			&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			southwestData.heights32Buffer->at<float>(maxIndex, 0, numVerticesPerSize) = southWestCornerHeight;
			southwestData.heights16Buffer->at<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
		}
		if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
			&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
			uint16_t half = half_from_float(f.u32);
			westData.heights32Buffer->at<float>(maxIndex, maxIndex, numVerticesPerSize) = southWestCornerHeight;
			westData.heights16Buffer->at<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
		}

		float southEastCornerHeight = currentData.heights32Buffer->at<float>(maxIndex, maxIndex, numVerticesPerSize);
		if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
			&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			southData.heights32Buffer->at<float>(maxIndex, 0, numVerticesPerSize) = southEastCornerHeight;
			southData.heights16Buffer->at<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
		}
		if (southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
			&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			southeastData.heights32Buffer->at<float>(0, 0, numVerticesPerSize) = southEastCornerHeight;
			southeastData.heights16Buffer->at<uint16_t>(0, 0, numVerticesPerSize) = half;
		}
		if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
			&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0)
		{
			TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
			uint16_t half = half_from_float(f.u32);
			eastData.heights32Buffer->at<float>(0, maxIndex, numVerticesPerSize) = southEastCornerHeight;
			eastData.heights16Buffer->at<uint16_t>(0, maxIndex, numVerticesPerSize) = half;
		}

		return updated;
	}
		
	void MeshCacheBuffer::calcMinMaxHeight(size_t numVerticesPerSize, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& heights32Buffer)
	{
		terrainEdit->minHeight = FLT_MAX;
		terrainEdit->maxHeight = -FLT_MAX;
		terrainEdit->eastSideXPlus.minHeight = FLT_MAX;
		terrainEdit->eastSideXPlus.maxHeight = -FLT_MAX;
		terrainEdit->westSideXMinus.minHeight = FLT_MAX;
		terrainEdit->westSideXMinus.maxHeight = -FLT_MAX;
		terrainEdit->southSideZPlus.minHeight = FLT_MAX;
		terrainEdit->southSideZPlus.maxHeight = -FLT_MAX;
		terrainEdit->northSideZMinus.minHeight = FLT_MAX;
		terrainEdit->northSideZMinus.maxHeight = -FLT_MAX;

		size_t maxIndex = numVerticesPerSize - 1;

		for (size_t z = 0; z < numVerticesPerSize; z++)
		{
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float h = heights32Buffer.at<float>(x, z, numVerticesPerSize);
				if (h < terrainEdit->minHeight)
					terrainEdit->minHeight = h;
				if (h > terrainEdit->maxHeight)
					terrainEdit->maxHeight = h;

				if (x == 0)
				{
					if (h < terrainEdit->westSideXMinus.minHeight)
						terrainEdit->westSideXMinus.minHeight = h;
					if (h > terrainEdit->westSideXMinus.maxHeight)
						terrainEdit->westSideXMinus.maxHeight = h;
				}

				if (z == 0)
				{
					if (h < terrainEdit->northSideZMinus.minHeight)
						terrainEdit->northSideZMinus.minHeight = h;
					if (h > terrainEdit->northSideZMinus.maxHeight)
						terrainEdit->northSideZMinus.maxHeight = h;
				}

				if (x == maxIndex)
				{
					if (h < terrainEdit->eastSideXPlus.minHeight)
						terrainEdit->eastSideXPlus.minHeight = h;
					if (h > terrainEdit->eastSideXPlus.maxHeight)
						terrainEdit->eastSideXPlus.maxHeight = h;
				}

				if (z == maxIndex)
				{
					if (h < terrainEdit->southSideZPlus.minHeight)
						terrainEdit->southSideZPlus.minHeight = h;
					if (h > terrainEdit->southSideZPlus.maxHeight)
						terrainEdit->southSideZPlus.maxHeight = h;
				}
			}
		}
	}

	bool MeshCacheBuffer::blendQuadrantOnNorthSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& data,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		if (data.terrainEditValues == nullptr || data.terrainEditValues->size() == 0
			|| data.heights32Buffer == nullptr || data.heights32Buffer->size() == 0
			|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0)
			return updated;

		TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
		TerrainEdit* northTerrainEdit = nullptr;
		if (northData.terrainEditValues != nullptr)
			northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
		//TerrainEdit* southTerrainEdit = nullptr;
		//if (southData.terrainEditValues != nullptr)
		//	southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
		TerrainEdit* westTerrainEdit = nullptr;
		if (westData.terrainEditValues != nullptr)
			westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
		TerrainEdit* eastTerrainEdit = nullptr;
		if (eastData.terrainEditValues != nullptr)
			eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
		TerrainEdit* northwestTerrainEdit = nullptr;
		if (northwestData.terrainEditValues != nullptr)
			northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
		TerrainEdit* northeastTerrainEdit = nullptr;
		if (northeastData.terrainEditValues != nullptr)
			northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
		//TerrainEdit* southwestTerrainEdit = nullptr;
		//if (southwestData.terrainEditValues != nullptr)
		//	southwestTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
		//TerrainEdit* southeastTerrainEdit = nullptr;
		//if (southeastData.terrainEditValues != nullptr)
		//	southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();

		size_t maxIndex = numVerticesPerSize - 1;

		if (northData.heights32Buffer != nullptr && northTerrainEdit != nullptr /* && (terrainEdit->northSideZMinus.needBlend || northTerrainEdit->southSideZPlus.needBlend)*/)
		{
			// moving on north side: every x with z = 0 
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->at<float>(x, 0, numVerticesPerSize) - northData.heights32Buffer->at<float>(x, maxIndex, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->at<float>(x, numVerticesToAdjustInEachQuadrant - 1, numVerticesPerSize)
								- northData.heights32Buffer->at<float>(x, maxIndex - (numVerticesToAdjustInEachQuadrant - 1), numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					northData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int z = (int)numVerticesToAdjustInEachQuadrant - 1; z >= 0; z--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->at<float>(x, z, numVerticesPerSize) - northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * z;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->at<float>(x, z, numVerticesPerSize) > northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (z == 0 && heightSaved)
						{
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) = saveHeight;
						}
						else
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) += increment;
						TheWorld_Utils::FLOAT_32 f(data.heights32Buffer->at<float>(x, z, numVerticesPerSize));
						uint16_t half = half_from_float(f.u32);
						data.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half;

						if (z == 0 && heightSaved)
						{
							northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) = saveHeight;
						}
						else
							northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) -= increment;
						TheWorld_Utils::FLOAT_32 f1(northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize));
						uint16_t half1 = half_from_float(f1.u32);
						northData.heights16Buffer->at<uint16_t>(x, maxIndex - z, numVerticesPerSize) = half1;

						if (z == 1)
						{
							float h1 = data.heights32Buffer->at<float>(x, z, numVerticesPerSize);
							float h2 = northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize);
							saveHeight = (h1 + h2) / 2;
							heightSaved = true;
						}

						//if (x == 0)
						//	PLOG_DEBUG << "x=" << std::to_string(x) << " z=" << std::to_string(z) << "/" << std::to_string(maxIndex - z) << " increment= " << std::to_string(increment) << " " << std::to_string(h) << " ==> " << std::to_string(data.heights32Buffer->at<float>(x, z, numVerticesPerSize)) << " " << std::to_string(hNorth) << " ==> " << std::to_string(northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize));
					}

					// DISABLED ATM BECAUSE terrainEdit->extraValues.blendingBendingFactor IS ALWAYS ZERO
					if (terrainEdit->extraValues.blendingBendingFactor != 0 && lastPhase)
					{
						std::hash<std::string> hasher;
						size_t seed = hasher(data.meshId + northData.meshId);
						srand((unsigned int)(seed));
						int random_i = (rand() % 100) + 1;			// random integer between 1 and 100
						float random_f = (((terrainEdit->extraValues.maxRandomBendingExtension - terrainEdit->extraValues.minRandomBendingExtension) / 100) * random_i) + terrainEdit->extraValues.minRandomBendingExtension;
						numVerticesToAdjustInEachQuadrant = size_t(ceilf(numVerticesToAdjustInEachQuadrant * random_f));
						float radiantStepPerVertex = TheWorld_Utils::kPi / (numVerticesToAdjustInEachQuadrant);

						float gapToFlatten1 = data.heights32Buffer->at<float>(x, numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - data.heights32Buffer->at<float>(x, 0, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = northData.heights32Buffer->at<float>(x, maxIndex - numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - northData.heights32Buffer->at<float>(x, maxIndex, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int z = (int)numVerticesToAdjustInEachQuadrant; z >= 0; z--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= modifiedHeightStep1;
							TheWorld_Utils::FLOAT_32 f1(data.heights32Buffer->at<float>(x, z, numVerticesPerSize));
							uint16_t half1 = half_from_float(f1.u32);
							data.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half1;

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) -= modifiedHeightStep2;
							TheWorld_Utils::FLOAT_32 f2(northData.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize));
							uint16_t half2 = half_from_float(f2.u32);
							northData.heights16Buffer->at<uint16_t>(x, maxIndex - z, numVerticesPerSize) = half2;

							idx++;
						}
					}
				}
			}

			terrainEdit->northSideZMinus.needBlend = false;
			northTerrainEdit->southSideZPlus.needBlend = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnSouthSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& data,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		if (data.terrainEditValues == nullptr || data.terrainEditValues->size() == 0
			|| data.heights32Buffer == nullptr || data.heights32Buffer->size() == 0
			|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0)
			return updated;

		TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
		//TerrainEdit* northTerrainEdit = nullptr;
		//if (northData.terrainEditValues != nullptr)
		//	northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
		TerrainEdit* southTerrainEdit = nullptr;
		if (southData.terrainEditValues != nullptr)
			southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
		TerrainEdit* westTerrainEdit = nullptr;
		if (westData.terrainEditValues != nullptr)
			westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
		TerrainEdit* eastTerrainEdit = nullptr;
		if (eastData.terrainEditValues != nullptr)
			eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
		//TerrainEdit* northwestTerrainEdit = nullptr;
		//if (northwestData.terrainEditValues != nullptr)
		//	northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
		//TerrainEdit* northeastTerrainEdit = nullptr;
		//if (northeastData.terrainEditValues != nullptr)
		//	northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
		TerrainEdit* southwestTerrainEdit = nullptr;
		if (southwestData.terrainEditValues != nullptr)
			southwestTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
		TerrainEdit* southeastTerrainEdit = nullptr;
		if (southeastData.terrainEditValues != nullptr)
			southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();

		size_t maxIndex = numVerticesPerSize - 1;

		if (southData.heights32Buffer != nullptr && southTerrainEdit != nullptr /* && (terrainEdit->southSideZPlus.needBlend || southTerrainEdit->northSideZMinus.needBlend)*/)
		{
			// moving on south side: every x with z = 0 
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->at<float>(x, maxIndex, numVerticesPerSize) - southData.heights32Buffer->at<float>(x, 0, numVerticesPerSize)) / 2;

				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->at<float>(x, maxIndex - (numVerticesToAdjustInEachQuadrant - 1), numVerticesPerSize)
						- southData.heights32Buffer->at<float>(x, numVerticesToAdjustInEachQuadrant - 1, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					southData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int z = (int)numVerticesToAdjustInEachQuadrant - 1; z >= 0; z--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) - southData.heights32Buffer->at<float>(x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * z;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) > southData.heights32Buffer->at<float>(x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (z == 0 && heightSaved)
						{
							data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) = saveHeight;
						}
						else
							data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) += increment;
						TheWorld_Utils::FLOAT_32 f(data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize));
						uint16_t half = half_from_float(f.u32);
						data.heights16Buffer->at<uint16_t>(x, maxIndex - z, numVerticesPerSize) = half;

						if (z == 0 && heightSaved)
						{
							southData.heights32Buffer->at<float>(x, z, numVerticesPerSize) = saveHeight;
						}
						else
							southData.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= increment;
						TheWorld_Utils::FLOAT_32 f1(southData.heights32Buffer->at<float>(x, z, numVerticesPerSize));
						uint16_t half1 = half_from_float(f1.u32);
						southData.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half1;

						if (z == 1)
						{
							float h1 = data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize);
							float h2 = southData.heights32Buffer->at<float>(x, z, numVerticesPerSize);
							saveHeight = (h1 + h2) / 2;
							heightSaved = true;
						}
					}

					// DISABLED ATM BECAUSE terrainEdit->extraValues.blendingBendingFactor IS ALWAYS ZERO
					if (terrainEdit->extraValues.blendingBendingFactor != 0 && lastPhase)
					{
						std::hash<std::string> hasher;
						size_t seed = hasher(data.meshId + southData.meshId);
						srand((unsigned int)(seed));
						int random_i = (rand() % 100) + 1;			// random integer between 1 and 100
						float random_f = (((terrainEdit->extraValues.maxRandomBendingExtension - terrainEdit->extraValues.minRandomBendingExtension) / 100) * random_i) + terrainEdit->extraValues.minRandomBendingExtension;
						numVerticesToAdjustInEachQuadrant = size_t(ceilf(numVerticesToAdjustInEachQuadrant * random_f));
						float radiantStepPerVertex = TheWorld_Utils::kPi / (numVerticesToAdjustInEachQuadrant);

						float gapToFlatten1 = data.heights32Buffer->at<float>(x, maxIndex - numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - data.heights32Buffer->at<float>(x, maxIndex, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = southData.heights32Buffer->at<float>(x, numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - southData.heights32Buffer->at<float>(x, 0, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int z = (int)numVerticesToAdjustInEachQuadrant; z >= 0; z--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize) -= modifiedHeightStep1;
							TheWorld_Utils::FLOAT_32 f1(data.heights32Buffer->at<float>(x, maxIndex - z, numVerticesPerSize));
							uint16_t half1 = half_from_float(f1.u32);
							data.heights16Buffer->at<uint16_t>(x, maxIndex - z, numVerticesPerSize) = half1;

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							southData.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= modifiedHeightStep2;
							TheWorld_Utils::FLOAT_32 f2(southData.heights32Buffer->at<float>(x, z, numVerticesPerSize));
							uint16_t half2 = half_from_float(f2.u32);
							southData.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half2;

							idx++;
						}
					}
				}
			}

			terrainEdit->southSideZPlus.needBlend = false;
			southTerrainEdit->northSideZMinus.needBlend = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnWestSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& data,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		if (data.terrainEditValues == nullptr || data.terrainEditValues->size() == 0
			|| data.heights32Buffer == nullptr || data.heights32Buffer->size() == 0
			|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0)
			return updated;

		TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
		TerrainEdit* northTerrainEdit = nullptr;
		if (northData.terrainEditValues != nullptr)
			northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
		TerrainEdit* southTerrainEdit = nullptr;
		if (southData.terrainEditValues != nullptr)
			southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
		TerrainEdit* westTerrainEdit = nullptr;
		if (westData.terrainEditValues != nullptr)
			westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
		//TerrainEdit* eastTerrainEdit = nullptr;
		//if (eastData.terrainEditValues != nullptr)
		//	eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
		TerrainEdit* northwestTerrainEdit = nullptr;
		if (northwestData.terrainEditValues != nullptr)
			northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
		//TerrainEdit* northeastTerrainEdit = nullptr;
		//if (northeastData.terrainEditValues != nullptr)
		//	northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
		TerrainEdit* southwestTerrainEdit = nullptr;
		if (southwestData.terrainEditValues != nullptr)
			southwestTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
		//TerrainEdit* southeastTerrainEdit = nullptr;
		//if (southeastData.terrainEditValues != nullptr)
		//	southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();

		size_t maxIndex = numVerticesPerSize - 1;

		if (westData.heights32Buffer != nullptr && westTerrainEdit != nullptr /*&& (terrainEdit->westSideXMinus.needBlend || westTerrainEdit->eastSideXPlus.needBlend)*/)
		{
			// moving on west side: every z with x = 0 
			for (size_t z = 0; z < numVerticesPerSize; z++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->at<float>(0, z, numVerticesPerSize) - westData.heights32Buffer->at<float>(maxIndex, z, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->at<float>(numVerticesToAdjustInEachQuadrant - 1, z, numVerticesPerSize)
						- westData.heights32Buffer->at<float>(maxIndex - (numVerticesToAdjustInEachQuadrant - 1), z, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					westData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int x = (int)numVerticesToAdjustInEachQuadrant - 1; x >= 0; x--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->at<float>(x, z, numVerticesPerSize) - westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * x;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->at<float>(x, z, numVerticesPerSize) > westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (x == 0 && heightSaved)
						{
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) = saveHeight;
						}
						else
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) += increment;
						TheWorld_Utils::FLOAT_32 f(data.heights32Buffer->at<float>(x, z, numVerticesPerSize));
						uint16_t half = half_from_float(f.u32);
						data.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half;

						if (x == 0 && heightSaved)
						{
							westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) = saveHeight;
						}
						else
							westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) -= increment;
						TheWorld_Utils::FLOAT_32 f1(westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize));
						uint16_t half1 = half_from_float(f1.u32);
						westData.heights16Buffer->at<uint16_t>(maxIndex - x, z, numVerticesPerSize) = half1;

						if (x == 1)
						{
							float h1 = data.heights32Buffer->at<float>(x, z, numVerticesPerSize);
							float h2 = westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize);
							saveHeight = (h1 + h2) / 2;
							heightSaved = true;
						}
					}
					
					// DISABLED ATM BECAUSE terrainEdit->extraValues.blendingBendingFactor IS ALWAYS ZERO
					if (terrainEdit->extraValues.blendingBendingFactor != 0 && lastPhase)
					{
						std::hash<std::string> hasher;
						size_t seed = hasher(data.meshId + westData.meshId);
						srand((unsigned int)(seed));
						int random_i = (rand() % 100) + 1;			// random integer between 1 and 100
						float random_f = (((terrainEdit->extraValues.maxRandomBendingExtension - terrainEdit->extraValues.minRandomBendingExtension) / 100) * random_i) + terrainEdit->extraValues.minRandomBendingExtension;
						numVerticesToAdjustInEachQuadrant = size_t(ceilf(numVerticesToAdjustInEachQuadrant * random_f));
						float radiantStepPerVertex = TheWorld_Utils::kPi / (numVerticesToAdjustInEachQuadrant);

						float gapToFlatten1 = data.heights32Buffer->at<float>(numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - data.heights32Buffer->at<float>(0, z, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = westData.heights32Buffer->at<float>(maxIndex - numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - westData.heights32Buffer->at<float>(maxIndex, z, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int x = (int)numVerticesToAdjustInEachQuadrant; x >= 0; x--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= modifiedHeightStep1;
							TheWorld_Utils::FLOAT_32 f1(data.heights32Buffer->at<float>(x, z, numVerticesPerSize));
							uint16_t half1 = half_from_float(f1.u32);
							data.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half1;

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) -= modifiedHeightStep2;
							TheWorld_Utils::FLOAT_32 f2(westData.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize));
							uint16_t half2 = half_from_float(f2.u32);
							westData.heights16Buffer->at<uint16_t>(maxIndex - x, z, numVerticesPerSize) = half2;

							idx++;
						}
					}
				}
			}

			terrainEdit->westSideXMinus.needBlend = false;
			westTerrainEdit->eastSideXPlus.needBlend = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnEastSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		CacheQuadrantData& data,
		CacheQuadrantData& northData,
		CacheQuadrantData& southData,
		CacheQuadrantData& westData,
		CacheQuadrantData& eastData,
		CacheQuadrantData& northwestData,
		CacheQuadrantData& northeastData,
		CacheQuadrantData& southwestData,
		CacheQuadrantData& southeastData)
	{
		bool updated = false;

		if (data.terrainEditValues == nullptr || data.terrainEditValues->size() == 0
			|| data.heights32Buffer == nullptr || data.heights32Buffer->size() == 0
			|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0)
			return updated;

		TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
		TerrainEdit* northTerrainEdit = nullptr;
		if (northData.terrainEditValues != nullptr)
			northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
		TerrainEdit* southTerrainEdit = nullptr;
		if (southData.terrainEditValues != nullptr)
			southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
		//TerrainEdit* westTerrainEdit = nullptr;
		//if (westData.terrainEditValues != nullptr)
		//	westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
		TerrainEdit* eastTerrainEdit = nullptr;
		if (eastData.terrainEditValues != nullptr)
			eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
		//TerrainEdit* northwestTerrainEdit = nullptr;
		//if (northwestData.terrainEditValues != nullptr)
		//	northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
		TerrainEdit* northeastTerrainEdit = nullptr;
		if (northeastData.terrainEditValues != nullptr)
			northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
		//TerrainEdit* southwestTerrainEdit = nullptr;
		//if (southwestData.terrainEditValues != nullptr)
		//	southwestTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
		TerrainEdit* southeastTerrainEdit = nullptr;
		if (southeastData.terrainEditValues != nullptr)
			southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();

		size_t maxIndex = numVerticesPerSize - 1;

		if (eastData.heights32Buffer != nullptr && eastTerrainEdit != nullptr /*&& (terrainEdit->eastSideXPlus.needBlend || eastTerrainEdit->westSideXMinus.needBlend)*/)
		{
			// moving on east side: every z with x = 0 
			for (size_t z = 0; z < numVerticesPerSize; z++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->at<float>(maxIndex, z, numVerticesPerSize) - eastData.heights32Buffer->at<float>(0, z, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->at<float>(maxIndex - (numVerticesToAdjustInEachQuadrant - 1), z, numVerticesPerSize)
						- eastData.heights32Buffer->at<float>(numVerticesToAdjustInEachQuadrant - 1, z, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					eastData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int x = (int)numVerticesToAdjustInEachQuadrant - 1; x >= 0; x--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) - eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * x;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) > eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (x == 0 && heightSaved)
						{
							data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) = saveHeight;
						}
						else
							data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) += increment;
						TheWorld_Utils::FLOAT_32 f(data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize));
						uint16_t half = half_from_float(f.u32);
						data.heights16Buffer->at<uint16_t>(maxIndex - x, z, numVerticesPerSize) = half;

						if (x == 0 && heightSaved)
						{
							eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize) = saveHeight;
						}
						else
							eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= increment;
						TheWorld_Utils::FLOAT_32 f1(eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize));
						uint16_t half1 = half_from_float(f1.u32);
						eastData.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half1;

						if (x == 1)
						{
							float h1 = data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize);
							float h2 = eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize);
							saveHeight = (h1 + h2) / 2;
							heightSaved = true;
						}
					}

					// DISABLED ATM BECAUSE terrainEdit->extraValues.blendingBendingFactor IS ALWAYS ZERO
					if (terrainEdit->extraValues.blendingBendingFactor != 0 && lastPhase)
					{
						std::hash<std::string> hasher;
						size_t seed = hasher(data.meshId + eastData.meshId);
						srand((unsigned int)(seed));
						int random_i = (rand() % 100) + 1;			// random integer between 1 and 100
						float random_f = (((terrainEdit->extraValues.maxRandomBendingExtension - terrainEdit->extraValues.minRandomBendingExtension) / 100) * random_i) + terrainEdit->extraValues.minRandomBendingExtension;
						numVerticesToAdjustInEachQuadrant = size_t(ceilf(numVerticesToAdjustInEachQuadrant * random_f));
						float radiantStepPerVertex = TheWorld_Utils::kPi / (numVerticesToAdjustInEachQuadrant);

						float gapToFlatten1 = data.heights32Buffer->at<float>(maxIndex - numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - data.heights32Buffer->at<float>(maxIndex, z, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = eastData.heights32Buffer->at<float>(numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - eastData.heights32Buffer->at<float>(0, z, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int x = (int)numVerticesToAdjustInEachQuadrant; x >= 0; x--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize) -= modifiedHeightStep1;
							TheWorld_Utils::FLOAT_32 f1(data.heights32Buffer->at<float>(maxIndex - x, z, numVerticesPerSize));
							uint16_t half1 = half_from_float(f1.u32);
							data.heights16Buffer->at<uint16_t>(maxIndex - x, z, numVerticesPerSize) = half1;

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize) -= modifiedHeightStep2;
							TheWorld_Utils::FLOAT_32 f2(eastData.heights32Buffer->at<float>(x, z, numVerticesPerSize));
							uint16_t half2 = half_from_float(f2.u32);
							eastData.heights16Buffer->at<uint16_t>(x, z, numVerticesPerSize) = half2;

							idx++;
						}
					}
				}
			}

			terrainEdit->eastSideXPlus.needBlend = false;
			eastTerrainEdit->westSideXMinus.needBlend = false;
		}

		return updated;
	}

	void ThreadPool::Start(std::string label, size_t num_threads, /*const std::function<void()>* threadInitFunction, const std::function<void()>* threadDeinitFunction,*/ ThreadInitDeinit* threadInitDeinit)
	{
		m_label = label;
		m_threadInitDeinit = threadInitDeinit;
		m_lastDiagnosticTime = std::chrono::time_point_cast<TheWorld_Utils::MsTimePoint::duration>(std::chrono::system_clock::now());
		m_lastAllWorkingStatus = false;
		//m_threadInitFunction = threadInitFunction;
		//m_threadDeinitFunction = threadDeinitFunction;
		uint32_t _num_threads = (uint32_t)num_threads;
		if (_num_threads <= 0)
			_num_threads  = std::thread::hardware_concurrency(); // Max # of threads the system supports
		//const uint32_t num_threads = 2;
		m_threads.resize(_num_threads);
		for (uint32_t i = 0; i < _num_threads; i++)
		{
			m_threads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
		}
	}

	size_t ThreadPool::getNumWorkingThreads(size_t& maxThreads)
	{
		maxThreads = m_threads.size();
		return m_numWorkingThreads;
	}
		
	bool ThreadPool::allThreadsWorking(void)
	{
		return m_numWorkingThreads >= m_threads.size();
	}

	void ThreadPool::ThreadLoop()
	{
		if (m_threadInitDeinit != nullptr)
			m_threadInitDeinit->threadInit();
		//if (m_threadInitFunction != nullptr)
		//{
		//	(*m_threadInitFunction)();
		//}

		DWORD tid = GetCurrentThreadId();
		std::thread::id this_id = std::this_thread::get_id();
		size_t _tid = std::hash<std::thread::id>{}(std::this_thread::get_id());

		while (true)
		{
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(m_queue_mutex);
				m_mutex_condition.wait(lock, [this] { return !m_jobs.empty() || m_should_terminate; });
				if (m_should_terminate)
				{
					break;
				}
				job = m_jobs.front();
				m_jobs.pop();

				TheWorld_Utils::MsTimePoint now = std::chrono::time_point_cast<TheWorld_Utils::MsTimePoint::duration>(std::chrono::system_clock::now());
				size_t numThreads = m_threads.size();
				bool allThreadsWorking = m_numWorkingThreads >= numThreads;
				if ((now - m_lastDiagnosticTime).count() >= 1000)
				{
					if (allThreadsWorking)
						PLOG_INFO << "ThreadPool " << m_label << " has all threads working (" << std::to_string(m_numWorkingThreads) << ":" << std::to_string(numThreads) << ") - Queue size: " << m_jobs.size();
					//else
					//	if (allThreadsWorking != m_lastAllWorkingStatus)
					//		PLOG_INFO << "ThreadPool " << m_label << " has free threads (" << std::to_string(m_numWorkingThreads + 1) << ":" << std::to_string(numThreads) << ")";

					m_lastDiagnosticTime = now;
					m_lastAllWorkingStatus = allThreadsWorking;
				}
			}
			
			{
				std::unique_lock<std::mutex> lock(m_threadmap_mutex);
				m_numWorkingThreads++;
				m_workingThreads[tid] = m_numWorkingThreads;
			}

			job();
			
			{
				std::unique_lock<std::mutex> lock(m_threadmap_mutex);
				if (m_workingThreads.contains(tid))
					m_workingThreads.erase(tid);
				m_numWorkingThreads--;
			}
		}

		if (m_threadInitDeinit != nullptr)
			m_threadInitDeinit->threadDeinit();
		//if (m_threadDeinitFunction != nullptr)
		//{
		//	(*m_threadDeinitFunction)();
		//}
	}
	
	//void ThreadPool::ThreadLoop()
	//{
	//	if (m_threadInitDeinit != nullptr)
	//		m_threadInitDeinit->threadInit();
	//	//if (m_threadInitFunction != nullptr)
	//	//{
	//	//	(*m_threadInitFunction)();
	//	//}

	//	DWORD tid = GetCurrentThreadId();
	//	std::thread::id this_id = std::this_thread::get_id();
	//	size_t _tid = std::hash<std::thread::id>{}(std::this_thread::get_id());

	//	while (true)
	//	{
	//		std::function<void()> job;
	//		{
	//			while (true)
	//			{
	//				{
	//					std::unique_lock<std::mutex> lock(m_queue_mutex);
	//					if (m_should_terminate)
	//						break;
	//					if (!m_jobs.empty())
	//					{
	//						job = m_jobs.front();
	//						m_jobs.pop();

	//						TheWorld_Utils::MsTimePoint now = std::chrono::time_point_cast<TheWorld_Utils::MsTimePoint::duration>(std::chrono::system_clock::now());
	//						size_t numThreads = m_threads.size();
	//						bool allThreadsWorking = m_numWorkingThreads >= numThreads;
	//						if ((now - m_lastDiagnosticTime).count() >= 1000)
	//						{
	//							if (allThreadsWorking)
	//								PLOG_INFO << "ThreadPool " << m_label << " has all threads working (" << std::to_string(m_numWorkingThreads) << ":" << std::to_string(numThreads) << ") - Queue size: " << m_jobs.size();
	//							//else
	//							//	if (allThreadsWorking != m_lastAllWorkingStatus)
	//							//		PLOG_INFO << "ThreadPool " << m_label << " has free threads (" << std::to_string(m_numWorkingThreads + 1) << ":" << std::to_string(numThreads) << ")";

	//							m_lastDiagnosticTime = now;
	//							m_lastAllWorkingStatus = allThreadsWorking;
	//						}
	//						break;
	//					}
	//				}
	//				Sleep(1);
	//			}
	//			if (m_should_terminate)
	//			{
	//				break;
	//			}
	//		}

	//		{
	//			std::unique_lock<std::mutex> lock(m_threadmap_mutex);
	//			m_numWorkingThreads++;
	//			m_workingThreads[tid] = m_numWorkingThreads;
	//		}

	//		job();

	//		{
	//			std::unique_lock<std::mutex> lock(m_threadmap_mutex);
	//			if (m_workingThreads.contains(tid))
	//				m_workingThreads.erase(tid);
	//			m_numWorkingThreads--;
	//		}
	//	}

	//	if (m_threadInitDeinit != nullptr)
	//		m_threadInitDeinit->threadDeinit();
	//	//if (m_threadDeinitFunction != nullptr)
	//	//{
	//	//	(*m_threadDeinitFunction)();
	//	//}
	//}

	void ThreadPool::QueueJob(const std::function<void()>& job)
	{
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			m_jobs.push(job);
		}
		m_mutex_condition.notify_one();
	}
	
	bool ThreadPool::busy()
	{
		bool poolbusy;
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			poolbusy = !m_jobs.empty();
		}
		return poolbusy;
	}

	void ThreadPool::Stop()
	{
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			m_should_terminate = true;
		}
		m_mutex_condition.notify_all();
		for (std::thread& active_thread : m_threads) 
		{
			active_thread.join();
		}
		m_threads.clear();
	}
	
	std::string Utils::ReplaceString(std::string subject, const std::string& search, const std::string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos)
		{
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	void Utils::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos)
		{
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}

	bool Utils::StringContains(std::string s, std::string search)
	{
		if (s.find(search) != std::string::npos)
			return true;
		else
			return false;
	}

	std::string ToString(GUID* guid)
	{
		char guid_string[37]; // 32 hex chars + 4 hyphens + null terminator
		snprintf(
			guid_string, sizeof(guid_string),
			"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			guid->Data1, guid->Data2, guid->Data3,
			guid->Data4[0], guid->Data4[1], guid->Data4[2],
			guid->Data4[3], guid->Data4[4], guid->Data4[5],
			guid->Data4[6], guid->Data4[7]);
		return guid_string;
	}

	// normals coord range from 0 to 1
	Eigen::Vector3d packNormal(Eigen::Vector3d normal)
	{
		Eigen::Vector3d temp = 0.5 * (normal + Eigen::Vector3d(1, 1, 1));
		return Eigen::Vector3d(temp.x(), temp.z(), temp.y());
	}

	// normals coord range from 0 to 1
	Eigen::Vector3d unpackNormal(Eigen::Vector3d packedNormal)
	{
		Eigen::Vector3d temp(packedNormal.x(), packedNormal.z(), packedNormal.y());
		return (2 * temp) - Eigen::Vector3d(1, 1, 1);
		//Eigen::Vector3d temp = (2 * packedNormal) - Eigen::Vector3d(1, 1, 1);
		//return Eigen::Vector3d(temp.x(), temp.z(), temp.y());
	}
	void packNormal(float normalX, float normalY, float normalZ, float& packedNormalX, float& packedNormalY, float& packedNormalZ)
	{
		Eigen::Vector3d packedNormal = TheWorld_Utils::packNormal(Eigen::Vector3d(normalX, normalY, normalZ));
		packedNormalX = float(packedNormal.x());
		packedNormalY = float(packedNormal.y());
		packedNormalZ = float(packedNormal.z());
	}

	void unpackNormal(float packedNormalX, float packedNormalY, float packedNormalZ, float& normalX, float& normalY, float& normalZ)
	{
		Eigen::Vector3d normal = TheWorld_Utils::unpackNormal(Eigen::Vector3d(packedNormalX, packedNormalY, packedNormalZ));
		normalX = float(normal.x());
		normalY = float(normal.y());
		normalZ = float(normal.z());
	}
}
