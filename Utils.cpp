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
#include <type_traits>

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
	bool MeshCacheBuffer::s_newCacheQuadrantFormat1 = true;
	bool MeshCacheBuffer::newCacheQuadrantFormat0(void)
	{
		return !s_newCacheQuadrantFormat1;
	}
	bool MeshCacheBuffer::newCacheQuadrantFormat1(void)
	{
		return s_newCacheQuadrantFormat1;
	}

	enum class MeshCacheBuffer::HeightsFormatInQuadrantBuffer MeshCacheBuffer::s_heightsFormatInNewSavedQuadrant = MeshCacheBuffer::HeightsFormatInQuadrantBuffer::UInt16;
	void MeshCacheBuffer::setHeightsFormatInNewSavedQuadrant(enum class HeightsFormatInQuadrantBuffer heightsFormat)
	{
		s_heightsFormatInNewSavedQuadrant = heightsFormat;
	}
	bool MeshCacheBuffer::heightsFloat16InNewSavedQuadrant(void)
	{
		return s_heightsFormatInNewSavedQuadrant == HeightsFormatInQuadrantBuffer::Float16;
	}
	bool MeshCacheBuffer::heightsUInt16InNewSavedQuadrant(void)
	{
		return s_heightsFormatInNewSavedQuadrant == HeightsFormatInQuadrantBuffer::UInt16;
	}
	bool MeshCacheBuffer::heightsFloat32InNewSavedQuadrant(void)
	{
		return s_heightsFormatInNewSavedQuadrant == HeightsFormatInQuadrantBuffer::Float32;
	}

	float MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(float height)
	{
		bool changed = false;
		return adaptHeightToNewSavedQuadrantFormat(height, changed);
	}

	float MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(float height, bool& changed)
	{
		changed = false;
		float newHeight = 0;

		if (MeshCacheBuffer::heightsFloat16InNewSavedQuadrant())
		{
			newHeight = MeshCacheBuffer::halfToFloat(MeshCacheBuffer::halfFromFloat(height));
			if (newHeight != height)
				changed = true;
		}
		else if (MeshCacheBuffer::heightsUInt16InNewSavedQuadrant())
		{
			newHeight = MeshCacheBuffer::uint16HeightToFloat(MeshCacheBuffer::uint16HeightFromFloat(height));
			if (newHeight != height)
				changed = true;
		}
		else if (MeshCacheBuffer::heightsFloat32InNewSavedQuadrant())
		{
			// No conversion needed for Float32
			newHeight = height;	
		}
		else
		{
			throw std::runtime_error("Unknown heights format in new saved quadrant.");
		}
		return newHeight;
	}

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

	std::chrono::system_clock::time_point null_time_point(void)
	{
		return std::chrono::system_clock::time_point();
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

	bool null_time_point(std::chrono::system_clock::time_point tp)
	{
		return is_time_point_null(tp);
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

	std::time_t SYSTEMTIME_utc_to_time_t_utc(const SYSTEMTIME& st)
	{
		std::tm t = {};
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_hour = st.wHour;
		t.tm_min = st.wMinute;
		t.tm_sec = st.wSecond;
		std::time_t time_t_utc = _mkgmtime(&t);	// correct: correct to time_t ignoring local calendar (it assumes input time is in UTC format and mantains it)
		return time_t_utc;
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

	constexpr auto QUADRANT_TRACKING_SINGLE = true;
	//constexpr auto QUADRANT_TRACKING_IDX_POS_X = -1;
	//constexpr auto QUADRANT_TRACKING_IDX_POS_Z = 0;
	//constexpr auto QUADRANT_TRACKING_IDX_POS_X = -55;
	//constexpr auto QUADRANT_TRACKING_IDX_POS_Z = -58;
	constexpr auto QUADRANT_TRACKING_IDX_POS_X = -17;
	constexpr auto QUADRANT_TRACKING_IDX_POS_Z = -36;

	constexpr auto QUADRANT_TRACKING_MULTIPLE = false;
	static std::vector<int> quadrantTrackingPosX = { -45, -45 };
	static std::vector<int> quadrantTrackingPosZ = { -80, -81 };

	constexpr auto QUADRANT_TRACKING_QUAD = false;
	constexpr auto QUADRANT_TRACKING_IDX_MIN_X = -45;
	constexpr auto QUADRANT_TRACKING_IDX_MAX_X = -41;
	constexpr auto QUADRANT_TRACKING_IDX_MIN_Z = -85;
	constexpr auto QUADRANT_TRACKING_IDX_MAX_Z = -81;

	bool QuadrantPos::isQuadrantTrackedForDebug(std::string owner) const
	{
		if (not isInitialized())
			return false;

		bool quadrantTrackedForDebug = false;
			
		if (QUADRANT_TRACKING_MULTIPLE and not quadrantTrackedForDebug)
		{
			if (quadrantTrackingPosX.size() == quadrantTrackingPosZ.size())
			{
				for (size_t i = 0; i < quadrantTrackingPosX.size(); i++)
				{
					if (m_quadrantPosX == quadrantTrackingPosX[i] && m_quadrantPosZ == quadrantTrackingPosZ[i])
						quadrantTrackedForDebug = true;
				}
			}
		}

		if (QUADRANT_TRACKING_QUAD and not quadrantTrackedForDebug)
		{
			if (m_quadrantPosX >= QUADRANT_TRACKING_IDX_MIN_X && m_quadrantPosX <= QUADRANT_TRACKING_IDX_MAX_X and
				m_quadrantPosZ >= QUADRANT_TRACKING_IDX_MIN_Z && m_quadrantPosZ <= QUADRANT_TRACKING_IDX_MAX_Z)
			{
				quadrantTrackedForDebug = true;
			}
		}

		if (QUADRANT_TRACKING_SINGLE and not quadrantTrackedForDebug)
		{
			if (m_quadrantPosX == QUADRANT_TRACKING_IDX_POS_X and m_quadrantPosZ == QUADRANT_TRACKING_IDX_POS_Z)
				quadrantTrackedForDebug = true;
		}
		
		//return (isInitialized() and m_quadrantPosX == QUADRANT_TRACKING_IDX_POS_X and m_quadrantPosZ == QUADRANT_TRACKING_IDX_POS_Z);
	
		if (quadrantTrackedForDebug)
		{
			PLOG_DEBUG << "[" << owner << "] QUADRANT TRACKED FOR DEBUG " << getIdStr();
		}
		
		return quadrantTrackedForDebug;
	}

	void QuadrantPos::trackQuadrantForDebug(std::string owner, std::string text, std::string hash) const
	{
//#ifdef _DEBUG
//		return;
//#else
//		return;
//#endif	
		if (isQuadrantTrackedForDebug(owner))
		{
			PLOG_DEBUG << "[" << owner << "] TRACKING QUADRANT " << getIdStr() << (text.size() > 0 ? " - " + text : "") << (hash.size() > 0 ? " - Hash=" + TheWorld_Utils::MeshCacheBuffer::meshIdToPrint(hash) : "");
		}
	}

	bool QuadrantPos::getNearestGridVertexOffset(float worldPosX, float worldPosZ, size_t& gridX, size_t& gridY)
	{
		bool ret = false;
		gridX = gridY = 0;

		if (isInitialized())
		{
			float offsetX = worldPosX - m_lowerXGridVertex;
			float offsetZ = worldPosZ - m_lowerZGridVertex;
			if (offsetX >= 0 and offsetX <= m_sizeInWU and offsetZ >= 0 and offsetZ <= m_sizeInWU)
			{
				gridX = (size_t)std::round(offsetX / m_gridStepInWU);
				gridY = (size_t)std::round(offsetZ / m_gridStepInWU);
				ret = true;
			}
		}

		return ret;
	}

	TerrainEdit::TerrainEdit(enum class TerrainEdit::TerrainType terrainType)
	{
		size = sizeof(TerrainEdit);
		needUploadToServer(false);
		//normalsNeedRegen(true);

		minHeight = 0;
		maxHeight = 0;

		//eastSideXPlus.needBlend_notused = true;
		eastSideXPlus.minHeight = 0.0f;
		eastSideXPlus.maxHeight = 0.0f;

		//westSideXMinus.needBlend_notused = true;
		westSideXMinus.minHeight = 0.0f;
		westSideXMinus.maxHeight = 0.0f;

		//southSideZPlus.needBlend_notused = true;
		southSideZPlus.minHeight = 0.0f;
		southSideZPlus.maxHeight = 0.0f;

		//northSideZMinus.needBlend_notused = true;
		northSideZMinus.minHeight = 0.0f;
		northSideZMinus.maxHeight = 0.0f;

		TerrainEdit::terrainType = terrainType;
		init(terrainType);

		//memset(extraValues.lowElevationTexName_r, 0, sizeof(extraValues.lowElevationTexName_r));
		//memset(extraValues.highElevationTexName_g, 0, sizeof(extraValues.highElevationTexName_g));
		//memset(extraValues.dirtTexName_b, 0, sizeof(extraValues.dirtTexName_b));
		//memset(extraValues.rocksTexName_a, 0, sizeof(extraValues.rocksTexName_a));
		//splatmapNeedRegen(true);
		//colormapNeedRegen(true);
		globalmapNeedRegen(true);

		memset(extraValues1.filler1, -1, sizeof(extraValues1.filler1));
		memset(extraValues1.filler2, '-', sizeof(extraValues1.filler2));
		memset(extraValues1.filler3, '/', sizeof(extraValues1.filler3));
	}

	bool TerrainEdit::operator==(const TerrainEdit& o) const
	{
		if (size != o.size)
			return false;
		if (m_needUploadToServer != o.m_needUploadToServer)
			return false;
		//if (m_normalsNeedRegen_notUsed != o.m_normalsNeedRegen_notUsed)
		//	return false;
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
		//if (m_splatmapNeedRegen_notUsed != o.m_splatmapNeedRegen_notUsed)
		//	return false;
		//if (m_colormapNeedRegen_notUsed != o.m_colormapNeedRegen_notUsed)
		//	return false;
		if (m_globalmapNeedRegen != o.m_globalmapNeedRegen)
			return false;
		if (extraValues.slopeVerticalFactor != o.extraValues.slopeVerticalFactor)
			return false;
		if (extraValues.slopeFlatFactor != o.extraValues.slopeFlatFactor)
			return false;
		if (extraValues.dirtOnRocksFactor != o.extraValues.dirtOnRocksFactor)
			return false;
		if (extraValues.lowElevationFactor_not_used != o.extraValues.lowElevationFactor_not_used)
			return false;
		if (extraValues.highElevationFactor_not_used != o.extraValues.highElevationFactor_not_used)
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
		if (m_heightsNeedRegen != o.m_heightsNeedRegen)
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
		if (strcmp(extraValues1.northGroundTextures.lowElevationTexName_r, o.extraValues1.northGroundTextures.lowElevationTexName_r) != 0)
			return false;
		if (strcmp(extraValues1.northGroundTextures.highElevationTexName_g, o.extraValues1.northGroundTextures.highElevationTexName_g) != 0)
			return false;
		if (strcmp(extraValues1.northGroundTextures.dirtTexName_b, o.extraValues1.northGroundTextures.dirtTexName_b) != 0)
			return false;
		if (strcmp(extraValues1.northGroundTextures.rocksTexName_a, o.extraValues1.northGroundTextures.rocksTexName_a) != 0)
			return false;
		if (strcmp(extraValues1.westGroundTextures.lowElevationTexName_r, o.extraValues1.westGroundTextures.lowElevationTexName_r) != 0)
			return false;
		if (strcmp(extraValues1.westGroundTextures.highElevationTexName_g, o.extraValues1.westGroundTextures.highElevationTexName_g) != 0)
			return false;
		if (strcmp(extraValues1.westGroundTextures.dirtTexName_b, o.extraValues1.westGroundTextures.dirtTexName_b) != 0)
			return false;
		if (strcmp(extraValues1.westGroundTextures.rocksTexName_a, o.extraValues1.westGroundTextures.rocksTexName_a) != 0)
			return false;
		if (strcmp(extraValues1.northwestGroundTextures.lowElevationTexName_r, o.extraValues1.northwestGroundTextures.lowElevationTexName_r) != 0)
			return false;
		if (strcmp(extraValues1.northwestGroundTextures.highElevationTexName_g, o.extraValues1.northwestGroundTextures.highElevationTexName_g) != 0)
			return false;
		if (strcmp(extraValues1.northwestGroundTextures.dirtTexName_b, o.extraValues1.northwestGroundTextures.dirtTexName_b) != 0)
			return false;
		if (strcmp(extraValues1.northwestGroundTextures.rocksTexName_a, o.extraValues1.northwestGroundTextures.rocksTexName_a) != 0)
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
		//buffer.reserve(sizeof(TerrainEdit));
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
		size_t i1 = sizeof(TerrainEdit);				// DEBUG
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

	void TerrainEdit::getGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const
	{
		lowElevationTexName = extraValues.lowElevationTexName_r;
		highElevationTexName = extraValues.highElevationTexName_g;
		dirtTexName = extraValues.dirtTexName_b;
		rocksTexName = extraValues.rocksTexName_a;
	}

	void TerrainEdit::getNorthGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const
	{
		lowElevationTexName = extraValues1.northGroundTextures.lowElevationTexName_r;
		highElevationTexName = extraValues1.northGroundTextures.highElevationTexName_g;
		dirtTexName = extraValues1.northGroundTextures.dirtTexName_b;
		rocksTexName = extraValues1.northGroundTextures.rocksTexName_a;
	}

	void TerrainEdit::setNorthGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName)
	{
		strcpy_s(extraValues1.northGroundTextures.lowElevationTexName_r, sizeof(extraValues1.northGroundTextures.lowElevationTexName_r), lowElevationTexName.c_str());
		strcpy_s(extraValues1.northGroundTextures.highElevationTexName_g, sizeof(extraValues1.northGroundTextures.highElevationTexName_g), highElevationTexName.c_str());
		strcpy_s(extraValues1.northGroundTextures.dirtTexName_b, sizeof(extraValues1.northGroundTextures.dirtTexName_b), dirtTexName.c_str());
		strcpy_s(extraValues1.northGroundTextures.rocksTexName_a, sizeof(extraValues1.northGroundTextures.rocksTexName_a), rocksTexName.c_str());
	}

	void TerrainEdit::getWestGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const
	{
		lowElevationTexName = extraValues1.westGroundTextures.lowElevationTexName_r;
		highElevationTexName = extraValues1.westGroundTextures.highElevationTexName_g;
		dirtTexName = extraValues1.westGroundTextures.dirtTexName_b;
		rocksTexName = extraValues1.westGroundTextures.rocksTexName_a;
	}

	void TerrainEdit::setWestGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName)
	{
		strcpy_s(extraValues1.westGroundTextures.lowElevationTexName_r, sizeof(extraValues1.westGroundTextures.lowElevationTexName_r), lowElevationTexName.c_str());
		strcpy_s(extraValues1.westGroundTextures.highElevationTexName_g, sizeof(extraValues1.westGroundTextures.highElevationTexName_g), highElevationTexName.c_str());
		strcpy_s(extraValues1.westGroundTextures.dirtTexName_b, sizeof(extraValues1.westGroundTextures.dirtTexName_b), dirtTexName.c_str());
		strcpy_s(extraValues1.westGroundTextures.rocksTexName_a, sizeof(extraValues1.westGroundTextures.rocksTexName_a), rocksTexName.c_str());
	}

	void TerrainEdit::getNorthwestGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const
	{
		lowElevationTexName = extraValues1.northwestGroundTextures.lowElevationTexName_r;
		highElevationTexName = extraValues1.northwestGroundTextures.highElevationTexName_g;
		dirtTexName = extraValues1.northwestGroundTextures.dirtTexName_b;
		rocksTexName = extraValues1.northwestGroundTextures.rocksTexName_a;
	}

	void TerrainEdit::setNorthwestGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName)
	{
		strcpy_s(extraValues1.northwestGroundTextures.lowElevationTexName_r, sizeof(extraValues1.northwestGroundTextures.lowElevationTexName_r), lowElevationTexName.c_str());
		strcpy_s(extraValues1.northwestGroundTextures.highElevationTexName_g, sizeof(extraValues1.northwestGroundTextures.highElevationTexName_g), highElevationTexName.c_str());
		strcpy_s(extraValues1.northwestGroundTextures.dirtTexName_b, sizeof(extraValues1.northwestGroundTextures.dirtTexName_b), dirtTexName.c_str());
		strcpy_s(extraValues1.northwestGroundTextures.rocksTexName_a, sizeof(extraValues1.northwestGroundTextures.rocksTexName_a), rocksTexName.c_str());
	}

	void TerrainEdit::adjustValues(TerrainEdit* northSideZMinus, TerrainEdit* southSideZPlus, TerrainEdit* westSideXMinus, TerrainEdit* eastSideXPlus)
	{
		if (!noise.desideredMinHeightMandatory)
		{
			int num = 0;
			float averageHeight = 0;

			if (northSideZMinus != nullptr)
			{
				//if (not northSideZMinus->southSideZPlus.needBlend_notused)
				{
					averageHeight += (northSideZMinus->southSideZPlus.maxHeight + northSideZMinus->southSideZPlus.minHeight) / 2;
					num++;
				}
			}
			if (southSideZPlus != nullptr)
			{
				//if (not southSideZPlus->northSideZMinus.needBlend_notused)
				{
					averageHeight += (southSideZPlus->northSideZMinus.maxHeight + southSideZPlus->northSideZMinus.minHeight) / 2;
					num++;
				}
			}
			if (westSideXMinus != nullptr)
			{
				//if (not westSideXMinus->eastSideXPlus.needBlend_notused)
				{
					averageHeight += (westSideXMinus->eastSideXPlus.maxHeight + westSideXMinus->eastSideXPlus.minHeight) / 2;
					num++;
				}
			}
			if (eastSideXPlus != nullptr)
			{
				//if (not eastSideXPlus->westSideXMinus.needBlend_notused)
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
	}

	MemoryBuffer::MemoryBuffer(BYTE* in, size_t len)
	{
		set(in, len);
	}

	MemoryBuffer::MemoryBuffer(const MemoryBuffer& o)
	{
		*this = o;
	}

	MemoryBuffer::MemoryBuffer(const std::string& in)
	{
		set((BYTE*)in.c_str(), in.size());
	}

	MemoryBuffer::~MemoryBuffer(void)
	{
		clear("destructor");
	}

	void MemoryBuffer::operator=(const MemoryBuffer& o)
	{
		set(o.m_ptr, o.m_size);
	}

	void MemoryBuffer::operator=(const std::string& o)
	{
		set((BYTE*)o.c_str(), o.size());
	}

	bool MemoryBuffer::operator==(const MemoryBuffer& o) const
	{
		if (m_size != o.m_size)
			return false;
		return memcmp(m_ptr, o.m_ptr, m_size) == 0;
	}

	bool MemoryBuffer::equals(const MemoryBuffer& o) const
	{
		return *this == o;
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

	void MemoryBuffer::fromString(std::string& buffer)
	{
		set((BYTE*)buffer.c_str(), buffer.size());
	}

	void MemoryBuffer::getPart(MemoryBuffer& outBuffer, size_t offset, int64_t length)
	{
		if (length < 0)						// from start + offset to the end of the buffer
			length = size() - offset;
		if (length > int64_t(size()) - int64_t(offset))		// to avoid memory overflow
			length = int64_t(size()) - int64_t(offset);
		outBuffer.set(ptr() + offset, length);
	}
	
	void MemoryBuffer::set(const MemoryBuffer& inBuffer)
	{
		set(inBuffer.ptr(), inBuffer.size());
	}

	void MemoryBuffer::set(BYTE* in, size_t size)
	{
		resize(size);
		memcpy(m_ptr, in, size);
	}

	void MemoryBuffer::setPart(MemoryBuffer& inBuffer, size_t offset)
	{
		setPart(inBuffer.ptr(), inBuffer.size(), offset);
	}

	void MemoryBuffer::setPart(BYTE* in, size_t size, size_t offset)
	{
		reserve(offset + size);
		memcpy(m_ptr + offset, in, size);
		if (m_size < offset + size)
			m_size = offset + size;
	}

	void MemoryBuffer::setPartOfRow(BYTE* in, size_t size, size_t row, size_t col, size_t rowSizeInBytes)
	{
		// z-based col/row expressed in bytes
		size_t offset = row * rowSizeInBytes + col;
		setPart(in, size, offset);
	}

	void MemoryBuffer::append(BYTE* in, size_t size)
	{
		size_t newLen = m_size + size;
		reserve(newLen);
		memcpy(m_ptr + m_size, in, size);
		m_size += size;
	}

	void MemoryBuffer::resize(size_t newLen)
	{
		reserve(newLen);
		adjustSize(newLen);
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
	
	BYTE* MemoryBuffer::ptr(void) const
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
	bool MemoryBuffer::notEmpty(void) const
	{
		return not empty();
	}
	void MemoryBuffer::adjustSize(size_t size)
	{
		reserve(size);
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

	//void MemoryBuffer::populateUint16Vector(std::vector<uint16_t>& v) const
	//{
	//	size_t numElements = size() / sizeof(uint16_t);
	//	my_assert(size() == numElements * sizeof(uint16_t));
	//	v.assign((uint16_t*)m_ptr, (uint16_t*)m_ptr + numElements);
	//}

	void MemoryBuffer::serializeToBufferAndTruncate(BYTE* in, size_t size, size_t& offset)
	{
		serializeToBuffer(in, size, offset, true);
	}

	void MemoryBuffer::serializeToBuffer(BYTE* in, size_t _size, size_t& offset, bool truncate)
	{
		if (int64_t(offset) < 0)			// to append to the end of the buffer
			offset = size();

		reserve(offset + _size);		// to avoid memory overflow
		memcpy(ptr() + offset, in, _size);
		if (truncate)
			adjustCursorAndSize(offset, _size);
		else
			offset += _size;
	}

	void MemoryBuffer::serializeToBufferAndTruncate(MemoryBuffer& in, size_t& offset)
	{
		serializeToBuffer(in, offset, true);
	}

	void MemoryBuffer::serializeToBuffer(MemoryBuffer& in, size_t& offset, bool truncate)
	{
		serializeToBuffer<size_t>(in.size(), offset, truncate);
		serializeToBuffer(in.ptr(), in.size(), offset, truncate);
	}

	void MemoryBuffer::serializeToBufferAndTruncate(std::string& in, size_t& offset)
	{
		serializeToBuffer(in, offset, true);
	}

	void MemoryBuffer::serializeToBuffer(std::string& in, size_t& offset, bool truncate)
	{
		serializeToBuffer<size_t>(in.size(), offset, truncate);
		serializeToBuffer((BYTE*)in.c_str(), in.size(), offset, truncate);
	}

	void MemoryBuffer::serializeStringToBufferAndTruncate(std::string& s, size_t& offset)
	{
		serializeStringToBuffer(s, offset, true);
	}

	void MemoryBuffer::serializeStringToBuffer(std::string& s, size_t& offset, bool truncate)
	{
		serializeToBuffer(s, offset, truncate);
	}

	std::string MemoryBuffer::deserializeStringFromBuffer(size_t& offset) const
	{
		std::string out;
		size_t stringLength = deserializeFromBuffer<size_t>(offset);
		out = std::string((char*)((BYTE*)ptr() + offset), stringLength);
		offset += stringLength;
		return out;
	}

	void MemoryBuffer::serializeMemoryBufferToBufferAndTruncate(MemoryBuffer& buffer, size_t& offset)
	{
		serializeMemoryBufferToBuffer(buffer, offset, true);
	}

	void MemoryBuffer::serializeMemoryBufferToBuffer(MemoryBuffer& buffer, size_t& offset, bool truncate)
	{
		serializeToBuffer(buffer, offset, truncate);
	}

	MemoryBuffer MemoryBuffer::deserializeMemoryBufferFromBuffer(size_t& offset) const
	{
		MemoryBuffer out;
		size_t bufferSize = deserializeFromBuffer<size_t>(offset);
		out.set(ptr() + offset, bufferSize);
		offset += bufferSize;
		return out;
	}

	void MemoryBuffer::deserializeFromBuffer(BYTE* out, size_t _size, size_t& offset) const
	{
		my_assert(offset + _size <= size());
		memcpy(out, ptr() + offset, _size);
		offset += _size;
	}

	NamedDataBuffer::NamedDataBuffer(void)
	{
	}
	
	NamedDataBuffer::NamedDataBuffer(NamedDataBuffer& buffer)
	{
		*this = buffer;
	}

	NamedDataBuffer::NamedDataBuffer(MemoryBuffer& buffer)
	{
		attachInternalBuffer(buffer);
	}

	NamedDataBuffer::~NamedDataBuffer(void)
	{
	}

	void NamedDataBuffer::initialize(void)
	{
		if (not initialized())
			m_internalBuffer = new MemoryBuffer();
	}

	bool NamedDataBuffer::initialized(void) const
	{
		return m_internalBuffer != nullptr;
	}

	void NamedDataBuffer::deinitialize(void)
	{
		if (initialized())
		{
			delete m_internalBuffer;
			m_internalBuffer = nullptr;
		}
	}
	
	bool NamedDataBuffer::deinitialized(void) const
	{
		return not initialized();
	}

	void NamedDataBuffer::free(void)
	{
		deinitialize();
	}

	bool NamedDataBuffer::freed(void) const
	{
		return deinitialized();
	}

	void NamedDataBuffer::resetEmpty(void)
	{
		deinitialize();
		initialize();
	}

	size_t NamedDataBuffer::internalBufferSize(void) const
	{
		if (initialized())
			return m_internalBuffer->size();
		else
			return 0;
	}

	size_t NamedDataBuffer::memoryOccupation(void) const
	{
		if (initialized())
			return m_internalBuffer->reserved();
		else
			return 0;
	}

	size_t NamedDataBuffer::serializedSize(void)
	{
		initialize();
		preSerialize();
		return sizeof(size_t) + m_internalBuffer->size();
	}

	void NamedDataBuffer::preSerialize(void)
	{
		// insert min internal buffer instance variables
		initialize();
	}

	void NamedDataBuffer::preDeserialize(void)
	{
		// set instance variables reading them from internal buffer
		initialize();
	}

	void NamedDataBuffer::serialize(BYTE* ptr, size_t& offset, bool preSerializationDone)
	{
		initialize();
		if (!preSerializationDone)
			preSerialize();
		TheWorld_Utils::serializeToByteStream<size_t>(m_internalBuffer->size(), ptr, offset);
		memcpy(ptr + offset, m_internalBuffer->ptr(), m_internalBuffer->size());
		offset += m_internalBuffer->size();
	}

	void NamedDataBuffer::serialize(MemoryBuffer& buffer)
	{
		initialize();
		size_t needeBufferSize = serializedSize();
		buffer.reserve(needeBufferSize);
		buffer.adjustSize(needeBufferSize);
		size_t offset = 0;
		serialize(buffer.ptr(), offset, true);
		my_assert(offset == needeBufferSize);
	}

	void NamedDataBuffer::deserialize(BYTE* ptr, size_t& offset)
	{
		initialize();
		preDeserialize();
		size_t internalBufferSize = TheWorld_Utils::deserializeFromByteStream<size_t>(ptr, offset);
		m_internalBuffer->set(ptr + offset, internalBufferSize);
		offset += internalBufferSize;
	}

	void NamedDataBuffer::deserialize(MemoryBuffer& buffer)
	{
		initialize();
		size_t offset;
		deserialize(buffer.ptr(), offset);
	}

	void NamedDataBuffer::attachInternalBuffer(MemoryBuffer& buffer)
	{
		initialize();
		preDeserialize();
		*m_internalBuffer = buffer;
	}

	//void NamedDataBuffer::detachInternalBuffer(MemoryBuffer& buffer)
	//{
	//	initialize();
	//	buffer = getInternalBuffer();
	//	clear();
	//}
	
	MemoryBuffer& NamedDataBuffer::getInternalBuffer(void)
	{
		initialize();
		preSerialize();
		return *m_internalBuffer;
	}

	void NamedDataBuffer::operator=(NamedDataBuffer& o)
	{
		initialize();
		attachInternalBuffer(o.getInternalBuffer());
	}

	//bool NamedDataBuffer::operator==(NamedDataBuffer& o)
	//{
	//	if (initialized() != o.initialized())
	//		return false;

	//	if (not initialized())
	//		return true;		// both not initialized, so they are equal
	//	
	//	return getInternalBuffer() == o.getInternalBuffer();	// both initialized and internal buffers are equal (WRONG: we should put data in the same order to compare buffer as sequences of bytes)
	//}

	bool NamedDataBuffer::add(MemoryBuffer& buffer, std::string name, bool canReplace)
	{
		initialize();
		return addMemoryBuffer(buffer, name, canReplace, NamedDataBuffer::Type::Buffer);
	}

	bool NamedDataBuffer::add(std::string& s, std::string name, bool canReplace)
	{
		initialize();

		std::string groupName = extractFirstGroup(name);
		if (groupName.size() > 0)
		{
			// the name contains a group: inefficient way to add a string to a group, but it is not expected to be used frequently, best is to create a MemoryBuffer add elements in it and then add the MemoryBuffer as group to the NamedDataBuffer
			MemoryBuffer groupBuffer;
			int32_t groupFlags = 0;
			bool found = getGroup(groupBuffer, groupName, groupFlags);
			NamedDataBuffer groupDataBuffer(groupBuffer);
			bool added = groupDataBuffer.add(s, name);
			if (added)
				added = addGroup(groupDataBuffer, groupName);
			return added;
		}

		bool replace = false;
		int64_t dataOffset = -1;
		if (exists(name, dataOffset))
		{
			if (canReplace)
			{
				replace = true;
			}
			else
				return false;
		}

		int32_t _flags = 0;	// not used atm

		if (replace)
		{
			size_t _offset = dataOffset + sizeof(size_t) + name.size();	// skip name length and name as they are not changing
			m_internalBuffer->serializeToBuffer<int32_t>(_flags, _offset, false);
			m_internalBuffer->serializeToBuffer<int32_t>(static_cast<int32_t>(NamedDataBuffer::Type::String), _offset, false);	// new data type
			size_t offset = _offset;	// saving offset pointing to data size: offset is now pointing to the data size
			size_t oldDataSize = m_internalBuffer->deserializeFromBuffer<size_t>(_offset);	// get old data size to know how many bytes to remove from the buffer: _offset is now pointing to the data
			size_t newDataSize = s.size();
			if (newDataSize == oldDataSize)
			{
				m_internalBuffer->serializeToBuffer(s, offset, false);	// replacing size and string with the new one of the same length
			}
			else if (newDataSize < oldDataSize)
			{
				m_internalBuffer->serializeToBuffer(s, offset, false);	// replacing size and string with the new one of smaller length: now offset point to the end of the new string
				size_t sizeDiff = oldDataSize - newDataSize;
				memmove(m_internalBuffer->ptr() + offset, m_internalBuffer->ptr() + offset + sizeDiff, m_internalBuffer->size() - (offset + sizeDiff));	// move data after the string to remove the remaining bytes of the old string
				m_internalBuffer->adjustSize(m_internalBuffer->size() - sizeDiff);	// adjust buffer size to remove the remaining bytes at the end of the buffer
			}
			else	// newDataSize > oldDataSize
			{
				size_t sizeDiff = newDataSize - oldDataSize;
				size_t newBufferSize = m_internalBuffer->size() + sizeDiff;
				MemoryBuffer tempBuffer;
				m_internalBuffer->getPart(tempBuffer, _offset + oldDataSize);	// save data after the old Data to the end of the buffer to move it later
				m_internalBuffer->adjustSize(newBufferSize);	// adjust buffer size and move data after the string to make room for the remaining bytes of the new string
				m_internalBuffer->serializeToBuffer(s, offset, false);	// replacing string
				m_internalBuffer->setPart(tempBuffer, offset);	// move data after the old string to the end of the new string
				my_assert(tempBuffer.size() + offset == newBufferSize);
			}
		}
		else
		{
			size_t offset = -1;		// append to the end of the buffer
			m_internalBuffer->serializeStringToBufferAndTruncate(name, offset);
			m_internalBuffer->serializeToBufferAndTruncate<int32_t>(_flags, offset);
			m_internalBuffer->serializeToBufferAndTruncate<int32_t>(static_cast<int32_t>(NamedDataBuffer::Type::String), offset);
			m_internalBuffer->serializeToBufferAndTruncate(s, offset);
		}

		return true;
	}

	bool NamedDataBuffer::addMemoryBuffer(MemoryBuffer& buffer, std::string name, bool canReplace, enum class Type type)
	{
		initialize();

		std::string groupName = extractFirstGroup(name);
		if (groupName.size() > 0)
		{
			// the name contains a group: inefficient way to add a string to a group, but it is not expected to be used frequently, best is to create a MemoryBuffer add elements in it and then add the MemoryBuffer as group to the NamedDataBuffer
			MemoryBuffer groupBuffer;
			int32_t groupFlags = 0;
			bool found = getGroup(groupBuffer, groupName, groupFlags);
			NamedDataBuffer groupDataBuffer(groupBuffer);
			bool added = groupDataBuffer.add(buffer, name);
			if (added)
				added = addGroup(groupDataBuffer, groupName);
			return added;
		}
		
		my_assert(type == NamedDataBuffer::Type::Buffer or type == NamedDataBuffer::Type::Group);
		
		bool replace = false;
		int64_t dataOffset = -1;
		if (exists(name, dataOffset))
		{
			if (canReplace)
			{
				replace = true;
			}
			else
				return false;
		}

		int32_t _flags = 0;	// not used atm

		if (replace)
		{
			size_t _offset = dataOffset + sizeof(size_t) + name.size();	// skip name length and name as they are not changing
			m_internalBuffer->serializeToBuffer<int32_t>(_flags, _offset, false);	// new data type
			m_internalBuffer->serializeToBuffer<int32_t>(static_cast<int32_t>(type), _offset, false);	// new data type
			size_t offset = _offset;	// saving offset pointing to data size: offset is now pointing to the data size
			size_t oldDataSize = m_internalBuffer->deserializeFromBuffer<size_t>(_offset);	// get old data size to know how many bytes to remove from the buffer: _offset is now pointing to the data
			size_t newDataSize = buffer.size();
			if (newDataSize == oldDataSize)
			{
				m_internalBuffer->serializeToBuffer(buffer, offset, false);	// replacing size and string with the new one of the same length
			}
			else if (newDataSize < oldDataSize)
			{
				m_internalBuffer->serializeToBuffer(buffer, offset, false);	// replacing size and string with the new one of smaller length: now offset point to the end of the new string
				size_t sizeDiff = oldDataSize - newDataSize;
				memmove(m_internalBuffer->ptr() + offset, m_internalBuffer->ptr() + offset + sizeDiff, m_internalBuffer->size() - (offset + sizeDiff));	// move data after the string to remove the remaining bytes of the old string
				m_internalBuffer->adjustSize(m_internalBuffer->size() - sizeDiff);	// adjust buffer size to remove the remaining bytes at the end of the buffer
			}
			else	// newDataSize > oldDataSize
			{
				size_t sizeDiff = newDataSize - oldDataSize;
				size_t newBufferSize = m_internalBuffer->size() + sizeDiff;
				MemoryBuffer tempBuffer;
				m_internalBuffer->getPart(tempBuffer, _offset + oldDataSize);	// save data after the old Data to the end of the buffer to move it later
				m_internalBuffer->adjustSize(newBufferSize);	// adjust buffer size and move data after the string to make room for the remaining bytes of the new string
				m_internalBuffer->serializeToBuffer(buffer, offset, false);	// replacing string
				m_internalBuffer->setPart(tempBuffer, offset);	// move data after the old string to the end of the new string
				my_assert(tempBuffer.size() + offset == newBufferSize);
			}
		}
		else
		{
			size_t offset = -1;		// append to the end of the buffer
			m_internalBuffer->serializeStringToBufferAndTruncate(name, offset);
			m_internalBuffer->serializeToBufferAndTruncate<int32_t>(_flags, offset);
			m_internalBuffer->serializeToBufferAndTruncate<int32_t>(static_cast<int32_t>(type), offset);
			m_internalBuffer->serializeToBufferAndTruncate(buffer, offset);
		}
		
		return true;
	}

	bool NamedDataBuffer::getMemoryBuffer(MemoryBuffer& buffer, std::string name, enum class Type& type, int32_t& flags)
	{
		initialize();

		int64_t _offset = find(name);
		if (_offset < 0)
			return false;

		size_t offset = size_t(_offset);

		std::string _name;
		size_t lenBuffer;
		getElementInfo(offset, _name, type, lenBuffer, flags);

		if (!isSameType<MemoryBuffer>(type))
			return false;

		offset = offset + sizeof(size_t) + _name.size() + sizeof(int32_t) + sizeof(int32_t);	// skip name length, name, flags and type info

		buffer = m_internalBuffer->deserializeMemoryBufferFromBuffer(offset);

		return true;
	}

	bool NamedDataBuffer::addGroup(NamedDataBuffer& buffer, std::string name, bool canReplace)
	{
		initialize();
		return addGroup(buffer.getInternalBuffer(), name, canReplace);
	}

	bool NamedDataBuffer::addGroup(MemoryBuffer& buffer, std::string name, bool canReplace)
	{
		initialize();
		return addMemoryBuffer(buffer, name, canReplace, NamedDataBuffer::Type::Group);
	}
		
	bool NamedDataBuffer::getGroup(NamedDataBuffer& buffer, std::string name, int32_t& flags)
	{
		initialize();
		return getGroup(buffer.getInternalBuffer(), name, flags);
	}

	bool NamedDataBuffer::getGroup(MemoryBuffer& buffer, std::string name, int32_t& flags)
	{
		if (not initialized())
			return false;

		std::string groupName = extractFirstGroup(name);
		if (groupName.size() > 0)
		{
			// the name contains a group
			MemoryBuffer groupBuffer;
			int32_t groupFlags = 0;
			bool found = getGroup(groupBuffer, groupName, groupFlags);
			if (!found)
				return false;
			NamedDataBuffer groupDataBuffer(groupBuffer);
			return groupDataBuffer.getGroup(buffer, name, flags);
		}

		enum class Type type;
		bool found = getMemoryBuffer(buffer, name, type, flags);
		if (found and type == Type::Group)
			return true;
		else
		{
			buffer.clear("get failed");
			return false;
		}
	}
		
	bool NamedDataBuffer::get(MemoryBuffer& buffer, std::string name, int32_t& flags)
	{
		if (not initialized())
			return false;
		
		std::string groupName = extractFirstGroup(name);
		if (groupName.size() > 0)
		{
			// the name contains a group
			MemoryBuffer groupBuffer;
			int32_t groupFlags = 0;
			bool found = getGroup(groupBuffer, groupName, groupFlags);
			if (!found)
				return false;
			NamedDataBuffer groupDataBuffer(groupBuffer);
			return groupDataBuffer.get(buffer, name, flags);
		}

		enum class Type type;
		bool found = getMemoryBuffer(buffer, name, type, flags);
		if (found and type == Type::Buffer)
			return true;
		else
		{
			buffer.clear("get failed");
			return false;
		}
	}

	bool NamedDataBuffer::get(std::string& s, std::string name, int32_t& flags)
	{
		if (not initialized())
			return false;

		std::string groupName = extractFirstGroup(name);
		if (groupName.size() > 0)
		{
			// the name contains a group
			MemoryBuffer groupBuffer;
			int32_t groupFlags = 0;
			bool found = getGroup(groupBuffer, groupName, groupFlags);
			if (!found)
				return false;
			NamedDataBuffer groupDataBuffer(groupBuffer);
			return groupDataBuffer.get(s, name, flags);
		}

		int64_t _offset = find(name);
		if (_offset < 0)
			return false;

		size_t offset = size_t(_offset);

		std::string _name = name;
		enum class Type type;
		size_t lenBuffer;
		getElementInfo(offset, _name, type, lenBuffer, flags);

		if (!isSameType<std::string>(type))
			return false;

		offset = offset + sizeof(size_t) + _name.size() + sizeof(int32_t) + sizeof(int32_t);	// skip name length, name, flags and type info

		s = m_internalBuffer->deserializeStringFromBuffer(offset);

		return true;
	}

	int64_t NamedDataBuffer::find(std::string name)
	{
		// if found return the offset of the element in internal buffer, otherwise return -1

		if (not initialized())
			return false;

		if (m_internalBuffer->size() == 0)
			return -1;

		size_t offset = 0;	// starting offset for search
		while (true)
		{
			size_t _offset = offset;
			std::string _name = m_internalBuffer->deserializeStringFromBuffer(offset);
			if (_name == name)
			{
				return _offset;
			}
			//enum class Type type = static_cast<enum class Type>(m_internalBuffer->deserialize<int32_t>(offset));		// skip the type of the element
			offset += sizeof(int32_t);																		// skip flags
			offset += sizeof(int32_t);																		// skip the type of the element
			size_t lenBuffer = m_internalBuffer->deserializeFromBuffer<size_t>(offset);						// read the length of the buffer of the element
			offset += lenBuffer;																			// skip the buffer of the element

			if (offset >= m_internalBuffer->size())
				return -1;
		}
	}
	void NamedDataBuffer::getNames(std::vector<std::string>& names, std::vector<enum class Type>& types)
	{
		names.clear();
		types.clear();

		if (not initialized())
			return;

		size_t offset = 0;
		while (offset < m_internalBuffer->size())
		{
			std::string name = m_internalBuffer->deserializeStringFromBuffer(offset);
			int32_t flags = m_internalBuffer->deserializeFromBuffer<int32_t>(offset);
			enum class Type type = static_cast<enum class Type>(m_internalBuffer->deserializeFromBuffer<int32_t>(offset));
			if (type == Type::Group)
			{
				MemoryBuffer groupBuffer = m_internalBuffer->deserializeMemoryBufferFromBuffer(offset); // read the buffer of the element as group buffer
				NamedDataBuffer groupDataBuffer(groupBuffer); // read the buffer of the element as group buffer
				std::vector<std::string> groupDataBufferNames;
				std::vector<enum class Type> groupDataBufferTypes;
				groupDataBuffer.getNames(groupDataBufferNames, groupDataBufferTypes); // recursively get names in the group and add them to the list of names
				for (auto& nameInGroup : groupDataBufferNames)
				{
					nameInGroup = nameInGroup.insert(0, "."); // add the separator for group
					nameInGroup = nameInGroup.insert(0, name); // add the group name as prefix
				}
				names.insert(names.end(), groupDataBufferNames.begin(), groupDataBufferNames.end());
				types.insert(types.end(), groupDataBufferTypes.begin(), groupDataBufferTypes.end());
			}
			else
			{
				names.push_back(name);
				types.push_back(type);
				size_t lenBuffer = m_internalBuffer->deserializeFromBuffer<size_t>(offset); // read the length of the buffer of the element
				offset += lenBuffer; // skip the buffer of the element
			}
		}
	
		my_assert(names.size() == types.size());
	}

	void NamedDataBuffer::getNames(std::vector<std::string>& names)
	{
		std::vector<enum class Type> types;
		getNames(names, types);
	}

	void NamedDataBuffer::getValues(std::vector<NamedDataBuffer::NamedDataBufferData>& values)
	{
		values.clear();

		if (not initialized())
			return;
		
		std::vector<std::string> names;
		std::vector<enum class Type> types;
		getNames(names, types);

		for (int idx = 0; idx < names.size(); idx++)
		{
			enum class Type type = types[idx];
			NamedDataBufferVariant v;
			int32_t flags = 0;
			if (type != Type::NotSet && type != Type::Unknown && type != Type::Group)
			{
				switch (type)
				{
					case Type::Int32:
						v = getVariant<int32_t>(names[idx], flags);
						break;
					case Type::Int64:
						v = getVariant<int64_t>(names[idx], flags);
						break;
					case Type::Size:
						v = getVariant<size_t>(names[idx], flags);
						break;
					case Type::Float:
						v = getVariant<float>(names[idx], flags);
						break;
					case Type::Double:
						v = getVariant<double>(names[idx], flags);
						break;
					case Type::String:
						v = getVariant<std::string>(names[idx], flags);
						break;
					case Type::Buffer:
						v = getVariant<MemoryBuffer>(names[idx], flags);
						break;
				}
			}
			NamedDataBufferData value(names[idx], type, flags, v);
			values.push_back(value);
		}
	}

	void NamedDataBuffer::getValues(std::map<std::string, NamedDataBuffer::NamedDataBufferData>& values)
	{
		values.clear();

		if (not initialized())
			return;

		std::vector<std::string> names;
		std::vector<enum class Type> types;
		getNames(names, types);

		for (int idx = 0; idx < names.size(); idx++)
		{
			enum class Type type = types[idx];
			NamedDataBufferVariant v;
			int32_t flags = 0;
			if (type != Type::NotSet && type != Type::Unknown && type != Type::Group)
			{
				switch (type)
				{
				case Type::Int32:
					v = getVariant<int32_t>(names[idx], flags);
					break;
				case Type::Int64:
					v = getVariant<int64_t>(names[idx], flags);
					break;
				case Type::Size:
					v = getVariant<size_t>(names[idx], flags);
					break;
				case Type::Float:
					v = getVariant<float>(names[idx], flags);
					break;
				case Type::Double:
					v = getVariant<double>(names[idx], flags);
					break;
				case Type::String:
					v = getVariant<std::string>(names[idx], flags);
					break;
				case Type::Buffer:
					v = getVariant<MemoryBuffer>(names[idx], flags);
					break;
				}
			}
			NamedDataBufferData value(names[idx], type, flags, v);
			values[names[idx]] = value;
		}
	}

	void NamedDataBuffer::getElementInfo(size_t offset, std::string& name, enum class Type& type, size_t& lenBuffer, int32_t& flags)
	{
		name = m_internalBuffer->deserializeStringFromBuffer(offset);
		flags = m_internalBuffer->deserializeFromBuffer<int32_t>(offset);
		type = static_cast<enum class Type>(m_internalBuffer->deserializeFromBuffer<int32_t>(offset));
		lenBuffer = m_internalBuffer->deserializeFromBuffer<size_t>(offset);
	}

	std::string NamedDataBuffer::extractFirstGroup(std::string& name)
	{
		size_t pos = name.find_first_of(".");
		if (pos == std::string::npos)
			return "";
		else
		{
			std::string group = name.substr(0, pos);
			name = name.substr(pos + 1);
			return group;
		}
	}

	bool NamedDataBuffer::remove(std::string name)
	{
		int64_t _offset = find(name);
		if (_offset < 0)
			return false;

		size_t offset = size_t(_offset);

		std::string _name;
		enum class Type type;
		size_t lenBuffer;
		int32_t flags;
		getElementInfo(offset, _name, type, lenBuffer, flags);

		size_t totalSizeElement = sizeof(size_t) + _name.size() + sizeof(int32_t) + sizeof(int32_t) + sizeof(size_t) + lenBuffer;
		memcpy(m_internalBuffer->ptr() + offset, m_internalBuffer->ptr() + offset + totalSizeElement, m_internalBuffer->size() - (offset + totalSizeElement));
		m_internalBuffer->adjustSize(m_internalBuffer->size() - totalSizeElement);

		return true;
	}

	bool NamedDataBuffer::exists(std::string name)
	{
		int64_t offset = -1;
		return exists(name, offset);
	}

	bool NamedDataBuffer::exists(std::string name, int64_t& offset)
	{
		offset = find(name);
		return offset >= 0;
	}

	bool NamedDataBuffer::emptyVariant(NamedDataBufferVariant& v)
	{
		return std::holds_alternative<std::monostate>(v);
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

	bool MeshCacheBuffer::firstMeshIdMoreRecent(std::string firstMeshId, std::string secondMeshId)
	{
		return firstMeshIdMoreRecentRef(firstMeshId, secondMeshId);
	}
		
	bool MeshCacheBuffer::firstMeshIdMoreRecentRef(std::string& firstMeshId, std::string& secondMeshId)
	{
		if (firstMeshId == secondMeshId)
			return false;
		
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

		if (firstTime == secondTime)	// useless to compare, because we have PREVIOUSLY checked if meshids are equal
			return false;

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
	
	void MeshCacheBuffer::getTerrainEdit(TerrainEdit& terrainEdit, bool workDir, bool updateLastAccessTime)
	{
		getMeshIdFromDisk(workDir, &terrainEdit, updateLastAccessTime);
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
		//size_t size_t_size = sizeof(size_t);

		FILE* inFile = nullptr;
		errno_t err = fopen_s(&inFile, cacheFilePath.c_str(), "rb");
		if (err != 0)
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Open " + cacheFilePath + " in errore - Err=" + std::to_string(err)).c_str()));

		if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t bufferSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		if (fread(shortBuffer, 1, 1, inFile) != 1)	// bypass buffer format "0"/"1"
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 1!").c_str()));
		}

		// read the serialized size of the mesh id
		if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
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
			if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
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
		QuadrantPos pos(m_lowerXGridVertex, m_lowerZGridVertex, m_level, int(m_numVerticesPerSize), m_gridStepInWU);
		return "WORLD QUADRANT " + m_mapName + " " + pos.getIdStr() + " Hash=" + meshIdToPrint(m_meshId);
	}

	bool MeshCacheBuffer::refreshMapsFromDisk(size_t numVerticesPerSize, float gridStepInWU, std::string _meshId, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float32HeightsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool overrideEmptyMaps)
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
				TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.3 ") + __FUNCTION__, "refreshMapsFromBuffer");

				refreshMapsFromBuffer(tempBuffer, _meshId, terrainEditValues, minAltitude, maxAltitude, float32HeightsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, namedDataBuffer, false, overrideEmptyMaps);
			}

			empty = true;
		}
		else
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromDisk 1.4  ") + __FUNCTION__, "refreshMapsFromBuffer");
			std::string meshIdFromBuffer;
			refreshMapsFromBuffer(buffer, meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float32HeightsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, namedDataBuffer, false, overrideEmptyMaps);
			//assert(_meshId.size() == 0 || meshIdFromBuffer == _meshId);
			if (_meshId.size() > 0 && meshIdFromBuffer != _meshId)
			{
				PLOG_DEBUG << "meshId from cache buffer (" << meshIdFromBuffer << ") not equal to meshId in input (" << _meshId;
				//throw(GDN_TheWorld_Exception(__FUNCTION__, (std::string("meshId from cache buffer (") + meshIdFromBuffer + ") not equal to meshId in input (" + _meshId).c_str()));
			}
		}

		return empty;
	}

	/*
	* Buffers are serialized in MeshCacheBuffer::setBufferFromCacheQuadrantData
	*/
	bool MeshCacheBuffer::refreshMapsFromBuffer(const BYTE* buffer, const size_t bufferSize, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float32HeightsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromBuffer 1 ") + __FUNCTION__, "ALL");

		bool dummyBuffer = false;
		
		if (bufferSize == 0)
		{
			meshIdFromBuffer.clear();
			dummyBuffer = true;
			return dummyBuffer;
		}

		//size_t uint16_t_size = sizeof(uint16_t);	// the size of an half ==> float_16
		//size_t float_size = sizeof(float);

		size_t size = 0;

		const char* movingStreamBuffer = (const char*)buffer;
		const char* endOfBuffer = movingStreamBuffer + bufferSize;

//#define BUFFER_FORMAT_VERSION_0 (bufferFormat == '0')	// Default buffer format, without the size of float16 and float32 height maps
#define BUFFER_FORMAT_VERSION_1 (bufferFormat == '1')
		char bufferFormat = *movingStreamBuffer;

		movingStreamBuffer++;	// bypass buffer format "0"/"1"

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

		TheWorld_Utils::QuadrantPos pos = getQuadrantPos();
		PLOG_VERBOSE << "Refreshing maps for quadrant " << pos.getIdStr() << " hash=" << TheWorld_Utils::MeshCacheBuffer::meshIdToPrint1(meshIdFromBuffer);
		//pos.TRACK_QUADRANT_FOR_DEBUG("Refreshing maps", meshIdFromBuffer);

		if (vectSize > 0)
		{
			minAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;

			maxAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;

			bool heightsAsFloat16InBuffer = true;

			size_t float16HeightMapSize = vectSize * sizeof(uint16_t);	// default value, for backward compatibility with old BUFFER_FORMAT_VERSION_0 buffers (if heights are uint16 there is one spare dummy height at the beginning and its value express the format of the heightmap)
			if (BUFFER_FORMAT_VERSION_1)
			{
				float16HeightMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				if (float16HeightMapSize == (vectSize + 1) * sizeof(uint16_t))	// backward compatibility with old BUFFER_FORMAT_VERSION_1 buffers (if heights are uint16 there is one spare dummy height at the beginning and its value express the format of the heightmap)
				{
					uint16_t heightmap16Format = TheWorld_Utils::deserializeFromByteStream<uint16_t>((BYTE*)movingStreamBuffer, size);	// fetch first height
					movingStreamBuffer += size;																							// and bypass it
					my_assert(heightmap16Format == uint16_t(HeightsFormatInQuadrantBuffer::Float16) or heightmap16Format == uint16_t(HeightsFormatInQuadrantBuffer::UInt16));
					if (heightmap16Format == uint16_t(HeightsFormatInQuadrantBuffer::Float16))
						heightsAsFloat16InBuffer = true;
					else
						heightsAsFloat16InBuffer = false;
					float16HeightMapSize = vectSize * sizeof(uint16_t);					// adjust size of the heightmap to exclude the first height
				}
				else
					my_assert(float16HeightMapSize == 0 or float16HeightMapSize == vectSize * sizeof(uint16_t));
			}
			TheWorld_Utils::MemoryBuffer float16HeightsBuffer;
			float16HeightsBuffer.set((BYTE*)movingStreamBuffer, float16HeightMapSize);
			movingStreamBuffer += float16HeightMapSize;

			//// DEBUG
			//if (pos.isQuadrantTrackedForDebug())
			//{
			//	if (terrainEditValues.size() > 0)
			//	{
			//		TerrainEdit terrainEdit;
			//		terrainEdit.deserialize(terrainEditValues);
			//		bool b = TheWorld_Utils::MeshCacheBuffer::debugCheckMinMaxHeight(pos, &terrainEdit, "refreshMapsFromBuffer", &meshIdFromBuffer, &float16HeightsBuffer, nullptr);
			//	}
			//}
			//// DEBUG

			size_t float32HeightMapSize = vectSize * sizeof(float);		// default value, for backward compatibility with old BUFFER_FORMAT_VERSION_0 buffers (if heights are uint16 there is one spare dummy height at the beginning and its value express the format of the heightmap)
			if (BUFFER_FORMAT_VERSION_1)
			{
				float32HeightMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				my_assert(float32HeightMapSize == 0 or float32HeightMapSize == vectSize * sizeof(float));
			}
			if (float32HeightMapSize == 0)
			{
				if (float16HeightsBuffer.size() == 0)
				{
					//float32HeightsBuffer.clear(__FUNCTION__);
					my_assert(false);
				}
				else
				{
					if (heightsAsFloat16InBuffer)
					{
						// heightmap16Format == uint16_t(HeightsFormatInQuadrantBuffer::Float16
						TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromBuffer 1.1 ") + __FUNCTION__, "calc heights 32 from float16");
						float32HeightMapSize = vectSize * sizeof(float);
						float32HeightsBuffer.reserve(float32HeightMapSize);
						float32HeightsBuffer.adjustSize(float32HeightMapSize);
						float* pFloat32 = float32HeightsBuffer.ptr<float>();
						uint16_t* pFloat16 = float16HeightsBuffer.ptr<uint16_t>();
						//size_t numVerticesPerSize = (size_t)sqrt((double)vectSize);
						for (int i = 0; i < vectSize; i++)
						{
							pFloat32[i] = TheWorld_Utils::MeshCacheBuffer::halfToFloat(pFloat16[i]);
						}
					}
					else
					{
						// heightmap16Format == uint16_t(HeightsFormatInQuadrantBuffer::UInt16
						TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshMapsFromBuffer 1.1 ") + __FUNCTION__, "calc heights 32 from uint16 heights");
						float32HeightMapSize = vectSize * sizeof(float);
						float32HeightsBuffer.reserve(float32HeightMapSize);
						float32HeightsBuffer.adjustSize(float32HeightMapSize);
						float* pFloat32 = float32HeightsBuffer.ptr<float>();
						uint16_t* pFloat16 = float16HeightsBuffer.ptr<uint16_t>();
						//size_t numVerticesPerSize = (size_t)sqrt((double)vectSize);
						for (int i = 0; i < vectSize; i++)
						{
							pFloat32[i] = TheWorld_Utils::MeshCacheBuffer::uint16HeightToFloat(pFloat16[i]);
						}
					}
				}
			}
			else
			{
				if (overrideEmptyMaps or float32HeightsBuffer.empty())
				{
					float32HeightsBuffer.set((BYTE*)movingStreamBuffer, float32HeightMapSize);
					movingStreamBuffer += float32HeightMapSize;
				}
			}

			size_t normalMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGB);
			if (BUFFER_FORMAT_VERSION_1)
			{
				normalMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				if (normalMapSize == 0)
				{
					if (overrideEmptyMaps)
						normalsBuffer.clear(__FUNCTION__);
				}
				else
				{
					if (overrideEmptyMaps or normalsBuffer.empty())
						normalsBuffer.set((BYTE*)movingStreamBuffer, normalMapSize);
					movingStreamBuffer += normalMapSize;
				}
			}
			else
			{
				struct TheWorld_Utils::_RGB* rgb = (struct TheWorld_Utils::_RGB*)movingStreamBuffer;
				if (rgb->r == 0 && rgb->g == 0 && rgb->b == 0)
				{
					if (overrideEmptyMaps)
						normalsBuffer.clear(__FUNCTION__);
					movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGB);
				}
				else
				{
					if (overrideEmptyMaps or normalsBuffer.empty())
						normalsBuffer.set((BYTE*)movingStreamBuffer, normalMapSize);
					movingStreamBuffer += normalMapSize;
				}
			}

			size_t splatmapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGBA);
			if (BUFFER_FORMAT_VERSION_1)
			{
				splatmapMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				if (splatmapMapSize == 0)
				{
					if (overrideEmptyMaps)
						splatmapBuffer.clear(__FUNCTION__);
				}
				else
				{
					if (overrideEmptyMaps or splatmapBuffer.empty())
						splatmapBuffer.set((BYTE*)movingStreamBuffer, splatmapMapSize);
					movingStreamBuffer += splatmapMapSize;
				}
			}
			else
			{
				if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGBA) <= endOfBuffer)
				{
					struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)movingStreamBuffer;
					if (rgba->r == 0 && rgba->g == 0 && rgba->b == 0 && rgba->a == 0)
					{
						if (overrideEmptyMaps)
							splatmapBuffer.clear(__FUNCTION__);
						movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
					}
					else
					{
						if (overrideEmptyMaps or splatmapBuffer.empty())
							splatmapBuffer.set((BYTE*)movingStreamBuffer, splatmapMapSize);
						movingStreamBuffer += splatmapMapSize;
					}
				}
				else
				{
					if (overrideEmptyMaps)
						splatmapBuffer.clear(__FUNCTION__);
				}
			}
			
			size_t colormapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGBA);
			if (BUFFER_FORMAT_VERSION_1)
			{
				colormapMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				if (colormapMapSize == 0)
				{
					if (overrideEmptyMaps)
						colormapBuffer.clear(__FUNCTION__);
				}
				else
				{
					if (overrideEmptyMaps or colormapBuffer.empty())
						colormapBuffer.set((BYTE*)movingStreamBuffer, colormapMapSize);
					movingStreamBuffer += colormapMapSize;
				}
			}
			else
			{
				if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGBA) <= endOfBuffer)
				{
					struct TheWorld_Utils::_RGBA* rgba = (struct TheWorld_Utils::_RGBA*)movingStreamBuffer;
					if ((rgba->r == 0 && rgba->g == 0 && rgba->b == 0 && rgba->a == 0) /* || (rgba->r == 255 && rgba->g == 255 && rgba->b == 255 && rgba->a == 255)*/)
					{
						if (overrideEmptyMaps)
							colormapBuffer.clear(__FUNCTION__);
						movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGBA);
					}
					else
					{
						if (overrideEmptyMaps or colormapBuffer.empty())
							colormapBuffer.set((BYTE*)movingStreamBuffer, colormapMapSize);
						movingStreamBuffer += colormapMapSize;
					}
				}
				else
				{
					if (overrideEmptyMaps)
						colormapBuffer.clear(__FUNCTION__);
				}
			}

			size_t globalmapMapSize = vectSize * sizeof(struct TheWorld_Utils::_RGB);
			if (BUFFER_FORMAT_VERSION_1)
			{
				globalmapMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
				movingStreamBuffer += size;
				if (globalmapMapSize == 0)
				{
					if (overrideEmptyMaps)
						globalmapBuffer.clear(__FUNCTION__);
				}
				else
				{
					if (overrideEmptyMaps or globalmapBuffer.empty())
						globalmapBuffer.set((BYTE*)movingStreamBuffer, globalmapMapSize);
					movingStreamBuffer += globalmapMapSize;
				}
			}
			else
			{
				if (movingStreamBuffer + sizeof(struct TheWorld_Utils::_RGB) <= endOfBuffer)
				{
					struct TheWorld_Utils::_RGB* rgb = (struct TheWorld_Utils::_RGB*)movingStreamBuffer;
					if ((rgb->r == 0 && rgb->g == 0 && rgb->b == 0) /* || (rgb->r == 255 && rgb->g == 255 && rgb->b == 255)*/)
					{
						if (overrideEmptyMaps)
							globalmapBuffer.clear(__FUNCTION__);
						movingStreamBuffer += sizeof(struct TheWorld_Utils::_RGB);
					}
					else
					{
						if (overrideEmptyMaps or globalmapBuffer.empty())
							globalmapBuffer.set((BYTE*)movingStreamBuffer, globalmapMapSize);
						movingStreamBuffer += globalmapMapSize;
					}
				}
				else
				{
					if (overrideEmptyMaps)
						globalmapBuffer.clear(__FUNCTION__);
				}
			}

			if (movingStreamBuffer + sizeof(size_t) <= endOfBuffer)
			{
				if (overrideEmptyMaps or namedDataBuffer.freed())
					namedDataBuffer.deserialize((BYTE*)movingStreamBuffer, size);
			}
			else
			{
				if (overrideEmptyMaps or namedDataBuffer.freed())
					namedDataBuffer.resetEmpty();
			}

			if (updateCache)
			{
				std::string meshId;
				TheWorld_Utils::TerrainEdit writtenTerrainEdit;
				writeBufferToDiskCache(buffer, bufferSize, meshId, false, false, &writtenTerrainEdit);
			}
		}
		else
		{
			dummyBuffer = true;
			minAltitude = maxAltitude = 0;
			if (overrideEmptyMaps)
			{
				float32HeightsBuffer.clear(__FUNCTION__);
				normalsBuffer.clear(__FUNCTION__);
				splatmapBuffer.clear(__FUNCTION__);
				colormapBuffer.clear(__FUNCTION__);
				globalmapBuffer.clear(__FUNCTION__);
				namedDataBuffer.resetEmpty();
			}
		}

		return dummyBuffer;
	}

	bool MeshCacheBuffer::refreshMapsFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float32HeightsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps)
	{
		return refreshMapsFromBuffer(buffer.ptr(), buffer.size(), meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float32HeightsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, namedDataBuffer, updateCache, overrideEmptyMaps);
	}

	bool MeshCacheBuffer::refreshMapsFromBuffer(std::string& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitude, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float32HeightsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps)
	{
		return refreshMapsFromBuffer((BYTE*)buffer.c_str(), buffer.size(), meshIdFromBuffer, terrainEditValues, minAltitude, maxAltitude, float32HeightsBuffer, normalsBuffer, splatmapBuffer, colormapBuffer, globalmapBuffer, namedDataBuffer, updateCache, overrideEmptyMaps);
	}

	bool MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(const BYTE* buffer, const size_t bufferSize, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps)
	{
		return refreshMapsFromBuffer(buffer, bufferSize, cacheQuadrantData.meshId, *cacheQuadrantData.terrainEditValues, cacheQuadrantData.minHeight, cacheQuadrantData.maxHeight, *cacheQuadrantData.heights32Buffer, *cacheQuadrantData.normalsBuffer, *cacheQuadrantData.splatmapBuffer, *cacheQuadrantData.colormapBuffer, *cacheQuadrantData.globalmapBuffer, *cacheQuadrantData.namedDataBuffer, updateCache, overrideEmptyMaps);
	}

	bool MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps)
	{
		return refreshCacheQuadrantDataFromBuffer(buffer.ptr(), buffer.size(), cacheQuadrantData, updateCache, overrideEmptyMaps);
	}

	bool MeshCacheBuffer::refreshCacheQuadrantDataFromBuffer(std::string& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps)
	{
		return refreshCacheQuadrantDataFromBuffer((BYTE*)buffer.c_str(), buffer.size(), cacheQuadrantData, updateCache, overrideEmptyMaps);
	}

	void MeshCacheBuffer::refreshInternalCacheQuadrantDataFromBuffer(std::string& buffer, bool force)
	{
		if (force || !m_internalCacheQuadrantData.buffersAllocated())
		{
			m_internalCacheQuadrantData.allocateBuffers(); 
			refreshCacheQuadrantDataFromBuffer(buffer, m_internalCacheQuadrantData, false, true);
		}
	}

	bool MeshCacheBuffer::refreshInternalCacheQuadrantDataFromDisk(bool force, bool renewMeshId)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshInternalCacheQuadrantDataFromDisk 1 ") + __FUNCTION__, "ALL");

		bool refreshed = false;
		
		if (force || !m_internalCacheQuadrantData.buffersAllocated())
		{
			bool workDir = true;

			TheWorld_Utils::MeshCacheBuffer cacheWorkDir = *this;
			std::string workMeshId = cacheWorkDir.getMeshIdFromDisk(true);
			TheWorld_Utils::MeshCacheBuffer cache = *this;
			std::string meshId = cache.getMeshIdFromDisk(false);
			if (meshId.size() == 0)
				workDir = true;
			else if (workMeshId.size() == 0)
				workDir = false;
			else
			{
				if (firstMeshIdMoreRecentRef(workMeshId, meshId))
					workDir = true;
				else
					workDir = false;
			}

			refreshed = refreshInternalCacheQuadrantDataFromDisk(workDir, force, renewMeshId);
		}

		return refreshed;
	}
		
	bool MeshCacheBuffer::refreshInternalCacheQuadrantDataFromDisk(bool workDir, bool force, bool renewMeshId)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer refreshInternalCacheQuadrantDataFromDisk 2 ") + __FUNCTION__, "ALL");

		bool refreshed = false;

		if (force || !m_internalCacheQuadrantData.buffersAllocated())
		{
			refreshed = true;

			std::string buffer;
			TerrainEdit terr;
			readBufferFromDisk(m_meshId, buffer, workDir, &terr);
			m_internalCacheQuadrantData.allocateBuffers();
			bool renewed = false;
			if (m_meshId.length() == 0 || buffer.length() == 0)
			{
				std::string meshid;
				setEmptyBuffer(getNumVerticesPerSize(), meshid, buffer);
				renewed = true;
			}
			refreshCacheQuadrantDataFromBuffer(buffer, m_internalCacheQuadrantData, false, true);
			if (renewMeshId && !renewed)
			{
				m_internalCacheQuadrantData.meshId = generateNewMeshId();
				m_meshId = m_internalCacheQuadrantData.meshId;
			}
			//m_internalCacheQuadrantData.terrainData()->deserialize(*m_internalCacheQuadrantData.terrainEditValues);
		}

		return refreshed;
	}

	MeshCacheBuffer::CacheQuadrantData* MeshCacheBuffer::internalCacheQuadrantData(void)
	{
		return &m_internalCacheQuadrantData;
	}

	
	void MeshCacheBuffer::readBufferFromDisk(std::string _meshId, TheWorld_Utils::MemoryBuffer& buffer, bool workDir, TerrainEdit* terrainEdit, bool onlyTerrainEdit)
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

		//size_t size_t_size = sizeof(size_t);	// get size of a size_t
		//size_t uint16_t_size = sizeof(uint16_t);	// the size of an half ==> float_16
		//size_t float_size = sizeof(float);

		FILE* inFile = nullptr;
		errno_t err = fopen_s(&inFile, cacheFilePath.c_str(), "rb");
		if (err != 0)
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Open " + cacheFilePath + " in errore - Err=" + std::to_string(err)).c_str()));

		// read the buffer size
		if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 2!").c_str()));
		}
		// and deserialize it
		size_t streamBufferSize = TheWorld_Utils::deserializeFromByteStream<size_t>(shortBuffer, size);

		if (fread(shortBuffer, 1, 1, inFile) != 1)	// bypass buffer format "0"/"1"
		{
			fclose(inFile);
			throw(GDN_TheWorld_Exception(__FUNCTION__, std::string("Read error 1!").c_str()));
		}

		// read the serialized size of the mesh id
		if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
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
			if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
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

			if (onlyTerrainEdit)
			{
				fclose(inFile);
				buffer.clear(__FUNCTION__);
				return;
			}
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
		if (fread(shortBuffer, sizeof(size_t), 1, inFile) != 1)
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
		
	std::string MeshCacheBuffer::writeBufferToDiskCache2(std::string& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache((BYTE*)buffer.c_str(), buffer.size(), writtenMeshId, renewMeshId, workDir, writtenTerrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(std::string& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache((BYTE*)buffer.c_str(), buffer.size(), writtenMeshId, renewMeshId, workDir, writtenTerrainEdit);
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache2(TheWorld_Utils::MemoryBuffer& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer.ptr(), buffer.size(), writtenMeshId, renewMeshId, workDir, writtenTerrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(TheWorld_Utils::MemoryBuffer& buffer, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer.ptr(), buffer.size(), writtenMeshId, renewMeshId, workDir, writtenTerrainEdit);
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache2(const BYTE* buffer, const size_t bufferSize, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		std::string bufferMeshId = writeBufferToDiskCache(buffer, bufferSize, writtenMeshId, renewMeshId, workDir, writtenTerrainEdit);
		m_meshId = writtenMeshId;
		return bufferMeshId;
	}

	std::string MeshCacheBuffer::writeBufferToDiskCache(const BYTE* buffer, const size_t bufferSize, std::string& writtenMeshId, bool renewMeshId, bool workDir, TerrainEdit* writtenTerrainEdit)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer writeBufferToCache 1 ") + __FUNCTION__, "ALL");

		std::string meshFilePath = getCacheFilePath();
		if (workDir)
			meshFilePath = getWorkCacheFilePath();
		std::string tempPath = meshFilePath + ".tmp";
		std::string path = meshFilePath;

		BYTE shortBuffer[256 + 1];
		size_t bufferSize_size = 0;	// get size of a size_t

		char* movingStreamBuffer = (char*)buffer;
		size_t size = 0;

//#define BUFFER_FORMAT_VERSION_0 (bufferFormat == '0')
//#define BUFFER_FORMAT_VERSION_1 (bufferFormat == '1')
		char bufferFormat = *movingStreamBuffer;
		movingStreamBuffer++;	// bypass buffer format "0"/"1"

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

		TheWorld_Utils::QuadrantPos pos = getQuadrantPos();
		PLOG_VERBOSE << "Quadrant " << pos.getIdStr() << " saving to " << (workDir ? "work " : "") << "disk cache with hash[" << TheWorld_Utils::MeshCacheBuffer::meshIdToPrint1(writtenMeshId) << "]";
		//pos.TRACK_QUADRANT_FOR_DEBUG("Saved to disk cache", writtenMeshId);

		TerrainEdit _writtenTerrainEdit;
		{
			size_t terrainEditSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
			movingStreamBuffer += size;			// bypass TerrainEdit size
			TheWorld_Utils::MemoryBuffer terrainEditBuffer((BYTE*)movingStreamBuffer, terrainEditSize);
			_writtenTerrainEdit.deserialize(terrainEditBuffer);
			movingStreamBuffer += terrainEditSize;			// bypass TerrainEdit

			//// DEBUG
			//if (pos.isQuadrantTrackedForDebug())
			//{
			//	std::string s = "MinMax " + std::to_string(_writtenTerrainEdit.minHeight) + "/" + std::to_string(_writtenTerrainEdit.maxHeight)
			//		+ " N=" + std::to_string(_writtenTerrainEdit.northSideZMinus.minHeight) + "/" + std::to_string(_writtenTerrainEdit.northSideZMinus.maxHeight)
			//		+ " S=" + std::to_string(_writtenTerrainEdit.southSideZPlus.minHeight) + "/" + std::to_string(_writtenTerrainEdit.southSideZPlus.maxHeight)
			//		+ " E=" + std::to_string(_writtenTerrainEdit.eastSideXPlus.minHeight) + "/" + std::to_string(_writtenTerrainEdit.eastSideXPlus.maxHeight)
			//		+ " W=" + std::to_string(_writtenTerrainEdit.westSideXMinus.minHeight) + "/" + std::to_string(_writtenTerrainEdit.westSideXMinus.maxHeight);
			//	pos.TRACK_QUADRANT_FOR_DEBUG(std::string("Updated quadrant server Cache - ") + s, writtenMeshId);
			//}
			//// DEBUG
		}

		//// DEBUG
		//if (pos.isQuadrantTrackedForDebug())
		//{
		//	size_t vectSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
		//	movingStreamBuffer += size;

		//	if (vectSize > 0)
		//	{
		//		float minAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
		//		movingStreamBuffer += size;

		//		float maxAltitude = TheWorld_Utils::deserializeFromByteStream<float>((BYTE*)movingStreamBuffer, size);
		//		movingStreamBuffer += size;

		//		size_t float16HeightMapSize = vectSize * sizeof(uint16_t);
		//		if (BUFFER_FORMAT_VERSION_1)
		//		{
		//			float16HeightMapSize = TheWorld_Utils::deserializeFromByteStream<size_t>((BYTE*)movingStreamBuffer, size);
		//			movingStreamBuffer += size;
		//			my_assert(float16HeightMapSize == vectSize * sizeof(uint16_t));
		//		}
		//		TheWorld_Utils::MemoryBuffer float16HeightsBuffer;
		//		float16HeightsBuffer.set((BYTE*)movingStreamBuffer, float16HeightMapSize);
		//		movingStreamBuffer += float16HeightMapSize;

		//		bool b = TheWorld_Utils::MeshCacheBuffer::debugCheckMinMaxHeight(pos, &_writtenTerrainEdit, "writeBufferToDiskCache",  & writtenMeshId, &float16HeightsBuffer, nullptr);
		//	}
		//}
		//// DEBUG


		if (writtenTerrainEdit != nullptr)
		{
			*writtenTerrainEdit = _writtenTerrainEdit;
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

	void MeshCacheBuffer::SmoothHeightmap(TheWorld_Utils::QuadrantPos& pos, std::vector<float>& heightmap, size_t smoothAmplitude, float smoothFactor)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer SmoothHeightmap 1 ") + __FUNCTION__, "ALL");

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		// smoothAmplitude must be less than numVerticesPerSize and odd
		{
			if (smoothAmplitude >= numVerticesPerSize)
				smoothAmplitude = numVerticesPerSize - 1;

			if (smoothAmplitude % 2 == 0)
			{
				// smoothAmplitude is even
				smoothAmplitude--;	// force to be odd
			}

			if (smoothAmplitude < 0)
				smoothAmplitude = 1;
		}

		std::vector<float> tempHeightmap = heightmap; // Create a copy of the original heightmap

		size_t offset = (smoothAmplitude - 1) / 2;

		for (size_t y = offset; y < numVerticesPerSize - offset; y++) {
			for (size_t x = offset; x < numVerticesPerSize - offset; x++) {
				// Media del vicinato offset x offset
				float sum = 0;
				for (int ky = -int(offset); ky <= int(offset); ky++) {
					for (int kx = -int(offset); kx <= int(offset); kx++) {
						sum += tempHeightmap[(y + ky) * numVerticesPerSize + (x + kx)];
					}
				}
				float average = sum / (smoothAmplitude * smoothAmplitude);

				// Applica lo smoothing bilanciando il valore originale
				heightmap[y * numVerticesPerSize + x] = std::lerp(tempHeightmap[y * numVerticesPerSize + x], average, smoothFactor);
			}
		}
	}

	//// Funzione ipotetica che restituisce un rumore base tra -1.0 e 1.0
	//float perlinNoise2D(float x, float y);

	//// Genera un'altezza smooth usando le ottave
	//float getSmoothHeight(float x, float y, FastNoiseLite& noise)
	//{
	//	float total = 0.0f;
	//	float frequency = 1.0f;
	//	float amplitude = 1.0f;
	//	float maxValue = 0.0f;  // Usato per normalizzare il risultato

	//	int octaves = noise.SetFractalOctaves
	//	float lacunarity = 
	//	float gain = 

	//	for (int i = 0; i < octaves; i++) {
	//		total += perlinNoise2D(x * frequency, y * frequency) * amplitude;

	//		maxValue += amplitude;

	//		frequency *= lacunarity; // Aumenta la frequenza (es. 2.0: raddoppia il dettaglio)
	//		amplitude *= gain;       // Riduce l'ampiezza (es. 0.5: dimezza l'impatto del dettaglio)
	//	}

	//	// Ritorna un valore normalizzato tra -1.0 e 1.0, o scalato per la tua altezza massima
	//	return total / maxValue;
	//}


	void MeshCacheBuffer::generateHeightsWithNoise(TheWorld_Utils::QuadrantPos& pos, TerrainEdit* terrainEdit, std::vector<float>& heightmap)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1 ") + __FUNCTION__, "ALL");

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();
		float gridStepInWU = pos.getGridStepInWU();
		float lowerXGridVertex = pos.getLowerXGridVertex();
		float lowerZGridVertex = pos.getLowerZGridVertex();
		//lowerXGridVertex = lowerZGridVertex = 0.0f;	// DEBUG for now, we generate the heights in local coordinates

		heightmap.clear();
		heightmap.resize(numVerticesPerSize * numVerticesPerSize);

		bool adjustHeights = true;
		
		static std::recursive_mutex s_mtxFlatBuffer;
		static std::vector<float> s_vectFlatGridHeights;
		static std::recursive_mutex s_mtxEmptyBuffer;
		static std::vector<float> s_vectEmptyGridHeights;

		/*
		* Generating Heights for flat or empty terrain types, we can optimize by using a precomputed buffer of heights, since all heights are the same. This avoids unnecessary calculations and improves performance.
		*/
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

			{
				std::lock_guard<std::recursive_mutex> lock(s_mtxFlatBuffer);
				if (s_vectFlatGridHeights.empty())
				{
					s_vectFlatGridHeights.clear();
					s_vectFlatGridHeights.resize(numVerticesPerSize * numVerticesPerSize);
					float altitude = FLAT_QUADRANTS_TERRAIN_HEIGHT;
					size_t idx = 0;
					for (int z = 0; z < numVerticesPerSize; z++)
						for (int x = 0; x < numVerticesPerSize; x++)
						{
							s_vectFlatGridHeights[idx] = altitude;
							idx++;
						}
				}
			}

			heightmap = s_vectFlatGridHeights;
			adjustHeights = false;
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
			
			{
				std::lock_guard<std::recursive_mutex> lock(s_mtxEmptyBuffer);
				if (s_vectEmptyGridHeights.empty())
				{
					s_vectEmptyGridHeights.clear();
					s_vectEmptyGridHeights.resize(numVerticesPerSize * numVerticesPerSize);
					float altitude = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
					size_t idx = 0;
					for (int z = 0; z < numVerticesPerSize; z++)
						for (int x = 0; x < numVerticesPerSize; x++)
						{
							s_vectEmptyGridHeights[idx] = altitude;
							idx++;
						}
				}
			}

			heightmap = s_vectEmptyGridHeights;
			adjustHeights = false;
		}
		else
		{
			// generating heights using noise for other terrain types
			{
				//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer generateHeights 1.2 ") + __FUNCTION__, "Generate Heights");

				//FastNoiseLite noiseLite(terrainEdit->noise.noiseSeed);
				float absProduct = abs(lowerXGridVertex * lowerZGridVertex);
				int noiseSeed = abs(terrainEdit->noise.noiseSeed + int(absProduct));
				FastNoiseLite noiseLite(noiseSeed);
				noiseLite.SetNoiseType((enum FastNoiseLite::NoiseType)terrainEdit->noise.noiseType);
				//noiseLite.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
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

				size_t maxIndex = numVerticesPerSize - 1;

				size_t idx = 0;
				for (int z = 0; z < numVerticesPerSize; z++)
					for (int x = 0; x < numVerticesPerSize; x++)
					{
						float altitude = 0.0f;

						{
							//TheWorld_Utils::GuardProfiler profiler(std::string("EditGenerate 1.2.2 ") + __FUNCTION__, "GetNoise main");

							float scaleFactor_JagIntensity = terrainEdit->noise.scaleFactor_JagIntensity;
							
							//float xf = abs((lowerXGridVertex + (x * gridStepInWU)) * scaleFactor_JagIntensity);
							//float zf = abs((lowerZGridVertex + (z * gridStepInWU)) * scaleFactor_JagIntensity);
							float xf = abs(lowerXGridVertex + (x * gridStepInWU * scaleFactor_JagIntensity));
							float zf = abs(lowerZGridVertex + (z * gridStepInWU * scaleFactor_JagIntensity));
							altitude = noiseLite.GetNoise(xf, zf);
							
							// noises are value in range -1 to 1 we need to multiply by heightsAmplitude to have an altitude coherent with the terrain type.
							// multiplying by gridStepInWU / 2.0f is a way to scale the noise based on the grid resolution, ensuring that the noise effect is maintained varying the gridstep.
							altitude *= (float)(terrainEdit->noise.heightsAmplitude / 2) * (gridStepInWU / 2.0f);
						}

						heightmap[idx] = altitude;

						idx++;
					}
			}
		}

		//// DEBUG
		//{
		//	// POS -55, -58
		//	// vertices (0-2048):	heightmap[z * numVerticesPerSize + x]
		//	//		x=1210 z=1258 h=269.159576 ????
		//	//		x=1210 z=1259 h=270.523071 6296
		//	//		x=1210 z=1260 h=271.935760 6296
		//	//		x=1210 z=1261 h=273.366913 6296
		//	//		x=1210 z=1262 h=274.783417 6300
		//	if (pos.IS_QUADRANT_TRACKED_FOR_DEBUG)
		//	{
		//		struct debug_pos
		//		{
		//			size_t x;
		//			size_t z;
		//			float h;
		//			float h_16normalized;
		//			debug_pos(size_t x, size_t z, float h)
		//			{
		//				this->x = x;
		//				this->z = z;
		//				this->h = h;
		//				//uint16_t u16 = TheWorld_Utils::MeshCacheBuffer::halfFromFloat(h);
		//				//this->h_16normalized = TheWorld_Utils::MeshCacheBuffer::halfToFloat_1(u16);
		//				Eigen::half f16 = static_cast<Eigen::half>(h);
		//				this->h_16normalized = static_cast<float>(f16);
		//			}
		//		};
		//		std::vector<struct debug_pos> debug_positions;
		//		for (size_t z = 1258; z <= 1262; z++)
		//			for (size_t x = 1208; x <= 1212; x++)
		//			{
		//				debug_positions.push_back(struct debug_pos(x, z, heightmap[z * numVerticesPerSize + x]));
		//			}
		//	}
		//}
		//// DEBUG

		/*
		* smooth the heightmap to reduce noise and create a more natural terrain appearance. The number of blending cycles can be adjusted based on the desired level of smoothness.
		*/
		//for (size_t i = 0; i < terrainEdit->extraValues.numBlendingCycles; i++)
		//	SmoothHeightmap(pos, heightmap, 5, 1.0);

		//// DEBUG
		//{
		//	// POS -55, -58
		//	// vertices (0-2048):	heightmap[z * numVerticesPerSize + x]
		//	//		x=1210 z=1258 h=269.159576 ==> h=269.176086 ????
		//	//		x=1210 z=1259 h=270.523071 ==> h=270.560638 6296
		//	//		x=1210 z=1260 h=271.935760 ==> h=271.964020 6296
		//	//		x=1210 z=1261 h=273.366913 ==> h=273.385284 6296
		//	//		x=1210 z=1262 h=274.783417 ==> h=274.823730 6300
		//	if (pos.IS_QUADRANT_TRACKED_FOR_DEBUG)
		//	{
		//		struct debug_pos
		//		{
		//			size_t x;
		//			size_t z;
		//			float h;
		//			float h_16normalized;
		//			debug_pos(size_t x, size_t z, float h)
		//			{
		//				this->x = x;
		//				this->z = z;
		//				this->h = h;
		//				//uint16_t u16 = TheWorld_Utils::MeshCacheBuffer::halfFromFloat(h);
		//				//this->h_16normalized = TheWorld_Utils::MeshCacheBuffer::halfToFloat_1(u16);
		//				Eigen::half f16 = static_cast<Eigen::half>(h);
		//				this->h_16normalized = static_cast<float>(f16);
		//			}
		//		};
		//		std::vector<struct debug_pos> debug_positions;
		//		for (size_t z = 1258; z <= 1262; z++)
		//			for (size_t x = 1208; x <= 1212; x++)
		//			{
		//				debug_positions.push_back(struct debug_pos(x, z, heightmap[z * numVerticesPerSize + x]));
		//			}
		//	}
		//}
		//// DEBUG

		/*
		* Recalc min/max heights for the whole terrain and for each side of the quadrant, since we have generated new heights.
		*/
		{
			size_t maxIndex = numVerticesPerSize - 1;
			
			size_t idx = 0;
			for (int z = 0; z < numVerticesPerSize; z++)
				for (int x = 0; x < numVerticesPerSize; x++)
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

					float altitude = heightmap[idx];
					
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

					idx++;
				}
		}

		/*
		* adjust height according to desidered min height and post generate elevation, if needed
		*/
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
						heightmap[idx] += corrector;
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

				//// DEBUG
				//{
				//	// POS -55, -58
				//	// vertices (0-2048):	heightmap[z * numVerticesPerSize + x]
				//	//		x=1210 z=1258 h=269.159576 ==> h=269.176086 ==> h=6292.76758 (h16=6292.00000 ==> h16=6292.00000) ????
				//	//		x=1210 z=1259 h=270.523071 ==> h=270.560638 ==> h=6294.15186 (h16=6296.00000 ==> h16=6296.00000) 6296
				//	//		x=1210 z=1260 h=271.935760 ==> h=271.964020 ==> h=6295.55518 (h16=6296.00000 ==> h16=6296.00000) 6296
				//	//		x=1210 z=1261 h=273.366913 ==> h=273.385284 ==> h=6296.97656 (h16=6296.00000 ==> h16=6296.00000) 6296
				//	//		x=1210 z=1262 h=274.783417 ==> h=274.823730 ==> h=6298.41504 (h16=6296.00000 ==> h16=6300.00000) 6300
				//	if (pos.IS_QUADRANT_TRACKED_FOR_DEBUG)
				//	{
				//		struct debug_pos
				//		{
				//			size_t x;
				//			size_t z;
				//			float h;
				//			float h_16normalized;
				//			debug_pos(size_t x, size_t z, float h)
				//			{
				//				this->x = x;
				//				this->z = z;
				//				this->h = h;
				//				//uint16_t u16 = TheWorld_Utils::MeshCacheBuffer::halfFromFloat(h);
				//				//this->h_16normalized = TheWorld_Utils::MeshCacheBuffer::halfToFloat_1(u16);
				//				Eigen::half f16 = static_cast<Eigen::half>(h);
				//				this->h_16normalized = static_cast<float>(f16);
				//			}
				//		};
				//		std::vector<struct debug_pos> debug_positions;
				//		for (size_t z = 1258; z <= 1262; z++)
				//			for (size_t x = 1208; x <= 1212; x++)
				//			{
				//				debug_positions.push_back(struct debug_pos(x, z, heightmap[z * numVerticesPerSize + x]));
				//			}
				//	}
				//}
				//// DEBUG
			}
		}


		if (heightsFloat16InNewSavedQuadrant())
		{
			// simulating the conversion to float16 and back to float32, to ensure that the heights in the heightmap vector are stored as they will be saved int the buffer.
			for (int idx = 0; idx < numVerticesPerSize * numVerticesPerSize; idx++)
				heightmap[idx] = halfToFloat(halfFromFloat(heightmap[idx]));
		}
		else if (heightsUInt16InNewSavedQuadrant())
		{
			// simulating the conversion to uint16 heights and back to float32, to ensure that the heights in the heightmap vector are stored as they will be saved int the buffer.
			for (int idx = 0; idx < numVerticesPerSize * numVerticesPerSize; idx++)
				heightmap[idx] = uint16HeightToFloat(uint16HeightFromFloat(heightmap[idx]));
		}
	}
		
	//void MeshCacheBuffer::applyWorldModifier(int level, size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights, WorldModifier& wm)
	//{
	//	TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer applyWorldModifier 1 ") + __FUNCTION__, "ALL");

	//	if (level != wm.getPos().getLevel())
	//		return;

	//	float sizeInWU = (numVerticesPerSize - 1) * gridStepInWU;

	//	float AOE = wm.getAOE();
	//	float wmX = wm.getPos().getPosX();
	//	float wmZ = wm.getPos().getPosZ();
	//	float wmMin = wm.getMin();
	//	float wmMax = wm.getMax();
	//	enum class TheWorld_Utils::WMType wmType = wm.getPos().getType();
	//	enum class TheWorld_Utils::WMFunctionType wmFunctionType = wm.getFunctionType();
	//	enum class TheWorld_Utils::WMOrder wmOrder = wm.getOrder();
	//	float wmMaxExtension = wmMax - wmMin;
	//	float wmStrength = wmMax - wmMin;

	//	if (wmType == TheWorld_Utils::WMType::none)
	//		return;

	//	if (AOE > 0)
	//	{
	//		float d1 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex, lowerZGridVertex);
	//		float d2 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex + sizeInWU, lowerZGridVertex);
	//		float d3 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex, lowerZGridVertex + sizeInWU);
	//		float d4 = TheWorld_Utils::Utils::getDistance(wmX, wmZ, lowerXGridVertex + sizeInWU, lowerZGridVertex + sizeInWU);

	//		if (d1 > AOE && d2 > AOE && d3 > AOE && d4 > AOE)
	//			return;
	//	}

	//	terrainEdit->eastSideXPlus.minHeight = FLT_MAX;
	//	terrainEdit->eastSideXPlus.maxHeight = -FLT_MAX;

	//	terrainEdit->westSideXMinus.minHeight = FLT_MAX;
	//	terrainEdit->westSideXMinus.maxHeight = -FLT_MAX;

	//	terrainEdit->southSideZPlus.minHeight = FLT_MAX;
	//	terrainEdit->southSideZPlus.maxHeight = -FLT_MAX;

	//	terrainEdit->northSideZMinus.minHeight = FLT_MAX;
	//	terrainEdit->northSideZMinus.maxHeight = -FLT_MAX;

	//	size_t maxIndex = numVerticesPerSize - 1;

	//	size_t idx = -1;
	//	for (int z = 0; z < numVerticesPerSize; z++)
	//		for (int x = 0; x < numVerticesPerSize; x++)
	//		{
	//			idx++;

	//			if (x == maxIndex && z == 0)
	//			{
	//				assert(x == maxIndex && z == 0);
	//			}
	//			if (x == 0 && z == maxIndex)
	//			{
	//				assert(x == 0 && z == maxIndex);
	//			}
	//			if (x == maxIndex && z == maxIndex)
	//			{
	//				assert(x == maxIndex && z == maxIndex);
	//			}

	//			float dist = TheWorld_Utils::Utils::getDistance(wmX, wmZ, (float)x * gridStepInWU + lowerXGridVertex, (float)z * gridStepInWU + lowerZGridVertex);

	//			if (dist > AOE)
	//			{
	//				if (idx == 0)
	//				{
	//					terrainEdit->minHeight = terrainEdit->maxHeight = vectGridHeights[idx];
	//				}
	//				else
	//				{
	//					if (vectGridHeights[idx] < terrainEdit->minHeight)
	//						terrainEdit->minHeight = vectGridHeights[idx];
	//					if (vectGridHeights[idx] > terrainEdit->maxHeight)
	//						terrainEdit->maxHeight = vectGridHeights[idx];
	//				}
	//				
	//				if (x == 0)
	//				{
	//					if (vectGridHeights[idx] < terrainEdit->westSideXMinus.minHeight)
	//						terrainEdit->westSideXMinus.minHeight = vectGridHeights[idx];
	//					if (vectGridHeights[idx] > terrainEdit->westSideXMinus.maxHeight)
	//						terrainEdit->westSideXMinus.maxHeight = vectGridHeights[idx];
	//				}

	//				if (z == 0)
	//				{
	//					if (vectGridHeights[idx] < terrainEdit->northSideZMinus.minHeight)
	//						terrainEdit->northSideZMinus.minHeight = vectGridHeights[idx];
	//					if (vectGridHeights[idx] > terrainEdit->northSideZMinus.maxHeight)
	//						terrainEdit->northSideZMinus.maxHeight = vectGridHeights[idx];
	//				}

	//				if (x == maxIndex)
	//				{
	//					if (vectGridHeights[idx] < terrainEdit->eastSideXPlus.minHeight)
	//						terrainEdit->eastSideXPlus.minHeight = vectGridHeights[idx];
	//					if (vectGridHeights[idx] > terrainEdit->eastSideXPlus.maxHeight)
	//						terrainEdit->eastSideXPlus.maxHeight = vectGridHeights[idx];
	//				}

	//				if (z == maxIndex)
	//				{
	//					if (vectGridHeights[idx] < terrainEdit->southSideZPlus.minHeight)
	//						terrainEdit->southSideZPlus.minHeight = vectGridHeights[idx];
	//					if (vectGridHeights[idx] > terrainEdit->southSideZPlus.maxHeight)
	//						terrainEdit->southSideZPlus.maxHeight = vectGridHeights[idx];
	//				}

	//				continue;
	//			}

	//			switch (wmType)
	//			{
	//			case TheWorld_Utils::WMType::elevator:
	//			{
	//				// TheWorld_Utils::WMType::elevator
	//				switch (wmFunctionType)
	//				{
	//					// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax
	//				case TheWorld_Utils::WMFunctionType::ConsiderMinMax:
	//				{
	//					switch (wmOrder)
	//					{
	//						// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax - TheWorld_Utils::WMOrder::MaxEffectOnWM
	//					case TheWorld_Utils::WMOrder::MaxEffectOnWM:
	//					{
	//						float coeff = (AOE - dist) / AOE;	// 0 on perimeter, 1 on WM
	//						float addition = wmMaxExtension * coeff + wmMin;
	//						vectGridHeights[idx] += addition;
	//					}
	//					break;
	//					// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderMinMax - TheWorld_Utils::WMOrder::MinEffectOnWM
	//					case TheWorld_Utils::WMOrder::MinEffectOnWM:
	//					{
	//						float coeff = dist / AOE;			// 1 on perimeter, 0 on WM
	//						float addition = wmMaxExtension * coeff + wmMin;
	//						vectGridHeights[idx] += addition;
	//					}
	//					break;
	//					}
	//				}
	//				break;

	//				// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength
	//				case TheWorld_Utils::WMFunctionType::ConsiderStrength:
	//				{
	//					switch (wmOrder)
	//					{
	//						// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength - TheWorld_Utils::WMOrder::MaxEffectOnWM
	//					case TheWorld_Utils::WMOrder::MaxEffectOnWM:
	//					{
	//						float coeff = (AOE - dist) / AOE;	// 0 on perimeter, 1 on WM
	//						float addition = wmStrength * coeff;
	//						vectGridHeights[idx] += addition;
	//					}
	//					break;

	//					// TheWorld_Utils::WMType::elevator - TheWorld_Utils::WMFunctionType::ConsiderStrength - TheWorld_Utils::WMOrder::MinEffectOnWM
	//					case TheWorld_Utils::WMOrder::MinEffectOnWM:
	//					{
	//						float coeff = dist / AOE;			// 1 on perimeter, 0 on WM
	//						float addition = wmMaxExtension * coeff;
	//						vectGridHeights[idx] += addition;
	//					}
	//					break;
	//					}
	//				}
	//				break;
	//				}
	//			}
	//			break;
	//			}

	//			if (idx == 0)
	//			{
	//				terrainEdit->minHeight = terrainEdit->maxHeight = vectGridHeights[idx];
	//			}
	//			else
	//			{
	//				if (vectGridHeights[idx] < terrainEdit->minHeight)
	//					terrainEdit->minHeight = vectGridHeights[idx];
	//				if (vectGridHeights[idx] > terrainEdit->maxHeight)
	//					terrainEdit->maxHeight = vectGridHeights[idx];
	//			}

	//			if (x == 0)
	//			{
	//				if (vectGridHeights[idx] < terrainEdit->westSideXMinus.minHeight)
	//					terrainEdit->westSideXMinus.minHeight = vectGridHeights[idx];
	//				if (vectGridHeights[idx] > terrainEdit->westSideXMinus.maxHeight)
	//					terrainEdit->westSideXMinus.maxHeight = vectGridHeights[idx];
	//			}

	//			if (z == 0)
	//			{
	//				if (vectGridHeights[idx] < terrainEdit->northSideZMinus.minHeight)
	//					terrainEdit->northSideZMinus.minHeight = vectGridHeights[idx];
	//				if (vectGridHeights[idx] > terrainEdit->northSideZMinus.maxHeight)
	//					terrainEdit->northSideZMinus.maxHeight = vectGridHeights[idx];
	//			}

	//			if (x == maxIndex)
	//			{
	//				if (vectGridHeights[idx] < terrainEdit->eastSideXPlus.minHeight)
	//					terrainEdit->eastSideXPlus.minHeight = vectGridHeights[idx];
	//				if (vectGridHeights[idx] > terrainEdit->eastSideXPlus.maxHeight)
	//					terrainEdit->eastSideXPlus.maxHeight = vectGridHeights[idx];
	//			}

	//			if (z == maxIndex)
	//			{
	//				if (vectGridHeights[idx] < terrainEdit->southSideZPlus.minHeight)
	//					terrainEdit->southSideZPlus.minHeight = vectGridHeights[idx];
	//				if (vectGridHeights[idx] > terrainEdit->southSideZPlus.maxHeight)
	//					terrainEdit->southSideZPlus.maxHeight = vectGridHeights[idx];
	//			}
	//		}
	//}
		
	void MeshCacheBuffer::setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, std::string& buffer)
	{
		TheWorld_Utils::MemoryBuffer _buffer;
		setBufferFromCacheQuadrantData(numVerticesPerSize, cacheQuadrantData, _buffer);
		buffer.clear();
		buffer.reserve(_buffer.size());
		buffer.assign((char*)_buffer.ptr(), _buffer.size());
	}

	/*
	* Buffers are de-serialized in MeshCacheBuffer::refreshMapsFromBuffer
	*/
	void MeshCacheBuffer::setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer)
	{
		if (TheWorld_Utils::MeshCacheBuffer::newCacheQuadrantFormat1())
			setBufferFromCacheQuadrantData_Format1(numVerticesPerSize, cacheQuadrantData, buffer);
		else
			throw(GDN_TheWorld_Exception(__FUNCTION__, "Unsupported cache quadrant format"));
	}
	
	void MeshCacheBuffer::setBufferFromCacheQuadrantData_Format1(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1 ") + __FUNCTION__, "ALL");

		size_t size = 0;

		size_t numHeights = 0;
		size_t numHeights32 = cacheQuadrantData.heights32Buffer != nullptr ? cacheQuadrantData.heights32Buffer->size() / sizeof(float) : 0;
		my_assert(cacheQuadrantData.heights32Buffer == nullptr || cacheQuadrantData.heights32Buffer->size() == numHeights32 * sizeof(float));
		size_t numNormals = cacheQuadrantData.normalsBuffer != nullptr ? cacheQuadrantData.normalsBuffer->size() / sizeof(TheWorld_Utils::_RGB) : 0;
		my_assert(cacheQuadrantData.normalsBuffer == nullptr || cacheQuadrantData.normalsBuffer->size() == numNormals * sizeof(TheWorld_Utils::_RGB));
		size_t numSplatmapVertices = cacheQuadrantData.splatmapBuffer != nullptr ? cacheQuadrantData.splatmapBuffer->size() / sizeof(TheWorld_Utils::_RGBA) : 0;
		my_assert(cacheQuadrantData.splatmapBuffer == nullptr || cacheQuadrantData.splatmapBuffer->size() == numSplatmapVertices * sizeof(TheWorld_Utils::_RGBA));
		size_t numColormapVertices = cacheQuadrantData.colormapBuffer != nullptr ? cacheQuadrantData.colormapBuffer->size() / sizeof(TheWorld_Utils::_RGBA) : 0;
		my_assert(cacheQuadrantData.colormapBuffer == nullptr || cacheQuadrantData.colormapBuffer->size() == numColormapVertices * sizeof(TheWorld_Utils::_RGBA));
		//size_t numGlobalmapVertices = cacheQuadrantData.globalmapBuffer != nullptr ? cacheQuadrantData.globalmapBuffer->size() / sizeof(TheWorld_Utils::_RGB) : 0;
		//my_assert(cacheQuadrantData.globalmapBuffer == nullptr || cacheQuadrantData.globalmapBuffer->size() == numGlobalmapVertices * sizeof(TheWorld_Utils::_RGB));

		my_assert(numHeights32 == 0 || numHeights32 == numVerticesPerSize * numVerticesPerSize);
		my_assert(numNormals == 0 || numNormals == numVerticesPerSize * numVerticesPerSize);
		my_assert(numSplatmapVertices == 0 || numSplatmapVertices == numVerticesPerSize * numVerticesPerSize || numSplatmapVertices == numVerticesPerSize * 4);
		my_assert(numColormapVertices == 0 || numColormapVertices == numVerticesPerSize * numVerticesPerSize);
		//my_assert(numGlobalmapVertices == 0 || numGlobalmapVertices == numVerticesPerSize * numVerticesPerSize);

		static std::recursive_mutex s_mtxEmptyBuffer;
		static TheWorld_Utils::MemoryBuffer s_emptyUInt16HeightsBuffer;
		static TheWorld_Utils::MemoryBuffer s_emptyFloat16HeightsBuffer;
		static TheWorld_Utils::MemoryBuffer s_emptyFloat32HeightsBuffer;
		TheWorld_Utils::MemoryBuffer height16HeightsBuffer;
		TheWorld_Utils::MemoryBuffer* _height16HeightsBuffer = nullptr;
		TheWorld_Utils::MemoryBuffer* _float32HeightsBuffer = cacheQuadrantData.heights32Buffer;
		TheWorld_Utils::MemoryBuffer* _normalsBuffer = cacheQuadrantData.normalsBuffer;
		TheWorld_Utils::MemoryBuffer* _splatmapBuffer = cacheQuadrantData.splatmapBuffer;

		size_t height16HeightmapSize = 0;
		size_t float32HeightmapSize = 0;

		if (numHeights32 == 0)
		{
			// generating empty heights buffer for quadrants that don't have any height data (for example, empty quadrants)
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.1 ") + __FUNCTION__, "gen empty quadrants heights");

			numHeights = numVerticesPerSize * numVerticesPerSize;
			
			if (heightsFloat16InNewSavedQuadrant())
			{
				// generating empty float16 heights buffer for quadrants that don't have any height data (for example, empty quadrants)
				// Must be set _height16HeightsBuffer and height16HeightmapSize

				_height16HeightsBuffer = &s_emptyFloat16HeightsBuffer;

				{
					std::lock_guard<std::recursive_mutex> lock(s_mtxEmptyBuffer);

					size_t float16HeightsBufferSize = (numHeights + 1) * sizeof(uint16_t);		// in height16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)

					if (s_emptyFloat16HeightsBuffer.size() == 0)
					{
						s_emptyFloat16HeightsBuffer.resize(float16HeightsBufferSize);
						uint16_t* float16HeightsBufferPtr = (uint16_t*)s_emptyFloat16HeightsBuffer.ptr();

						*float16HeightsBufferPtr = uint16_t(HeightsFormatInQuadrantBuffer::Float16);		// in height16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
						float16HeightsBufferPtr++;

						uint16_t emptyHalf = TheWorld_Utils::MeshCacheBuffer::halfFromFloat(EMPTY_QUADRANTS_TERRAIN_HEIGHT);
						for (size_t z = 0; z < numVerticesPerSize; z++)
							for (size_t x = 0; x < numVerticesPerSize; x++)
							{
								*float16HeightsBufferPtr = emptyHalf;
								float16HeightsBufferPtr++;
							}

						assert((BYTE*)float16HeightsBufferPtr - s_emptyFloat16HeightsBuffer.ptr() == float16HeightsBufferSize);
					}
					else
					{
						assert(float16HeightsBufferSize == _height16HeightsBuffer->size());
					}
				}

				height16HeightmapSize = _height16HeightsBuffer->size();
			}
			else if (heightsUInt16InNewSavedQuadrant())
			{
				// generating empty uint16 heights buffer for quadrants that don't have any height data (for example, empty quadrants)
				// Must be set _height16HeightsBuffer and height16HeightmapSize
				
				_height16HeightsBuffer = &s_emptyUInt16HeightsBuffer;

				{
					std::lock_guard<std::recursive_mutex> lock(s_mtxEmptyBuffer);

					size_t uint16HeightsBufferSize = (numHeights + 1) * sizeof(uint16_t);			// in height16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)

					if (s_emptyUInt16HeightsBuffer.size() == 0)
					{
						s_emptyUInt16HeightsBuffer.resize(uint16HeightsBufferSize);
						uint16_t* uint16HeightsBufferPtr = (uint16_t*)s_emptyUInt16HeightsBuffer.ptr();

						*uint16HeightsBufferPtr = uint16_t(HeightsFormatInQuadrantBuffer::UInt16);		// in height16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
						uint16HeightsBufferPtr++;

						uint16_t emptyUInt16Height = TheWorld_Utils::MeshCacheBuffer::uint16HeightFromFloat(EMPTY_QUADRANTS_TERRAIN_HEIGHT);
						for (size_t z = 0; z < numVerticesPerSize; z++)
							for (size_t x = 0; x < numVerticesPerSize; x++)
							{
								*uint16HeightsBufferPtr = emptyUInt16Height;
								uint16HeightsBufferPtr++;
							}

						assert((BYTE*)uint16HeightsBufferPtr - s_emptyUInt16HeightsBuffer.ptr() == uint16HeightsBufferSize);
					}
					else
					{
						assert(uint16HeightsBufferSize == _height16HeightsBuffer->size());
					}
				}

				height16HeightmapSize = _height16HeightsBuffer->size();
			}
			else if (heightsFloat32InNewSavedQuadrant())
			{
				// generating empty float32 heights buffer for quadrants that don't have any height data (for example, empty quadrants)
				// Must be set _float32HeightsBuffer and float32HeightmapSize
				
				_float32HeightsBuffer = &s_emptyFloat32HeightsBuffer;

				{
					std::lock_guard<std::recursive_mutex> lock(s_mtxEmptyBuffer);

					size_t float32HeightsBufferSize = numHeights * sizeof(float);

					if (s_emptyFloat32HeightsBuffer.size() == 0)
					{
						s_emptyFloat32HeightsBuffer.resize(float32HeightsBufferSize);
						memset(s_emptyFloat32HeightsBuffer.ptr(), 0, float32HeightsBufferSize);		// initializing all float to zero (positive-zero float has all bit to zero)
					}
					else
					{
						assert(float32HeightsBufferSize == _float32HeightsBuffer->size());
					}
				}

				float32HeightmapSize = _float32HeightsBuffer->size();
			}
			else
			{
				my_assert(false);
			}
		}
		else
		{
			my_assert(numHeights32 == numVerticesPerSize * numVerticesPerSize);

			numHeights = numHeights32;

			if (heightsFloat16InNewSavedQuadrant())
			{
				// generating float16 heights buffer from float 32 heights for quadrants that have height data (for example, non-empty quadrants)
				// Must be set _height16HeightsBuffer and height16HeightmapSize

				_height16HeightsBuffer = &height16HeightsBuffer;

				size_t float16HeightsBufferSize = (numHeights + 1) * sizeof(uint16_t);			// in float16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
				_height16HeightsBuffer->reserve(float16HeightsBufferSize);
				_height16HeightsBuffer->adjustSize(float16HeightsBufferSize);
				uint16_t* float16HeightsBufferPtr = _height16HeightsBuffer->ptr<uint16_t>();
				
				*float16HeightsBufferPtr = uint16_t(HeightsFormatInQuadrantBuffer::Float16);		// in float16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
				float16HeightsBufferPtr++;
				
				float* float32HeightsBufferPtr = _float32HeightsBuffer->ptr<float>();
				BYTE* endOf32HeightsBufferPtr = (BYTE*)float32HeightsBufferPtr + _float32HeightsBuffer->size();
				while (float32HeightsBufferPtr < (float*)endOf32HeightsBufferPtr)
				{
					uint16_t f16 = TheWorld_Utils::MeshCacheBuffer::halfFromFloat(*float32HeightsBufferPtr);
					*float16HeightsBufferPtr = f16;
					float32HeightsBufferPtr++;
					float16HeightsBufferPtr++;
				}
				my_assert((BYTE*)float16HeightsBufferPtr - _height16HeightsBuffer->ptr() == float16HeightsBufferSize);

				height16HeightmapSize = _height16HeightsBuffer->size();
			}
			else if (heightsUInt16InNewSavedQuadrant())
			{
				// generating uint16 heights buffer from float 32 heights for quadrants that have height data (for example, non-empty quadrants)
				// Must be set _height16HeightsBuffer and height16HeightmapSize

				_height16HeightsBuffer = &height16HeightsBuffer;

				size_t height16HeightsBufferSize = (numHeights + 1) * sizeof(uint16_t);			// in uint16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
				_height16HeightsBuffer->reserve(height16HeightsBufferSize);
				_height16HeightsBuffer->adjustSize(height16HeightsBufferSize);
				uint16_t* height16HeightsBufferPtr = _height16HeightsBuffer->ptr<uint16_t>();

				*height16HeightsBufferPtr = uint16_t(HeightsFormatInQuadrantBuffer::UInt16);		// in uint16 buffers first byte is the heights format in the buffer (0=uint16, 1=float16)
				height16HeightsBufferPtr++;

				float* float32HeightsBufferPtr = _float32HeightsBuffer->ptr<float>();
				BYTE* endOf32HeightsBufferPtr = (BYTE*)float32HeightsBufferPtr + _float32HeightsBuffer->size();
				while (float32HeightsBufferPtr < (float*)endOf32HeightsBufferPtr)
				{
					uint16_t uint16Height = TheWorld_Utils::MeshCacheBuffer::uint16HeightFromFloat(*float32HeightsBufferPtr);
					*height16HeightsBufferPtr = uint16Height;
					float32HeightsBufferPtr++;
					height16HeightsBufferPtr++;
				}
				my_assert((BYTE*)height16HeightsBufferPtr - _height16HeightsBuffer->ptr() == height16HeightsBufferSize);
				height16HeightmapSize = _height16HeightsBuffer->size();
			}
			else if (heightsFloat32InNewSavedQuadrant())
			{
				// Must be set float32HeightmapSize (_float32HeightsBuffer is already set with the buffer of the input quadrant)

				float32HeightmapSize = _float32HeightsBuffer->size();
			}
			else
			{
				my_assert(false);
			}
		}

		size_t normalmapSize = numNormals > 0 ? numNormals * sizeof(TheWorld_Utils::_RGB) : 0;
		size_t splatmapSize = numSplatmapVertices > 0 ? numSplatmapVertices * sizeof(TheWorld_Utils::_RGBA) : 0;
		size_t colormapSize = numColormapVertices > 0 ? numColormapVertices * sizeof(TheWorld_Utils::_RGBA) : 0;
		size_t globalmapSize = cacheQuadrantData.globalmapBuffer == nullptr ? 0 : cacheQuadrantData.globalmapBuffer->size();
		size_t namedDataBufferSerializedSize = cacheQuadrantData.namedDataBuffer == nullptr ? 0 : cacheQuadrantData.namedDataBuffer->serializedSize();

		// Forcing not to save 32 heights and normals in the buffer to save memory, cache disk space and bandwidth (they must be generated on the fly by heights 16 and normals directly by the shader)
		normalmapSize = 0;			// for now we don't want to use normals in cache quadrant buffers, we will generate them on the fly when needed (we want to save memory and bandwidth)
		colormapSize = 0;			// colormaps are not used atm

		size_t terrainEditValueSize = cacheQuadrantData.terrainEditValues == nullptr ? 0 : cacheQuadrantData.terrainEditValues->size();
		size_t streamBufferSize = 1	/* buffer format "0"/"1" */
			+ sizeof(size_t) + cacheQuadrantData.meshId.length()
			+ sizeof(size_t) + terrainEditValueSize
			+ sizeof(size_t)		/* numheights */
			+ sizeof(float)			/*min_altitude*/
			+ sizeof(float)			/*max_altitude*/
			+ sizeof(size_t) + height16HeightmapSize
			+ sizeof(size_t) + float32HeightmapSize
			+ sizeof(size_t) + normalmapSize
			+ sizeof(size_t) + splatmapSize
			+ sizeof(size_t) + colormapSize
			+ sizeof(size_t) + globalmapSize
			+ namedDataBufferSerializedSize;

		buffer.reserve(streamBufferSize);
		BYTE* _streamBuffer = buffer.ptr();

		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.2 ") + __FUNCTION__, "copy meshId/edit values");

			memcpy(_streamBuffer, "1", 1);
			_streamBuffer++;

			TheWorld_Utils::serializeToByteStream<size_t>(cacheQuadrantData.meshId.length(), _streamBuffer, size);
			_streamBuffer += size;

			memcpy(_streamBuffer, cacheQuadrantData.meshId.c_str(), cacheQuadrantData.meshId.length());
			_streamBuffer += cacheQuadrantData.meshId.length();

			TheWorld_Utils::serializeToByteStream<size_t>(terrainEditValueSize, _streamBuffer, size);
			_streamBuffer += size;

			if (terrainEditValueSize > 0)
			{
				TerrainEdit terrainData;
				terrainData.deserialize(*cacheQuadrantData.terrainEditValues);
				memcpy(_streamBuffer, &terrainData, terrainEditValueSize);
				_streamBuffer += terrainEditValueSize;
				cacheQuadrantData.minHeight = terrainData.minHeight;
				cacheQuadrantData.maxHeight = terrainData.maxHeight;
			}
		}

		TheWorld_Utils::serializeToByteStream<size_t>(numHeights, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<float>(cacheQuadrantData.minHeight, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<float>(cacheQuadrantData.maxHeight, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(height16HeightmapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (height16HeightmapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.3 ") + __FUNCTION__, "copy heights 16");

			memcpy(_streamBuffer, _height16HeightsBuffer->ptr(), height16HeightmapSize);
			_streamBuffer += height16HeightmapSize;
		}

		TheWorld_Utils::serializeToByteStream<size_t>(float32HeightmapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (float32HeightmapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.3 ") + __FUNCTION__, "copy heights 32");

			memcpy(_streamBuffer, _float32HeightsBuffer->ptr(), float32HeightmapSize);
			_streamBuffer += float32HeightmapSize;
		}
			
		TheWorld_Utils::serializeToByteStream<size_t>(normalmapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (normalmapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.4 ") + __FUNCTION__, "copy normals");

			memcpy(_streamBuffer, _normalsBuffer->ptr(), normalmapSize);
			_streamBuffer += normalmapSize;
		}

		TheWorld_Utils::serializeToByteStream<size_t>(splatmapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (splatmapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.4 ") + __FUNCTION__, "copy splatmap");

			memcpy(_streamBuffer, _splatmapBuffer->ptr(), splatmapSize);
			_streamBuffer += splatmapSize;
		}

		TheWorld_Utils::serializeToByteStream<size_t>(colormapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (colormapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.4 ") + __FUNCTION__, "copy colormap");

			memcpy(_streamBuffer, cacheQuadrantData.colormapBuffer->ptr(), colormapSize);
			_streamBuffer += colormapSize;
		}

		TheWorld_Utils::serializeToByteStream<size_t>(globalmapSize, _streamBuffer, size);
		_streamBuffer += size;

		if (globalmapSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.4 ") + __FUNCTION__, "copy globalmap");

			memcpy(_streamBuffer, cacheQuadrantData.globalmapBuffer->ptr(), globalmapSize);
			_streamBuffer += globalmapSize;
		}

		if (namedDataBufferSerializedSize > 0)
		{
			TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromCacheQuadrantData_Format1 1.5 ") + __FUNCTION__, "serialize named data buffer");

			cacheQuadrantData.namedDataBuffer->serialize(_streamBuffer, size);
			_streamBuffer += size;
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
		terrainEdit.heightsNeedRegen(true);
		//terrainEdit.normalsNeedRegen(true);
		//terrainEdit.splatmapNeedRegen(true);
		terrainEdit.globalmapNeedRegen(true);
		//terrainEdit.colormapNeedRegen(true);
		TheWorld_Utils::MemoryBuffer terrainEditValuesBuffer((BYTE*)&terrainEdit, terrainEdit.size);
		cacheQuadrantData.minHeight = 0.0f;
		cacheQuadrantData.maxHeight = 0.0f;
		cacheQuadrantData.terrainEditValues = &terrainEditValuesBuffer;
		//TheWorld_Utils::MemoryBuffer emptyFloat16HeightsBuffer;
		TheWorld_Utils::MemoryBuffer emptyFloat32HeightsBuffer;
		TheWorld_Utils::MemoryBuffer emptyNormalBuffer;
		TheWorld_Utils::MemoryBuffer emptySplatmapBuffer;
		TheWorld_Utils::MemoryBuffer emptyColormapBuffer;
		TheWorld_Utils::MemoryBuffer emptyGlobalmapBuffer;
		//cacheQuadrantData.heights16Buffer = &emptyFloat16HeightsBuffer;
		cacheQuadrantData.heights32Buffer = &emptyFloat32HeightsBuffer;
		cacheQuadrantData.normalsBuffer = &emptyNormalBuffer;
		cacheQuadrantData.splatmapBuffer = &emptySplatmapBuffer;
		cacheQuadrantData.colormapBuffer = &emptyColormapBuffer;
		cacheQuadrantData.globalmapBuffer = &emptyGlobalmapBuffer;

		setBufferFromCacheQuadrantData(numVerticesPerSize, cacheQuadrantData, buffer);
	}

	struct TheWorld_Utils::_RGBA MeshCacheBuffer::calcSplat(TheWorld_Utils::MemoryBuffer& heightmap, size_t index, size_t numVerticesPerSize, float gridStepInWU, TerrainEdit* terrainEdit)
	{
		float normalX, normalY, normalZ, h;
		TheWorld_Utils::MeshCacheBuffer::calcNormal(heightmap, index, numVerticesPerSize, gridStepInWU, normalX, normalY, normalZ, h);
		Eigen::Vector3d normal = Eigen::Vector3d(double(normalX), double(normalY), double(normalZ));

		Eigen::Vector3d up(0.0f, 1.0f, 0.0f);

		// component on up direction of the normal (it is the y component of the normal if up is classic up == 0, 1, 0) - It ranges from 0=vertical terrain to 1=flat terrain
		float upComponent = float(normal.dot(up));

		// (1.0f - upComponent) ranges from 0=flat terrain to 1=vertical terrain so slope ranges from 0-slopeFlatFactor=flat terrain to slopeVerticalFactor-slopeFlatFactor=vertical terrain
		float slope = (terrainEdit->extraValues.slopeVerticalFactor * (1.0f - upComponent)) - terrainEdit->extraValues.slopeFlatFactor;

		// amounts range from 0.0f to 1.0f
		float rocksAmount = TheWorld_Utils::Utils::clamp<float>(slope, 0.0f, 1.0f);								// rocksAmount is max when slope is greater than 1 (vertical enough) and is null when slope is less than 0 (flat enough), otherwise it grows with the slope (if slope between 0 and 1)
		float dirtAmount = TheWorld_Utils::Utils::clamp<float>(rocksAmount * terrainEdit->extraValues.dirtOnRocksFactor, 0.0f, 1.0f);	// dirt grows on rocks
		//float highElevationAmount = terrainEdit->extraValues.highElevationFactor * ((h - terrainEdit->minHeight) / (terrainEdit->maxHeight - terrainEdit->minHeight)) - terrainEdit->extraValues.lowElevationFactor;		// ranges from -lowElevationFactor to highElevationFactor-lowElevationFactor
		float highElevationAmount = 
			TheWorld_Utils::TerrainEdit::THEWORLD_SPLATMAP_HIGH_ELEVATION_FACTOR * ((h - TheWorld_Utils::TerrainEdit::THEWORLD_SPLATMAP_MIN_HEIGHT) / (TheWorld_Utils::TerrainEdit::THEWORLD_SPLATMAP_MAX_HEIGHT - TheWorld_Utils::TerrainEdit::THEWORLD_SPLATMAP_MIN_HEIGHT))
			- TheWorld_Utils::TerrainEdit::THEWORLD_SPLATMAP_LOW_ELEVATION_FACTOR;		// ranges from -lowElevationFactor to highElevationFactor-lowElevationFactor
		highElevationAmount = TheWorld_Utils::Utils::clamp<float>(highElevationAmount, 0.0f, 1.0f);				// highElevationAmount is capped from 0 and 1
		float lowElevationAmount = 1.0f - highElevationAmount;													// lowElevationAmount is the complement of highElevationAmount in range 0/1

		//if (rocksAmount == 0.0f)
		//	rocksAmount = 0.0f;

		struct TheWorld_Utils::_RGBA rgba;

		float r, g, b, a;
		if (terrainEdit->extraValues.splatMapMode == 1)
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

		rgba.r = BYTE(r * 255);
		rgba.g = BYTE(g * 255);
		rgba.b = BYTE(b * 255);
		rgba.a = BYTE(a * 255);

		return rgba;
	}

	void MeshCacheBuffer::calcNormal(TheWorld_Utils::MemoryBuffer& heightmap, size_t index, size_t numVerticesPerSize, float gridStepInWU, float& normalX, float& normalY, float& normalZ, float& h)
	{
		size_t posX = TheWorld_Utils::MemoryBuffer::getPosXFromIndex(index, numVerticesPerSize);		// 0-based pos X in heightmap
		size_t posZ = TheWorld_Utils::MemoryBuffer::getPosYFromIndex(index, numVerticesPerSize);		// 0-based pos Z in heightmap

		h = heightmap.get_at<float>(index);

		Eigen::Vector3d normal;

		if (posX < numVerticesPerSize - 1 and posZ < numVerticesPerSize - 1)
		{
			// we are far from right and bottom border we can fetch left and bottom heights
			float h_right = heightmap.get_at<float>(index + 1);
			float h_bottom = heightmap.get_at<float>(index + numVerticesPerSize);
			normal = Eigen::Vector3d(h - h_right, gridStepInWU, h - h_bottom);
		}
		else if (posX < numVerticesPerSize - 1 and posZ == numVerticesPerSize - 1)
		{
			// we are on the bottom border but not on the the right border
			float h_right = heightmap.get_at<float>(index + 1);
			float h_top = heightmap.get_at<float>(index - numVerticesPerSize);
			normal = Eigen::Vector3d(h - h_top, gridStepInWU, h - h_right);
		}
		else if (posX == numVerticesPerSize - 1 and posZ < numVerticesPerSize - 1)
		{
			// we are on the right border but not on the bottom border
			float h_left = heightmap.get_at<float>(index - 1);
			float h_bottom = heightmap.get_at<float>(index + numVerticesPerSize);
			normal = Eigen::Vector3d(h - h_bottom, gridStepInWU, h - h_left);
		}
		else
		{
			// we are on the right / bottom vertex
			float h_left = heightmap.get_at<float>(index - 1);
			float h_top = heightmap.get_at<float>(index - numVerticesPerSize);
			normal = Eigen::Vector3d(h - h_left, gridStepInWU, h - h_top);
		}

		normal.normalize();
		normalX = (float)normal.x();
		normalY = (float)normal.y();
		normalZ = (float)normal.z();
	}
	
	uint16_t MeshCacheBuffer::halfFromFloat(float _f)
	{
		//TheWorld_Utils::FLOAT_32 f(_f);
		//return halfFromFloat(f.u32);
		Eigen::half h16 = static_cast<Eigen::half>(_f);
		uint16_t raw_bits = Eigen::numext::bit_cast<uint16_t>(h16);
		return raw_bits;
		//this->h_16normalized = static_cast<float>(f16);
	}
		
	//uint16_t MeshCacheBuffer::halfFromFloat(uint32_t _f)
	//{
	//	// https://github.com/macton/ps3-archive.git thanks to Mike Acton
	//	return half_from_float(_f);
	//}
	
	//uint32_t MeshCacheBuffer::halfToFloat(uint16_t raw_bits)
	//{
	//	//// https://github.com/macton/ps3-archive.git thanks to Mike Acton
	//	//return half_to_float(_f);
	//	Eigen::half h16 = Eigen::numext::bit_cast<Eigen::half>(raw_bits);
	//	float f = static_cast<float>(h16);
	//	return f;
	//}
	
	float MeshCacheBuffer::halfToFloat(uint16_t raw_bits)
	{
		//// https://github.com/macton/ps3-archive.git thanks to Mike Acton
		//TheWorld_Utils::FLOAT_32 f;
		//f.u32 = (half_to_float(_f));
		//return f.f32;
		Eigen::half h16 = Eigen::numext::bit_cast<Eigen::half>(raw_bits);
		float f = static_cast<float>(h16);
		return f;
	}
	
	uint16_t MeshCacheBuffer::uint16HeightFromFloat(float h)
	{
		int16_t rounded = int16_t(std::round(h * 3.0f));
		uint16_t bias = 32767;
		return std::clamp<uint16_t>(rounded + bias, MeshCacheBuffer::UINT16_HEIGHT_MIN, MeshCacheBuffer::UINT16_HEIGHT_MAX);
	}

	float MeshCacheBuffer::uint16HeightToFloat(uint16_t h)
	{
		uint16_t bias = 32767;
		return (float(h) - float(bias)) / 3.0f;
	}

	float MeshCacheBuffer::uint16HeightMinFloat(void)
	{
		return MeshCacheBuffer::uint16HeightToFloat(MeshCacheBuffer::UINT16_HEIGHT_MIN);		// -10922.3330
	}
	float MeshCacheBuffer::uint16HeightMaxFloat(void)
	{
		return MeshCacheBuffer::uint16HeightToFloat(MeshCacheBuffer::UINT16_HEIGHT_MAX);		// 10922.6670
	}

	void MeshCacheBuffer::setDummyBuffer(std::string meshId, std::string& buffer, TheWorld_Utils::TerrainEdit* terrainEdit)
	{
		if (TheWorld_Utils::MeshCacheBuffer::newCacheQuadrantFormat1())
			setDummyBuffer_Format1(meshId, buffer, terrainEdit);
		else
			throw(GDN_TheWorld_Exception(__FUNCTION__, "Unsupported cache quadrant format"));
		//else
		//	setDummyBuffer_Format0(meshId, buffer, terrainEdit);
	}

	void MeshCacheBuffer::setDummyBuffer_Format1(std::string meshId, std::string& buffer, TheWorld_Utils::TerrainEdit* terrainEdit)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setDummyBuffer_Format1 1 ") + __FUNCTION__, "ALL");

		size_t size = 0;

		size_t vectSize = 0;
		size_t terrainEditValuesBufferSize = 0, float16HeightmapSize = 0, float32HeightmapSize = 0, normalmapSize = 0, splatmapSize = 0, colormapSize = 0, globalmapSize = 0;
		if (terrainEdit != nullptr)
			terrainEditValuesBufferSize = terrainEdit->size;
		float min_altitude = 0.0f, max_altitude = 0.0f;
		size_t streamBufferSize = 1 /* buffer format "0"/"1" */
			+ sizeof(size_t) + meshId.length()
			+ sizeof(size_t) + terrainEditValuesBufferSize	// terrain edit values buffer
			+ sizeof(size_t)	// vect size = 0
			+ sizeof(float)		// min altitude
			+ sizeof(float)		// max altitude
			+ sizeof(size_t)	// float16 heightmap size = 0
			+ sizeof(size_t)	// float32 heightmap size = 0
			+ sizeof(size_t)	// normalmap size = 0
			+ sizeof(size_t)	// splatmap size = 0
			+ sizeof(size_t)	// colormap size = 0
			+ sizeof(size_t);	// globalmap size = 0
		BYTE* startOfStreamBuffer = nullptr;
		BYTE* _streamBuffer = nullptr;

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.1 ") + __FUNCTION__, "calloc");

			_streamBuffer = startOfStreamBuffer = (BYTE*)calloc(1, streamBufferSize);
			if (startOfStreamBuffer == nullptr)
				throw(std::exception((std::string(__FUNCTION__) + std::string(" Allocation error")).c_str()));
		}

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.2 ") + __FUNCTION__, "meshId/edit values");

			memcpy(_streamBuffer, "1", 1);
			_streamBuffer++;

			TheWorld_Utils::serializeToByteStream<size_t>(meshId.length(), _streamBuffer, size);
			_streamBuffer += size;

			memcpy(_streamBuffer, meshId.c_str(), meshId.length());
			_streamBuffer += meshId.length();

			TheWorld_Utils::serializeToByteStream<size_t>(terrainEditValuesBufferSize, _streamBuffer, size);
			_streamBuffer += size;

			if (terrainEditValuesBufferSize > 0)
			{
				memcpy(_streamBuffer, terrainEdit, terrainEditValuesBufferSize);
				_streamBuffer += terrainEditValuesBufferSize;
			}
		}

		TheWorld_Utils::serializeToByteStream<size_t>(vectSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<float>(min_altitude, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<float>(max_altitude, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(float16HeightmapSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(float32HeightmapSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(normalmapSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(splatmapSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(colormapSize, _streamBuffer, size);
		_streamBuffer += size;

		TheWorld_Utils::serializeToByteStream<size_t>(globalmapSize, _streamBuffer, size);
		_streamBuffer += size;

		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.4 ") + __FUNCTION__, "copy out buffer");
			buffer = std::string((char*)startOfStreamBuffer, streamBufferSize);
		}
		
		{
			//TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer setBufferFromHeights 1.5 ") + __FUNCTION__, "free");
			free(startOfStreamBuffer);
		}

		m_meshId = meshId;
	}

	//bool MeshCacheBuffer::debugCheckMinMaxHeight(TheWorld_Utils::QuadrantPos& pos, TheWorld_Utils::TerrainEdit* terrainEdit,
	//	//float& minHeight, float& maxHeight, float& northSideMin, float& northSideMax, float& southSideMin, float& southSideMax, float& westSideMin, float& westSideMax, float& eastSideMin, float& eastSideMax, 
	//	std::string msgHeader, std::string* meshId, TheWorld_Utils::MemoryBuffer* heights16Buffer, TheWorld_Utils::MemoryBuffer* heights32Buffer)
	//{
	//	std::string _msg;

	//	std::string _meshId = meshId == nullptr ? "" : *meshId;
	//	
	//	//if (not pos.isQuadrantTrackedForDebug())
	//	//	return true;
	//	
	//	bool match = true;

	//	size_t numVerticesPerSize = (size_t)pos.getNumVerticesPerSize();

	//	if (match)
	//	{
	//		if (heights32Buffer == nullptr and heights16Buffer == nullptr)
	//		{
	//			_msg += "checkMinMaxHeight: both heights32Buffer and heights16Buffer are null";
	//			match = false;
	//		}
	//	}

	//	if (match)
	//	{
	//		if (heights32Buffer != nullptr)
	//		{
	//			if (match)
	//			{
	//				if (heights32Buffer->size() == 0)
	//				{
	//					_msg += "checkMinMaxHeight: heights32Buffer is empty";
	//					match = false;
	//				}
	//			}
	//			if (match)
	//			{
	//				if (heights32Buffer->size() != (numVerticesPerSize * numVerticesPerSize * sizeof(float)))
	//				{
	//					_msg += "checkMinMaxHeight: heights32Buffer has incorrect size";
	//					match = false;
	//				}
	//			}
	//		}
	//	}

	//	if (match)
	//	{
	//		if (heights16Buffer != nullptr)
	//		{
	//			if (match)
	//			{
	//				if (heights16Buffer->size() == 0)
	//				{
	//					_msg += "checkMinMaxHeight: heights16Buffer is empty";
	//					match = false;
	//				}
	//			}
	//			if (match)
	//			{
	//				if (heights16Buffer->size() != (numVerticesPerSize * numVerticesPerSize * sizeof(uint16_t)))
	//				{
	//					_msg += "checkMinMaxHeight: heights16Buffer has incorrect size";
	//					match = false;
	//				}
	//			}
	//		}
	//	}

	//	if (match)
	//	{
	//		size_t maxIndex = numVerticesPerSize - 1;

	//		float minHeight = FLT_MAX, maxHeight = -FLT_MAX;
	//		float northSideMin = FLT_MAX, northSideMax = -FLT_MAX;
	//		float southSideMin = FLT_MAX, southSideMax = -FLT_MAX;
	//		float westSideMin = FLT_MAX, westSideMax = -FLT_MAX;
	//		float eastSideMin = FLT_MAX, eastSideMax = -FLT_MAX;

	//		for (size_t z = 0; z < numVerticesPerSize; z++)
	//		{
	//			for (size_t x = 0; x < numVerticesPerSize; x++)
	//			{
	//				float h32 = FLT_MAX, h16 = FLT_MAX, h = FLT_MAX;
	//				if (heights32Buffer != nullptr)
	//					h32 = heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
	//				if (heights16Buffer != nullptr)
	//					h16 = TheWorld_Utils::MeshCacheBuffer::halfToFloat_1(heights16Buffer->_deprecated_get_at_ref<uint16_t>(x, z, numVerticesPerSize));
	//				if (h32 != FLT_MAX and h16 != FLT_MAX)
	//				{
	//					if (h32 != h16)
	//					{
	//						_msg += "h32 not equal h16";
	//						match = false;
	//						break;
	//					}
	//					h = h32;
	//				}
	//				else if (h32 != FLT_MAX)
	//					h = h32;
	//				else if (h16 != FLT_MAX)
	//					h = h16;
	//				else
	//				{
	//					_msg += "both h32 and h16 are FLT_MAX";
	//					match = false;
	//					break;
	//				}

	//				if (h < minHeight)
	//					minHeight = h;
	//				if (h > maxHeight)
	//					maxHeight = h;

	//				if (x == 0)
	//				{
	//					if (h < westSideMin)
	//						westSideMin = h;
	//					if (h > westSideMax)
	//						westSideMax = h;
	//				}

	//				if (z == 0)
	//				{
	//					if (h < northSideMin)
	//						northSideMin = h;
	//					if (h > northSideMax)
	//						northSideMax = h;
	//				}

	//				if (x == maxIndex)
	//				{
	//					if (h < eastSideMin)
	//						eastSideMin = h;
	//					if (h > eastSideMax)
	//						eastSideMax = h;
	//				}

	//				if (z == maxIndex)
	//				{
	//					if (h < southSideMin)
	//						southSideMin = h;
	//					if (h > southSideMax)
	//						southSideMax = h;
	//				}
	//			}

	//			if (not match)
	//				break;
	//		}

	//		if (match)
	//		{
	//			if (terrainEdit->minHeight != minHeight)
	//			{
	//				_msg += (std::string("minHeight terrainEdit ") + std::to_string(terrainEdit->minHeight) + " != recalc " + std::to_string(minHeight) + " ");
	//				match = false;
	//			}
	//			if (terrainEdit->maxHeight != maxHeight)
	//			{
	//				_msg += (std::string("maxHeight terrainEdit ") + std::to_string(terrainEdit->maxHeight) + " != recalc " + std::to_string(maxHeight) + " ");
	//				match = false;
	//			}
	//			if (terrainEdit->northSideZMinus.minHeight != northSideMin or terrainEdit->northSideZMinus.maxHeight != northSideMax)
	//			{
	//				_msg += " - N ";
	//				if (terrainEdit->northSideZMinus.minHeight != northSideMin)
	//				{
	//					_msg += (std::string("minHeight terrainEdit ") + std::to_string(terrainEdit->northSideZMinus.minHeight) + " != recalc " + std::to_string(northSideMin) + " ");
	//					match = false;
	//				}
	//				if (terrainEdit->northSideZMinus.maxHeight != northSideMax)
	//				{
	//					_msg += (std::string("maxHeight terrainEdit ") + std::to_string(terrainEdit->northSideZMinus.maxHeight) + " != recalc " + std::to_string(northSideMax) + " ");
	//					match = false;
	//				}
	//			}
	//			if (terrainEdit->southSideZPlus.minHeight != southSideMin or terrainEdit->southSideZPlus.maxHeight != southSideMax)
	//			{
	//				_msg += " - S ";
	//				if (terrainEdit->southSideZPlus.minHeight != southSideMin)
	//				{
	//					_msg += (std::string("minHeight terrainEdit ") + std::to_string(terrainEdit->southSideZPlus.minHeight) + " != recalc " + std::to_string(southSideMin) + " ");
	//					match = false;
	//				}
	//				if (terrainEdit->southSideZPlus.maxHeight != southSideMax)
	//				{
	//					_msg += (std::string("maxHeight terrainEdit ") + std::to_string(terrainEdit->southSideZPlus.maxHeight) + " != recalc " + std::to_string(southSideMax) + " ");
	//					match = false;
	//				}
	//			}
	//			if (terrainEdit->westSideXMinus.minHeight != westSideMin or terrainEdit->westSideXMinus.maxHeight != westSideMax)
	//			{
	//				_msg += " - W ";
	//				if (terrainEdit->westSideXMinus.minHeight != westSideMin)
	//				{
	//					_msg += (std::string("minHeight terrainEdit ") + std::to_string(terrainEdit->westSideXMinus.minHeight) + " != recalc " + std::to_string(westSideMin) + " ");
	//					match = false;
	//				}
	//				if (terrainEdit->westSideXMinus.maxHeight != westSideMax)
	//				{
	//					_msg += (std::string("maxHeight terrainEdit ") + std::to_string(terrainEdit->westSideXMinus.maxHeight) + " != recalc " + std::to_string(westSideMax) + " ");
	//					match = false;
	//				}
	//			}
	//			if (terrainEdit->eastSideXPlus.minHeight != eastSideMin or terrainEdit->eastSideXPlus.maxHeight != eastSideMax)
	//			{
	//				_msg += " - E ";
	//				if (terrainEdit->eastSideXPlus.minHeight != eastSideMin)
	//				{
	//					_msg += (std::string("minHeight terrainEdit ") + std::to_string(terrainEdit->eastSideXPlus.minHeight) + " != recalc " + std::to_string(eastSideMin) + " ");
	//					match = false;
	//				}
	//				if (terrainEdit->eastSideXPlus.maxHeight != eastSideMax)
	//				{
	//					_msg += (std::string("maxHeight terrainEdit ") + std::to_string(terrainEdit->eastSideXPlus.maxHeight) + " != recalc " + std::to_string(eastSideMax) + " ");
	//					match = false;
	//				}
	//			}
	//		}
	//	}

	//	//// DEBUG
	//	//if (match)
	//	//{
	//	//	pos.TRACK_QUADRANT_FOR_DEBUG(msgHeader + " - MinMax MATCHING", _meshId);
	//	//}
	//	//else
	//	//{
	//	//	std::string s = "MinMax " + std::to_string(terrainEdit->minHeight) + "/" + std::to_string(terrainEdit->maxHeight)
	//	//		+ " N=" + std::to_string(terrainEdit->northSideZMinus.minHeight) + "/" + std::to_string(terrainEdit->northSideZMinus.maxHeight)
	//	//		+ " S=" + std::to_string(terrainEdit->southSideZPlus.minHeight) + "/" + std::to_string(terrainEdit->southSideZPlus.maxHeight)
	//	//		+ " E=" + std::to_string(terrainEdit->eastSideXPlus.minHeight) + "/" + std::to_string(terrainEdit->eastSideXPlus.maxHeight)
	//	//		+ " W=" + std::to_string(terrainEdit->westSideXMinus.minHeight) + "/" + std::to_string(terrainEdit->westSideXMinus.maxHeight);
	//	//	pos.TRACK_QUADRANT_FOR_DEBUG(s, _meshId);
	//	//	pos.TRACK_QUADRANT_FOR_DEBUG(msgHeader + " - MinMax MISMATCH - " + _msg, _meshId);
	//	//}
	//	//// DEBUG

	//	if (match)
	//		return match;
	//	else
	//		return match;
	//}

	bool MeshCacheBuffer::calcMinMaxHeights(TheWorld_Utils::QuadrantPos pos, bool renewMeshIdOnUpdate, CacheQuadrantData& data)
	{
		bool updated = false;

		size_t numVerticesPerSize = (size_t)pos.getNumVerticesPerSize();

		my_assert(data.heights32Buffer != nullptr and data.heights32Buffer->size() == numVerticesPerSize * numVerticesPerSize * sizeof(float));
		my_assert(data.terrainEditValues != nullptr and data.terrainEditValues->size() == sizeof(TerrainEdit));

		//my_assert(data.heights16Buffer != nullptr and data.heights16Buffer->size() == numVerticesPerSize * numVerticesPerSize * sizeof(uint16_t));
		//{
		//	size_t numVertices = numVerticesPerSize * numVerticesPerSize;

		//	size_t heights16BufferSize = numVertices * sizeof(uint16_t);
		//	uint16_t* movingHeights16Buffer = (uint16_t*)data.heights16Buffer->ptr();

		//	size_t heights32BufferSize = numVertices * sizeof(float);
		//	float* movingHeights32Buffer = (float*)data.heights32Buffer->ptr();
		//	
		//	{
		//		TheWorld_Utils::GuardProfiler profiler(std::string("generateDetailDataForQuadrants 2 ") + __FUNCTION__, "Move Heights to float16/float32 buffer");
		//		for (int z = 0; z < numVerticesPerSize; z++)
		//			for (int x = 0; x < numVerticesPerSize; x++)
		//			{
		//				float altitude = *movingHeights32Buffer;
		//				TheWorld_Utils::FLOAT_32 f(altitude);
		//				*movingHeights16Buffer = half_from_float(f.u32);
		//				f.u32 = half_to_float(*movingHeights16Buffer);
		//				if (*movingHeights32Buffer != f.f32)
		//				{
		//					*movingHeights32Buffer = f.f32;
		//					updated = true;
		//				}
		//				movingHeights16Buffer++;
		//				movingHeights32Buffer++;
		//			}
		//		PLOG_VERBOSE << "calcMinMaxHeights - " << "Adjusted Heights to float16/float32 buffer to prevent height mismatch " << pos.getIdStr() << " " << pos.getName() << " - Elapsed " << profiler.partialDuration() / 1000 << " ms";
		//	}
		//}
		
		//if (data.heights32Buffer == nullptr or data.heights16Buffer == nullptr)
		//	return updated;

		TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();

		float minHeight = FLT_MAX;
		float maxHeight = -FLT_MAX;
		float eastSideXPlusMinHeight = FLT_MAX;
		float eastSideXPlusMaxHeight = -FLT_MAX;
		float westSideXMinusMinHeight = FLT_MAX;
		float westSideXMinusMaxHeight = -FLT_MAX;
		float southSideZPlusMinHeight = FLT_MAX;
		float southSideZPlusMaxHeight = -FLT_MAX;
		float northSideZMinusMinHeight = FLT_MAX;
		float northSideZMinusMaxHeight = -FLT_MAX;

		size_t maxIndex = numVerticesPerSize - 1;

		for (size_t z = 0; z < numVerticesPerSize; z++)
		{
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float h = data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
				bool changed = false;
				h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h, changed);
				if (changed)
				{
					data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = h;
					updated = true;
				}
				if (h < minHeight)
					minHeight = h;
				if (h > maxHeight)
					maxHeight = h;

				if (x == 0)
				{
					if (h < westSideXMinusMinHeight)
						westSideXMinusMinHeight = h;
					if (h > westSideXMinusMaxHeight)
						westSideXMinusMaxHeight = h;
				}

				if (z == 0)
				{
					if (h < northSideZMinusMinHeight)
						northSideZMinusMinHeight = h;
					if (h > northSideZMinusMaxHeight)
						northSideZMinusMaxHeight = h;
				}

				if (x == maxIndex)
				{
					if (h < eastSideXPlusMinHeight)
						eastSideXPlusMinHeight = h;
					if (h > eastSideXPlusMaxHeight)
						eastSideXPlusMaxHeight = h;
				}

				if (z == maxIndex)
				{
					if (h < southSideZPlusMinHeight)
						southSideZPlusMinHeight = h;
					if (h > southSideZPlusMaxHeight)
						southSideZPlusMaxHeight = h;
				}
			}
		}

		if (terrainEdit->minHeight != minHeight)
		{
			terrainEdit->minHeight = minHeight;
			updated = true;
		}
		if (terrainEdit->maxHeight != maxHeight)
		{
			terrainEdit->maxHeight = maxHeight;
			updated = true;
		}
		if (terrainEdit->eastSideXPlus.minHeight != eastSideXPlusMinHeight)
		{
			terrainEdit->eastSideXPlus.minHeight = eastSideXPlusMinHeight;
			updated = true;
		}
		if (terrainEdit->eastSideXPlus.maxHeight != eastSideXPlusMaxHeight)
		{
			terrainEdit->eastSideXPlus.maxHeight = eastSideXPlusMaxHeight;
			updated = true;
		}
		if (terrainEdit->westSideXMinus.minHeight != westSideXMinusMinHeight)
		{
			terrainEdit->westSideXMinus.minHeight = westSideXMinusMinHeight;
			updated = true;
		}
		if (terrainEdit->westSideXMinus.maxHeight != westSideXMinusMaxHeight)
		{
			terrainEdit->westSideXMinus.maxHeight = westSideXMinusMaxHeight;
			updated = true;
		}
		if (terrainEdit->southSideZPlus.minHeight != southSideZPlusMinHeight)
		{
			terrainEdit->southSideZPlus.minHeight = southSideZPlusMinHeight;
			updated = true;
		}
		if (terrainEdit->southSideZPlus.maxHeight != southSideZPlusMaxHeight)
		{
			terrainEdit->southSideZPlus.maxHeight = southSideZPlusMaxHeight;
			updated = true;
		}
		if (terrainEdit->northSideZMinus.minHeight != northSideZMinusMinHeight)
		{
			terrainEdit->northSideZMinus.minHeight = northSideZMinusMinHeight;
			updated = true;
		}
		if (terrainEdit->northSideZMinus.maxHeight != northSideZMinusMaxHeight)
		{
			terrainEdit->northSideZMinus.maxHeight = northSideZMinusMaxHeight;
			updated = true;
		}

		if (renewMeshIdOnUpdate and updated)
		{
			data.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();	// update meshId as terrain data updated
		}

		return updated;
	}

	//bool MeshCacheBuffer::calcMinMaxHeightsDebug(TheWorld_Utils::QuadrantPos pos, bool renewMeshIdOnUpdate, CacheQuadrantData& data)
	//{
	//	bool updated = false;

	//	size_t numVerticesPerSize = (size_t)pos.getNumVerticesPerSize();

	//	my_assert(data.heights32Buffer != nullptr and data.heights32Buffer->size() == numVerticesPerSize * numVerticesPerSize * sizeof(float));
	//	my_assert(data.heights16Buffer != nullptr and data.heights16Buffer->size() == numVerticesPerSize * numVerticesPerSize * sizeof(uint16_t));
	//	my_assert(data.terrainEditValues != nullptr and data.terrainEditValues->size() == sizeof(TerrainEdit));

	//	{
	//		size_t numVertices = numVerticesPerSize * numVerticesPerSize;

	//		size_t heights16BufferSize = numVertices * sizeof(uint16_t);
	//		uint16_t* movingHeights16Buffer = (uint16_t*)data.heights16Buffer->ptr();

	//		size_t heights32BufferSize = numVertices * sizeof(float);
	//		float* movingHeights32Buffer = (float*)data.heights32Buffer->ptr();

	//		{
	//			TheWorld_Utils::GuardProfiler profiler(std::string("generateDetailDataForQuadrants 2 ") + __FUNCTION__, "Move Heights to float16/float32 buffer");
	//			for (int z = 0; z < numVerticesPerSize; z++)
	//				for (int x = 0; x < numVerticesPerSize; x++)
	//				{
	//					float altitude = *movingHeights32Buffer;
	//					TheWorld_Utils::FLOAT_32 f(altitude);
	//					*movingHeights16Buffer = half_from_float(f.u32);
	//					
	//					f.u32 = half_to_float(*movingHeights16Buffer);
	//					if (*movingHeights32Buffer != f.f32)
	//					{
	//						*movingHeights32Buffer = f.f32;
	//						updated = true;
	//					}
	//					
	//					movingHeights16Buffer++;
	//					movingHeights32Buffer++;
	//				}
	//			PLOG_VERBOSE << "calcMinMaxHeights - " << "Adjusted Heights to float16/float32 buffer to prevent height mismatch " << pos.getIdStr() << " " << pos.getName() << " - Elapsed " << profiler.partialDuration() / 1000 << " ms";
	//		}
	//	}

	//	//if (data.heights32Buffer == nullptr or data.heights16Buffer == nullptr)
	//	//	return updated;

	//	//TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();

	//	//float minHeight = FLT_MAX;
	//	//float maxHeight = -FLT_MAX;
	//	//float eastSideXPlusMinHeight = FLT_MAX;
	//	//float eastSideXPlusMaxHeight = -FLT_MAX;
	//	//float westSideXMinusMinHeight = FLT_MAX;
	//	//float westSideXMinusMaxHeight = -FLT_MAX;
	//	//float southSideZPlusMinHeight = FLT_MAX;
	//	//float southSideZPlusMaxHeight = -FLT_MAX;
	//	//float northSideZMinusMinHeight = FLT_MAX;
	//	//float northSideZMinusMaxHeight = -FLT_MAX;

	//	//size_t maxIndex = numVerticesPerSize - 1;

	//	//for (size_t z = 0; z < numVerticesPerSize; z++)
	//	//{
	//	//	for (size_t x = 0; x < numVerticesPerSize; x++)
	//	//	{
	//	//		float h = data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
	//	//		if (h < minHeight)
	//	//			minHeight = h;
	//	//		if (h > maxHeight)
	//	//			maxHeight = h;

	//	//		if (x == 0)
	//	//		{
	//	//			if (h < westSideXMinusMinHeight)
	//	//				westSideXMinusMinHeight = h;
	//	//			if (h > westSideXMinusMaxHeight)
	//	//				westSideXMinusMaxHeight = h;
	//	//		}

	//	//		if (z == 0)
	//	//		{
	//	//			if (h < northSideZMinusMinHeight)
	//	//				northSideZMinusMinHeight = h;
	//	//			if (h > northSideZMinusMaxHeight)
	//	//				northSideZMinusMaxHeight = h;
	//	//		}

	//	//		if (x == maxIndex)
	//	//		{
	//	//			if (h < eastSideXPlusMinHeight)
	//	//				eastSideXPlusMinHeight = h;
	//	//			if (h > eastSideXPlusMaxHeight)
	//	//				eastSideXPlusMaxHeight = h;
	//	//		}

	//	//		if (z == maxIndex)
	//	//		{
	//	//			if (h < southSideZPlusMinHeight)
	//	//				southSideZPlusMinHeight = h;
	//	//			if (h > southSideZPlusMaxHeight)
	//	//				southSideZPlusMaxHeight = h;
	//	//		}
	//	//	}
	//	//}

	//	//if (terrainEdit->minHeight != minHeight)
	//	//{
	//	//	terrainEdit->minHeight = minHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->maxHeight != maxHeight)
	//	//{
	//	//	terrainEdit->maxHeight = maxHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->eastSideXPlus.minHeight != eastSideXPlusMinHeight)
	//	//{
	//	//	terrainEdit->eastSideXPlus.minHeight = eastSideXPlusMinHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->eastSideXPlus.maxHeight != eastSideXPlusMaxHeight)
	//	//{
	//	//	terrainEdit->eastSideXPlus.maxHeight = eastSideXPlusMaxHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->westSideXMinus.minHeight != westSideXMinusMinHeight)
	//	//{
	//	//	terrainEdit->westSideXMinus.minHeight = westSideXMinusMinHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->westSideXMinus.maxHeight != westSideXMinusMaxHeight)
	//	//{
	//	//	terrainEdit->westSideXMinus.maxHeight = westSideXMinusMaxHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->southSideZPlus.minHeight != southSideZPlusMinHeight)
	//	//{
	//	//	terrainEdit->southSideZPlus.minHeight = southSideZPlusMinHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->southSideZPlus.maxHeight != southSideZPlusMaxHeight)
	//	//{
	//	//	terrainEdit->southSideZPlus.maxHeight = southSideZPlusMaxHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->northSideZMinus.minHeight != northSideZMinusMinHeight)
	//	//{
	//	//	terrainEdit->northSideZMinus.minHeight = northSideZMinusMinHeight;
	//	//	updated = true;
	//	//}
	//	//if (terrainEdit->northSideZMinus.maxHeight != northSideZMinusMaxHeight)
	//	//{
	//	//	terrainEdit->northSideZMinus.maxHeight = northSideZMinusMaxHeight;
	//	//	updated = true;
	//	//}

	//	//if (renewMeshIdOnUpdate and updated)
	//	//{
	//	//	data.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();	// update meshId as terrain data updated
	//	//}

	//	return updated;
	//}

	bool MeshCacheBuffer::blendQuadrant(TheWorld_Utils::QuadrantPos pos, bool lastPhase, bool renewMeshIdOnUpdate,
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
		TheWorld_Utils::QuadrantPos northQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::NorthZMinus);
		TheWorld_Utils::QuadrantPos southQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::SouthZPlus);
		TheWorld_Utils::QuadrantPos westQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::WestXMinus);
		TheWorld_Utils::QuadrantPos eastQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::EastXPlus);
		TheWorld_Utils::QuadrantPos northwestQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::NorthwestZMinusXMinus);
		TheWorld_Utils::QuadrantPos northeastQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::NortheastZMinusXPlus);
		TheWorld_Utils::QuadrantPos southwestQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::SouthwestZPlusXMinus);
		TheWorld_Utils::QuadrantPos southeastQuadrantPos = pos.getQuadrantPos(TheWorld_Utils::QuadrantPos::DirectionSlot::SoutheastZPlusXPlus);

		bool updated = false;

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();
		float gridStepInWU = pos.getGridStepInWU();

		//if (!terrainEdit->northSideZMinus.needBlend && !terrainEdit->southSideZPlus.needBlend && !terrainEdit->eastSideXPlus.needBlend && !terrainEdit->westSideXMinus.needBlend)
		//	return updated;
		
		//	NW	N	NE
		//		_
		//	W	D	E
		//
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(pos, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D	E
		//		-
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(pos, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	|D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnWestSide(pos, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D|	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnEastSide(pos, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
			updated = true;

		CacheQuadrantData emptyData;

		//	NW	|N	NE
		//		
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnWestSide(northQuadrantPos, lastPhase, northData,
			emptyData /*N*/, data /*S*/, northwestData /*W*/, northeastData /*E*/, emptyData /*NW*/, emptyData /*NE*/, westData /*SW*/, eastData /*SE*/))
			updated = true;
		//	NW	N|	NE
		//		
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnEastSide(northQuadrantPos, lastPhase, northData,
			emptyData /*N*/, data /*S*/, northwestData /*W*/, northeastData /*E*/, emptyData /*NW*/, emptyData /*NE*/, westData /*SW*/, eastData /*SE*/))
			updated = true;

		//	NW	N	NE
		//		
		//	W	D	E
		//		
		//	SW	|S	SE
		if (blendQuadrantOnWestSide(southQuadrantPos, lastPhase, southData,
			data /*N*/, emptyData /*S*/, southwestData /*W*/, southeastData /*E*/, westData /*NW*/, eastData /*NE*/, emptyData /*SW*/, emptyData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//		
		//	SW	S|	SE
		if (blendQuadrantOnEastSide(southQuadrantPos, lastPhase, southData,
			data /*N*/, emptyData /*S*/, southwestData /*W*/, southeastData /*E*/, westData /*NW*/, eastData /*NE*/, emptyData /*SW*/, emptyData /*SE*/))
			updated = true;

		//	NW	N	NE
		//	_	
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(westQuadrantPos, lastPhase, westData,
			northwestData /*N*/, southwestData /*S*/, emptyData /*W*/, data /*E*/, emptyData /*NW*/, northData /*NE*/, emptyData /*SW*/, southData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//	-	
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(westQuadrantPos, lastPhase, westData,
			northwestData /*N*/, southwestData /*S*/, emptyData /*W*/, data /*E*/, emptyData /*NW*/, northData /*NE*/, emptyData /*SW*/, southData /*SE*/))
			updated = true;

		//	NW	N	NE
		//			_
		//	W	D	E
		//		
		//	SW	S	SE
		if (blendQuadrantOnNorthSide(eastQuadrantPos, lastPhase, eastData,
			northeastData /*N*/, southeastData /*S*/, data /*W*/, emptyData /*E*/, northData /*NW*/, emptyData /*NE*/, southData /*SW*/, emptyData /*SE*/))
			updated = true;
		//	NW	N	NE
		//		
		//	W	D	E
		//			-
		//	SW	S	SE
		if (blendQuadrantOnSouthSide(eastQuadrantPos, lastPhase, eastData,
			northeastData /*N*/, southeastData /*S*/, data /*W*/, emptyData /*E*/, northData /*NW*/, emptyData /*NE*/, southData /*SW*/, emptyData /*SE*/))
			updated = true;

		if (lastPhase)
		{
			if (blendBorders(pos, lastPhase, data, northData, southData, westData, eastData, northwestData, northeastData, southwestData, southeastData))
				updated = true;
		}

		bool minMaxUpdated = false, northSideMinMaxUpdated = false, southSideMinMaxUpdated = false, westSideMinMaxUpdated = false, eastSideMinMaxUpdated = false, 
			northwestSideMinMaxUpdated = false, northeastSideMinMaxUpdated = false, southwestSideMinMaxUpdated = false, southeastSideMinMaxUpdated = false;
		
		//if (lastPhase or data.heightsUpdated)
		//{
		//	TerrainEdit* terrainEdit = (TerrainEdit*)data.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(pos, numVerticesPerSize, terrainEdit, *data.heights32Buffer))
		//	{
		//		updated = minMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or northData.heightsUpdated) and northData.terrainEditValues != nullptr and northData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* northTerrainEdit = (TerrainEdit*)northData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(northQuadrantPos, numVerticesPerSize, northTerrainEdit, *northData.heights32Buffer))
		//	{
		//		updated = northSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or southData.heightsUpdated) and southData.terrainEditValues != nullptr and southData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* southTerrainEdit = (TerrainEdit*)southData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(southQuadrantPos,numVerticesPerSize, southTerrainEdit, *southData.heights32Buffer))
		//	{
		//		updated = southSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or westData.heightsUpdated) and westData.terrainEditValues != nullptr and westData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* westTerrainEdit = (TerrainEdit*)westData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(westQuadrantPos, numVerticesPerSize, westTerrainEdit, *westData.heights32Buffer))
		//	{
		//		updated = westSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or eastData.heightsUpdated) and eastData.terrainEditValues != nullptr and eastData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* eastTerrainEdit = (TerrainEdit*)eastData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(eastQuadrantPos, numVerticesPerSize, eastTerrainEdit, *eastData.heights32Buffer))
		//	{
		//		updated = eastSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or northwestData.heightsUpdated) and northwestData.terrainEditValues != nullptr and northwestData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* northwestTerrainEdit = (TerrainEdit*)northwestData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(northwestQuadrantPos, numVerticesPerSize, northwestTerrainEdit, *northwestData.heights32Buffer))
		//	{
		//		updated = northwestSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or northeastData.heightsUpdated) and northeastData.terrainEditValues != nullptr and northeastData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* northeastTerrainEdit = (TerrainEdit*)northeastData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(northeastQuadrantPos, numVerticesPerSize, northeastTerrainEdit, *northeastData.heights32Buffer))
		//	{
		//		updated = northeastSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or southwestData.heightsUpdated) and southwestData.terrainEditValues != nullptr and southwestData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* southwesthTerrainEdit = (TerrainEdit*)southwestData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(southwestQuadrantPos, numVerticesPerSize, southwesthTerrainEdit, *southwestData.heights32Buffer))
		//	{
		//		updated = southwestSideMinMaxUpdated = true;
		//	}
		//}

		//if ((lastPhase or southeastData.heightsUpdated) and southeastData.terrainEditValues != nullptr and southeastData.terrainEditValues->ptr() != nullptr)
		//{
		//	TerrainEdit* southeastTerrainEdit = (TerrainEdit*)southeastData.terrainEditValues->ptr();
		//	if (calcMinMaxHeight(southeastQuadrantPos, numVerticesPerSize, southeastTerrainEdit, *southeastData.heights32Buffer))
		//	{
		//		updated = southeastSideMinMaxUpdated = true;
		//	}
		//}

		if (renewMeshIdOnUpdate)
		{
			if (data.heightsUpdated or minMaxUpdated)
				data.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();	// update meshId as heights updated
			if (northData.heightsUpdated or northSideMinMaxUpdated)
				northData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (southData.heightsUpdated or southSideMinMaxUpdated)
				southData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (westData.heightsUpdated or westSideMinMaxUpdated)
				westData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (eastData.heightsUpdated or eastSideMinMaxUpdated)
				eastData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (northwestData.heightsUpdated or northwestSideMinMaxUpdated)
				northwestData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (northeastData.heightsUpdated or northeastSideMinMaxUpdated)
				northeastData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (southwestData.heightsUpdated or southwestSideMinMaxUpdated)
				southwestData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
			if (southeastData.heightsUpdated or southeastSideMinMaxUpdated)
				southeastData.meshId = TheWorld_Utils::MeshCacheBuffer::generateNewMeshId();
		}
		
		return updated;
	}

	bool MeshCacheBuffer::blendBorders(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		if (currentData.heights32Buffer == nullptr || currentData.heights32Buffer->size() == 0
			/*|| currentData.heights16Buffer == nullptr || currentData.heights16Buffer->size() == 0*/)
			return updated;

		size_t maxIndex = numVerticesPerSize - 1;

		// idx = 0 ==> North/West corner of current quadrant
		for (size_t idx = 0; idx < numVerticesPerSize; idx++)
		{
			
			//	NW	N	NE
			//		-
			//	W	D	E
			//			
			//	SW	S	SE
			if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				/*&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northData.heights32Buffer;
				CacheQuadrantData* data1 = &northData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;
				
				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = currentData.heights32Buffer;
				CacheQuadrantData* data2 = &currentData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = currentData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//		-	
			//	SW	S	SE
			if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				/*&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = currentData.heights32Buffer;
				CacheQuadrantData* data1 = &currentData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southData.heights32Buffer;
				CacheQuadrantData* data2 = &southData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = currentData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D |	E
			//			
			//	SW	S	SE
			if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				/*&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = currentData.heights32Buffer;
				CacheQuadrantData* data1 = &currentData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = eastData.heights32Buffer;
				CacheQuadrantData* data2 = &eastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = currentData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = eastData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W | D	E
			//			
			//	SW  S	SE
			if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				/*&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = westData.heights32Buffer;
				CacheQuadrantData* data1 = &westData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = currentData.heights32Buffer;
				CacheQuadrantData* data2 = &currentData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = westData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = currentData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW |N	NE
			//		
			//	W	D	E
			//			
			//	SW	S	SE
			if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
				&& northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				/*&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0
				&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northwestData.heights32Buffer;
				CacheQuadrantData* data1 = &northwestData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = northData.heights32Buffer;
				CacheQuadrantData* data2 = &northData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northwestData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = northData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N|	NE
			//		
			//	W	D	E
			//			
			//	SW	S	SE
			if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
				&& northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
				/*&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0
				&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northData.heights32Buffer;
				CacheQuadrantData* data1 = &northData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = northeastData.heights32Buffer;
				CacheQuadrantData* data2 = &northeastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = northeastData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//	-	
			//	W	D	E
			//			
			//	SW	S	SE
			if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
				&& westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				/*&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0
				&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northwestData.heights32Buffer;
				CacheQuadrantData* data1 = &northwestData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = westData.heights32Buffer;
				CacheQuadrantData* data2 = &westData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northwestData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = westData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//			-
			//	W	D	E
			//			
			//	SW	S	SE
			if (northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
				&& eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				/*&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0
				&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = northeastData.heights32Buffer;
				CacheQuadrantData* data1 = &northeastData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = eastData.heights32Buffer;
				CacheQuadrantData* data2 = &eastData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = northeastData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = eastData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//	-		
			//	SW	S	SE
			if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
				&& southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
				/*&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0
				&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = westData.heights32Buffer;
				CacheQuadrantData* data1 = &westData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southwestData.heights32Buffer;
				CacheQuadrantData* data2 = &southwestData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = westData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southwestData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			-		
			//	SW	S	SE
			if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
				&& southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
				/*&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0
				&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = eastData.heights32Buffer;
				CacheQuadrantData* data1 = &eastData;
				size_t border1X = idx;				size_t border1Z = maxIndex;
				size_t nearBorder1X = idx;			size_t nearBorder1Z = maxIndex - 1;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southeastData.heights32Buffer;
				CacheQuadrantData* data2 = &southeastData;
				size_t border2X = idx;				size_t border2Z = 0;
				size_t nearBorder2X = idx;			size_t nearBorder2Z = 1;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = eastData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southeastData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			
			//	SW	|S	SE
			if (southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
				&& southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				/*&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0
				&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = southwestData.heights32Buffer;
				CacheQuadrantData* data1 = &southwestData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southData.heights32Buffer;
				CacheQuadrantData* data2 = &southData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = southwestData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}

			//	NW	N	NE
			//		
			//	W	D	E
			//			
			//	SW	S|	SE
			if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
				&& southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
				/*&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0
				&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0*/)
			{
				TheWorld_Utils::MemoryBuffer* data1heights32Buffer = southData.heights32Buffer;
				CacheQuadrantData* data1 = &southData;
				size_t border1X = maxIndex;			size_t border1Z = idx;
				size_t nearBorder1X = maxIndex - 1;	size_t nearBorder1Z = idx;

				TheWorld_Utils::MemoryBuffer* data2heights32Buffer = southeastData.heights32Buffer;
				CacheQuadrantData* data2 = &southeastData;
				size_t border2X = 0;					size_t border2Z = idx;
				size_t nearBorder2X = 1;				size_t nearBorder2Z = idx;

				float nearBorderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(nearBorder1X, nearBorder1Z, numVerticesPerSize);
				float nearBorderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(nearBorder2X, nearBorder2Z, numVerticesPerSize);
				float borderH1 = data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize);
				float borderH2 = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize);
				if (borderH1 != borderH2)
				{
					updated = data1->heightsUpdated = data2->heightsUpdated = true;
					float h = (nearBorderH1 + nearBorderH2) / 2;
					h = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(h);
					data1heights32Buffer->_deprecated_get_at_ref<float>(border1X, border1Z, numVerticesPerSize) = data2heights32Buffer->_deprecated_get_at_ref<float>(border2X, border2Z, numVerticesPerSize) = h;

					//TheWorld_Utils::FLOAT_32 f(h);
					//uint16_t half = half_from_float(f.u32);
					//TheWorld_Utils::MemoryBuffer* data1heights16Buffer = southData.heights16Buffer;
					//TheWorld_Utils::MemoryBuffer* data2heights16Buffer = southeastData.heights16Buffer;
					//data1heights16Buffer->_deprecated_get_at_ref<uint16_t>(border1X, border1Z, numVerticesPerSize) = data2heights16Buffer->_deprecated_get_at_ref<uint16_t>(border2X, border2Z, numVerticesPerSize) = half;
				}
			}
		}

		bool changed = false;

		float northWestCornerHeight = currentData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize);
		northWestCornerHeight = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(northWestCornerHeight, changed);
		if (changed)
			updated = currentData.heightsUpdated = true;
		if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
			/*&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0*/)
		{
			if (northData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) != northWestCornerHeight)
			{
				northData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) = northWestCornerHeight;
				updated = northData.heightsUpdated = true;
			}
		}
		if (northwestData.heights32Buffer != nullptr && northwestData.heights32Buffer->size() > 0
			/*&& northwestData.heights16Buffer != nullptr && northwestData.heights16Buffer->size() > 0*/)
		{
			if (northwestData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) != northWestCornerHeight)
			{
				northwestData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) = northWestCornerHeight;
				updated = northwestData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(northWestCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//northwestData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
			}
		}
		if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
			/*&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0*/)
		{
			if (westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) != northWestCornerHeight)
			{
				westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) = northWestCornerHeight;
				updated = westData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(northWestCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//westData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
			}
		}
		float northEastCornerHeight = currentData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize);
		northEastCornerHeight = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(northEastCornerHeight, changed);
		if (changed)
			updated = currentData.heightsUpdated = true;
		if (northData.heights32Buffer != nullptr && northData.heights32Buffer->size() > 0
			/*&& northData.heights16Buffer != nullptr && northData.heights16Buffer->size() > 0*/)
		{
			if (northData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) != northEastCornerHeight)
			{
				northData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) = northEastCornerHeight;
				updated = northData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//northData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
			}
		}
		if (northeastData.heights32Buffer != nullptr && northeastData.heights32Buffer->size() > 0
			/*&& northeastData.heights16Buffer != nullptr && northeastData.heights16Buffer->size() > 0*/)
		{
			if (northeastData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) != northEastCornerHeight)
			{
				northeastData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) = northEastCornerHeight;
				updated = northeastData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//northeastData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(0, maxIndex, numVerticesPerSize) = half;
			}
		}
		if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
			/*&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0*/)
		{
			if (eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) != northEastCornerHeight)
			{
				eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) = northEastCornerHeight;
				updated = eastData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(northEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//eastData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(0, 0, numVerticesPerSize) = half;
			}
		}

		float southWestCornerHeight = currentData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize);
		southWestCornerHeight = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(southWestCornerHeight, changed);
		if (changed)
			updated = currentData.heightsUpdated = true;
		if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
			/*&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0*/)
		{
			if (southData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) != southWestCornerHeight)
			{
				southData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) = southWestCornerHeight;
				updated = southData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//southData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(0, 0, numVerticesPerSize) = half;
			}
		}
		if (southwestData.heights32Buffer != nullptr && southwestData.heights32Buffer->size() > 0
			/*&& southwestData.heights16Buffer != nullptr && southwestData.heights16Buffer->size() > 0*/)
		{
			if (southwestData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) != southWestCornerHeight)
			{
				southwestData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) = southWestCornerHeight;
				updated = southwestData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//southwestData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
			}
		}
		if (westData.heights32Buffer != nullptr && westData.heights32Buffer->size() > 0
			/*&& westData.heights16Buffer != nullptr && westData.heights16Buffer->size() > 0*/)
		{
			if (westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) != southWestCornerHeight)
			{
				westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize) = southWestCornerHeight;
				updated = westData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southWestCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//westData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, maxIndex, numVerticesPerSize) = half;
			}
		}

		float southEastCornerHeight = currentData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, maxIndex, numVerticesPerSize);
		southEastCornerHeight = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(southEastCornerHeight, changed);
		if (changed)
			updated = currentData.heightsUpdated = true;
		if (southData.heights32Buffer != nullptr && southData.heights32Buffer->size() > 0
			/*&& southData.heights16Buffer != nullptr && southData.heights16Buffer->size() > 0*/)
		{
			if (southData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) != southEastCornerHeight)
			{
				southData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, 0, numVerticesPerSize) = southEastCornerHeight;
				updated = southData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//southData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(maxIndex, 0, numVerticesPerSize) = half;
			}
		}
		if (southeastData.heights32Buffer != nullptr && southeastData.heights32Buffer->size() > 0
			/*&& southeastData.heights16Buffer != nullptr && southeastData.heights16Buffer->size() > 0*/)
		{
			if (southeastData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) != southEastCornerHeight)
			{
				southeastData.heights32Buffer->_deprecated_get_at_ref<float>(0, 0, numVerticesPerSize) = southEastCornerHeight;
				updated = southeastData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//southeastData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(0, 0, numVerticesPerSize) = half;
			}	
		}
		if (eastData.heights32Buffer != nullptr && eastData.heights32Buffer->size() > 0
			/*&& eastData.heights16Buffer != nullptr && eastData.heights16Buffer->size() > 0*/)
		{
			if (eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) != southEastCornerHeight)
			{
				eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, maxIndex, numVerticesPerSize) = southEastCornerHeight;
				updated = eastData.heightsUpdated = true;
				//TheWorld_Utils::FLOAT_32 f(southEastCornerHeight);
				//uint16_t half = half_from_float(f.u32);
				//eastData.heights16Buffer->_deprecated_get_at_ref<uint16_t>(0, maxIndex, numVerticesPerSize) = half;
			}
		}

		return updated;
	}
		
	//bool MeshCacheBuffer::calcMinMaxHeight(TheWorld_Utils::QuadrantPos& pos, size_t numVerticesPerSize, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& heights32Buffer)
	//{
	//	bool updated = false;
	//	
	//	float terrainEditMinHeight = FLT_MAX;
	//	float terrainEditMaxHeight = -FLT_MAX;
	//	float terrainEditEastSideXPlusMinHeight = FLT_MAX;
	//	float terrainEditEastSideXPlusMaxHeight = -FLT_MAX;
	//	float terrainEditWestSideXMinusMinHeight = FLT_MAX;
	//	float terrainEditWestSideXMinusMaxHeight = -FLT_MAX;
	//	float terrainEditSouthSideZPlusMinHeight = FLT_MAX;
	//	float terrainEditSouthSideZPlusMaxHeight = -FLT_MAX;
	//	float terrainEditNorthSideZMinusMinHeight = FLT_MAX;
	//	float terrainEditNorthSideZMinusMaxHeight = -FLT_MAX;
	//	
	//	size_t maxIndex = numVerticesPerSize - 1;

	//	for (size_t z = 0; z < numVerticesPerSize; z++)
	//	{
	//		for (size_t x = 0; x < numVerticesPerSize; x++)
	//		{
	//			float h = heights32Buffer._deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
	//			if (h < terrainEditMinHeight)
	//				terrainEditMinHeight = h;
	//			if (h > terrainEditMaxHeight)
	//				terrainEditMaxHeight = h;

	//			if (x == 0)
	//			{
	//				if (h < terrainEditWestSideXMinusMinHeight)
	//					terrainEditWestSideXMinusMinHeight = h;
	//				if (h > terrainEditWestSideXMinusMaxHeight)
	//					terrainEditWestSideXMinusMaxHeight = h;
	//			}

	//			if (z == 0)
	//			{
	//				if (h < terrainEditNorthSideZMinusMinHeight)
	//					terrainEditNorthSideZMinusMinHeight = h;
	//				if (h > terrainEditNorthSideZMinusMaxHeight)
	//					terrainEditNorthSideZMinusMaxHeight = h;
	//			}

	//			if (x == maxIndex)
	//			{
	//				if (h < terrainEditEastSideXPlusMinHeight)
	//					terrainEditEastSideXPlusMinHeight = h;
	//				if (h > terrainEditEastSideXPlusMaxHeight)
	//					terrainEditEastSideXPlusMaxHeight = h;
	//			}

	//			if (z == maxIndex)
	//			{
	//				if (h < terrainEditSouthSideZPlusMinHeight)
	//					terrainEditSouthSideZPlusMinHeight = h;
	//				if (h > terrainEditSouthSideZPlusMaxHeight)
	//					terrainEditSouthSideZPlusMaxHeight = h;
	//			}
	//		}
	//	}

	//	if (terrainEdit->minHeight != terrainEditMinHeight)
	//	{
	//		terrainEdit->minHeight = terrainEditMinHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->maxHeight != terrainEditMaxHeight)
	//	{
	//		terrainEdit->maxHeight = terrainEditMaxHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->eastSideXPlus.minHeight != terrainEditEastSideXPlusMinHeight)
	//	{
	//		terrainEdit->eastSideXPlus.minHeight = terrainEditEastSideXPlusMinHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->eastSideXPlus.maxHeight != terrainEditEastSideXPlusMaxHeight)
	//	{
	//		terrainEdit->eastSideXPlus.maxHeight = terrainEditEastSideXPlusMaxHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->westSideXMinus.minHeight != terrainEditWestSideXMinusMinHeight)
	//	{
	//		terrainEdit->westSideXMinus.minHeight = terrainEditWestSideXMinusMinHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->westSideXMinus.maxHeight != terrainEditWestSideXMinusMaxHeight)
	//	{
	//		terrainEdit->westSideXMinus.maxHeight = terrainEditWestSideXMinusMaxHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->southSideZPlus.minHeight != terrainEditSouthSideZPlusMinHeight)
	//	{
	//		terrainEdit->southSideZPlus.minHeight = terrainEditSouthSideZPlusMinHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->southSideZPlus.maxHeight != terrainEditSouthSideZPlusMaxHeight)
	//	{
	//		terrainEdit->southSideZPlus.maxHeight = terrainEditSouthSideZPlusMaxHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->northSideZMinus.minHeight != terrainEditNorthSideZMinusMinHeight)
	//	{
	//		terrainEdit->northSideZMinus.minHeight = terrainEditNorthSideZMinusMinHeight;
	//		updated = true;
	//	}
	//	if (terrainEdit->northSideZMinus.maxHeight != terrainEditNorthSideZMinusMaxHeight)
	//	{
	//		terrainEdit->northSideZMinus.maxHeight = terrainEditNorthSideZMinusMaxHeight;
	//		updated = true;
	//	}

	//	return updated;
	//}
 
	bool MeshCacheBuffer::blendQuadrantOnNorthSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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
			/*|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0 */ )
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

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		size_t maxIndex = numVerticesPerSize - 1;

		if (northData.heights32Buffer != nullptr && northTerrainEdit != nullptr /* && (terrainEdit->northSideZMinus.needBlend || northTerrainEdit->southSideZPlus.needBlend)*/)
		{
			// moving on north side: all the x values with z = 0 
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, 0, numVerticesPerSize) - northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, numVerticesToAdjustInEachQuadrant - 1, numVerticesPerSize)
								- northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - (numVerticesToAdjustInEachQuadrant - 1), numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					northData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int z = (int)numVerticesToAdjustInEachQuadrant - 1; z >= 0; z--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) - northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * z;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) > northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (z == 0 && heightSaved)
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = saveHeight;
						else
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) += increment;
						data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

						if (z == 0 && heightSaved)
							northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = saveHeight;
						else
							northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) -= increment;
						northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize));

						if (z == 1)
						{
							float h1 = data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
							float h2 = northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize);
							saveHeight = (h1 + h2) / 2;
							heightSaved = true;
						}

						//if (x == 0)
						//	PLOG_DEBUG << "x=" << std::to_string(x) << " z=" << std::to_string(z) << "/" << std::to_string(maxIndex - z) << " increment= " << std::to_string(increment) << " " << std::to_string(h) << " ==> " << std::to_string(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize)) << " " << std::to_string(hNorth) << " ==> " << std::to_string(northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize));
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

						float gapToFlatten1 = data.heights32Buffer->_deprecated_get_at_ref<float>(x, numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - data.heights32Buffer->_deprecated_get_at_ref<float>(x, 0, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int z = (int)numVerticesToAdjustInEachQuadrant; z >= 0; z--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= modifiedHeightStep1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) -= modifiedHeightStep2;
							northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(northData.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize));

							idx++;
						}
					}
				}
			}

			terrainEdit->northSideZMinus.needBlend_notused = false;
			northTerrainEdit->southSideZPlus.needBlend_notused = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnSouthSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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
			/*|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0*/)
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

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		size_t maxIndex = numVerticesPerSize - 1;

		if (southData.heights32Buffer != nullptr && southTerrainEdit != nullptr /* && (terrainEdit->southSideZPlus.needBlend || southTerrainEdit->northSideZMinus.needBlend)*/)
		{
			// moving on south side: all the x values with z = 0 
			for (size_t x = 0; x < numVerticesPerSize; x++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex, numVerticesPerSize) - southData.heights32Buffer->_deprecated_get_at_ref<float>(x, 0, numVerticesPerSize)) / 2;

				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - (numVerticesToAdjustInEachQuadrant - 1), numVerticesPerSize)
						- southData.heights32Buffer->_deprecated_get_at_ref<float>(x, numVerticesToAdjustInEachQuadrant - 1, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					southData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int z = (int)numVerticesToAdjustInEachQuadrant - 1; z >= 0; z--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) - southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * z;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) > southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (z == 0 && heightSaved)
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = saveHeight;
						else
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) += increment;
						data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize));

						if (z == 0 && heightSaved)
							southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = saveHeight;
						else
							southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= increment;
						southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

						if (z == 1)
						{
							float h1 = data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize);
							float h2 = southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
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

						float gapToFlatten1 = data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = southData.heights32Buffer->_deprecated_get_at_ref<float>(x, numVerticesToAdjustInEachQuadrant, numVerticesPerSize) - southData.heights32Buffer->_deprecated_get_at_ref<float>(x, 0, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int z = (int)numVerticesToAdjustInEachQuadrant; z >= 0; z--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) -= modifiedHeightStep1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, maxIndex - z, numVerticesPerSize));

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= modifiedHeightStep2;
							southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(southData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

							idx++;
						}
					}
				}
			}

			terrainEdit->southSideZPlus.needBlend_notused = false;
			southTerrainEdit->northSideZMinus.needBlend_notused = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnWestSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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
			/*|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0*/)
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

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		size_t maxIndex = numVerticesPerSize - 1;

		if (westData.heights32Buffer != nullptr && westTerrainEdit != nullptr /*&& (terrainEdit->westSideXMinus.needBlend || westTerrainEdit->eastSideXPlus.needBlend)*/)
		{
			// moving on west side: all the z values with x = 0 
			for (size_t z = 0; z < numVerticesPerSize; z++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(0, z, numVerticesPerSize) - westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, z, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->_deprecated_get_at_ref<float>(numVerticesToAdjustInEachQuadrant - 1, z, numVerticesPerSize)
						- westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - (numVerticesToAdjustInEachQuadrant - 1), z, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					westData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int x = (int)numVerticesToAdjustInEachQuadrant - 1; x >= 0; x--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) - westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * x;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) > westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (x == 0 && heightSaved)
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = saveHeight;
						else
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) += increment;
						data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

						if (x == 0 && heightSaved)
							westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = saveHeight;
						else
							westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) -= increment;
						westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize));

						if (x == 1)
						{
							float h1 = data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
							float h2 = westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize);
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

						float gapToFlatten1 = data.heights32Buffer->_deprecated_get_at_ref<float>(numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - data.heights32Buffer->_deprecated_get_at_ref<float>(0, z, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, z, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int x = (int)numVerticesToAdjustInEachQuadrant; x >= 0; x--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= modifiedHeightStep1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) -= modifiedHeightStep2;
							westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(westData.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize));

							idx++;
						}
					}
				}
			}

			terrainEdit->westSideXMinus.needBlend_notused = false;
			westTerrainEdit->eastSideXPlus.needBlend_notused = false;
		}

		return updated;
	}

	bool MeshCacheBuffer::blendQuadrantOnEastSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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
			/*|| data.heights16Buffer == nullptr || data.heights16Buffer->size() == 0*/)
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

		size_t numVerticesPerSize = pos.getNumVerticesPerSize();

		size_t maxIndex = numVerticesPerSize - 1;

		if (eastData.heights32Buffer != nullptr && eastTerrainEdit != nullptr /*&& (terrainEdit->eastSideXPlus.needBlend || eastTerrainEdit->westSideXMinus.needBlend)*/)
		{
			// moving on east side: all the z values with x = 0 
			for (size_t z = 0; z < numVerticesPerSize; z++)
			{
				float heightGapForEachQuandrantOnBorder = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, z, numVerticesPerSize) - eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, z, numVerticesPerSize)) / 2;
				size_t numVerticesToAdjustInEachQuadrant = (size_t)ceilf(heightGapForEachQuandrantOnBorder * terrainEdit->extraValues.amplitudeBlendFactor);
				if (numVerticesToAdjustInEachQuadrant > numVerticesPerSize)
					numVerticesToAdjustInEachQuadrant = numVerticesPerSize;

				if (numVerticesToAdjustInEachQuadrant > 0)
				{
					float increment = 0.0f;
					float averageStep = (abs(data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - (numVerticesToAdjustInEachQuadrant - 1), z, numVerticesPerSize)
						- eastData.heights32Buffer->_deprecated_get_at_ref<float>(numVerticesToAdjustInEachQuadrant - 1, z, numVerticesPerSize)) / 2) / numVerticesToAdjustInEachQuadrant;

					data.heightsUpdated = true;
					eastData.heightsUpdated = true;
					updated = true;

					float saveHeight = 0;
					bool heightSaved = false;
					for (int x = (int)numVerticesToAdjustInEachQuadrant - 1; x >= 0; x--)
					{
						float currentGapInEachQuadrant = abs(data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) - eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize)) / 2;
						float desideredGapInEachQuadrant = abs(averageStep) * x;
						if (currentGapInEachQuadrant > desideredGapInEachQuadrant)
						{
							increment = currentGapInEachQuadrant - desideredGapInEachQuadrant;
							if (data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) > eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize))
								increment = -increment;
						}
						else
							increment = 0.0f;

						if (x == 0 && heightSaved)
							data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = saveHeight;
						else
							data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) += increment;
						data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize));

						if (x == 0 && heightSaved)
							eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = saveHeight;
						else
							eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= increment;
						eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

						if (x == 1)
						{
							float h1 = data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize);
							float h2 = eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize);
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

						float gapToFlatten1 = data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex, z, numVerticesPerSize);
						float heightStep1 = gapToFlatten1 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor1 = heightStep1 * terrainEdit->extraValues.blendingBendingFactor;
						float gapToFlatten2 = eastData.heights32Buffer->_deprecated_get_at_ref<float>(numVerticesToAdjustInEachQuadrant, z, numVerticesPerSize) - eastData.heights32Buffer->_deprecated_get_at_ref<float>(0, z, numVerticesPerSize);
						float heightStep2 = gapToFlatten2 / numVerticesToAdjustInEachQuadrant;
						float heightStepFactor2 = heightStep2 * terrainEdit->extraValues.blendingBendingFactor;
						size_t idx = 0;
						for (int x = (int)numVerticesToAdjustInEachQuadrant; x >= 0; x--)
						{
							float modifiedHeightStep1 = heightStep1 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) -= modifiedHeightStep1;
							data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(data.heights32Buffer->_deprecated_get_at_ref<float>(maxIndex - x, z, numVerticesPerSize));

							float modifiedHeightStep2 = heightStep2 + sin(float(idx) * radiantStepPerVertex) * heightStepFactor2;
							eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) -= modifiedHeightStep2;
							eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize) = MeshCacheBuffer::adaptHeightToNewSavedQuadrantFormat(eastData.heights32Buffer->_deprecated_get_at_ref<float>(x, z, numVerticesPerSize));

							idx++;
						}
					}
				}
			}

			terrainEdit->eastSideXPlus.needBlend_notused = false;
			eastTerrainEdit->westSideXMinus.needBlend_notused = false;
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
	
	void Utils::deleteFile(std::string filePath)
	{
		remove(filePath.c_str());
	}

	void Utils::getFilesOrderedByLastAccessTime(std::string dirPath, std::string extension, std::map<std::time_t, std::string>& files)
	{
		files.clear();

		if (!fs::exists(dirPath))
			return;

		while (extension.starts_with("."))
			extension = extension.substr(1);

		for (const auto& entry : fs::directory_iterator(dirPath))
			if (entry.is_regular_file())
			{
				std::string filename = entry.path().filename().generic_string();
				std::string filenameExt = entry.path().filename().extension().generic_string();
				while (filenameExt.starts_with("."))
					filenameExt = filenameExt.substr(1);
				if (extension == filenameExt)
				{
					std::string filePath = dirPath + "\\" + filename;
					HANDLE hFile = CreateFile(convert(filePath).c_str(), FILE_GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						FILETIME ftAccess;
						if (GetFileTime(hFile, NULL, &ftAccess, NULL))
						{
							SYSTEMTIME stAccess;	FileTimeToSystemTime(&ftAccess, &stAccess);
							std::time_t t = SYSTEMTIME_utc_to_time_t_utc(stAccess);
							files[t] = filename;
						}
						CloseHandle(hFile);
					}
				}
			}
	}

	void Utils::deleteUnusedFiles(std::string dirPath, std::string extension, size_t notAccessedFor /* seconds */, size_t notMoreThan)
	{
		TheWorld_Utils::GuardProfiler profiler(std::string("MeshCacheBuffer freeOldFiles 1 ") + __FUNCTION__, "ALL");

		if (!fs::exists(dirPath))
			return;

		std::map<std::time_t, std::string> files;
		TheWorld_Utils::Utils::getFilesOrderedByLastAccessTime(dirPath, extension, files);

		size_t numFilesToEvaluate = files.size() - notMoreThan;
		
		std::vector<std::time_t> toDelete;
		for (auto it = files.rbegin(); it != files.rend() && numFilesToEvaluate > 0; ++it, --numFilesToEvaluate)
		{
			std::time_t currentTime = current_utc_time_t();
			size_t _notAccessedFor = currentTime - it->first;
			if (_notAccessedFor > notAccessedFor)
			{
				toDelete.push_back(it->first);
			}
		}

		for (auto& t : toDelete)
		{
			std::string fullPath = dirPath + "\\" + files[t];
			deleteFile(fullPath);
		}
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

	HeightMap::HeightMap(MemoryBuffer& float32Heights, size_t numVerticesPerSize)
	{
		my_assert(float32Heights.size() == numVerticesPerSize * numVerticesPerSize * sizeof(float));	// float32Heights size must match numVerticesPerSize
		my_assert(numVerticesPerSize % 2 == 1);															// numVerticesPerSize must be odd
		my_assert((numVerticesPerSize - 1) % 8 == 0);													// numVerticesPerSize - 1 must be multiple of 8: that means that the number of squares must be multiple of 8;

		m_numVerticesPerSize = numVerticesPerSize;
		set(float32Heights);
	}

	float HeightMap::getHeight(size_t row, size_t col) const
	{
		if (row >= m_numVerticesPerSize || col >= m_numVerticesPerSize)
			throw std::out_of_range("HeightMap::getHeight: row or col out of range");
		return get_at<float>(row, col, m_numVerticesPerSize);
	}

	//void HeightMap::setHeight(size_t row, size_t col, float height)	// TO BE TESTED
	//{
	//	if (row >= m_numVerticesPerSize || col >= m_numVerticesPerSize)
	//		throw std::out_of_range("HeightMap::getHeight: row or col out of range");
	//	get_at_ref<float>(row, col, m_numVerticesPerSize) = height;
	//}

	Level::Track::Square::Square()
	{
	}

	Level::Track::Square::Square(HeightMap& heightmap, size_t row, size_t col, float height)
	{
		if (row >= heightmap.getNumVerticesPerSize() - 1 || col >= heightmap.getNumVerticesPerSize() - 1)
			throw std::out_of_range("Level::Track::Square::Square: row or col out of range");
		
		m_type = getTypeFromHeightMap(heightmap, row, col, height);
	}

	Level::Track::Square::Type Level::Track::Square::getTypeFromHeightMap(HeightMap& heightmap, size_t row, size_t col, float height)
	{
		float h0 = heightmap.getHeight(row, col);					// vertex 0: upper left corner
		float h1 = heightmap.getHeight(row, col + 1);				// vertex 1: upper right corner
		float h2 = heightmap.getHeight(row + 1, col);				// vertex 2: lower left corner
		float h3 = heightmap.getHeight(row + 1, col + 1);			// vertex 3: lower right corner
		BYTE vertexMask = 0;
		if (h0 >= height) vertexMask |= 1;	// vertex 0
		if (h1 >= height) vertexMask |= 2;	// vertex 1
		if (h2 >= height) vertexMask |= 4;	// vertex 2
		if (h3 >= height) vertexMask |= 8;	// vertex 3
		if (vertexMask == 0)
			return Type::BelowHeight;
		else if (vertexMask == 15)
			return Type::AboveHeight;
		else
			return Type::IntersectedByHeight;
	}
	
	size_t Level::Track::Square::getIndexFromRowCol(HeightMap& heightmap, size_t row, size_t col)
	{
		return row * (heightmap.getNumVerticesPerSize() - 1) + col;
	}

	void Level::Track::Square::getRowColFromIndex(HeightMap& heightmap, size_t index, size_t& row, size_t& col)
	{
		row = index / (heightmap.getNumVerticesPerSize() - 1);
		col = index % (heightmap.getNumVerticesPerSize() - 1);
	}

	Level::Level()
	{
	}

	void Level::generateTracks(HeightMap& heightmap)
	{
		m_numSquaresPerSize = heightmap.getNumVerticesPerSize() - 1;

		std::vector<Track::Square> squares;	// 1 byte per square of vertices, each byte is a bitmask of the 4 vertices of the square, 1 if the vertex height is >= m_height, 0 otherwise
											// if the byte is 0, the square is below the height, if the byte is 15 (1111), the square is above the height, if the byte is between 1 and 14, the square is intersected by the height

		// map the heightmap to the squares to find the type of each square (above, below, intersected by height)
		squares.resize(m_numSquaresPerSize * m_numSquaresPerSize);
		for (size_t row = 0; row < m_numSquaresPerSize; row++)
		{
			for (size_t col = 0; col < m_numSquaresPerSize; col++)
			{
				Track::Square sq;
				squares[Track::Square::getIndexFromRowCol(heightmap, row, col)] = Track::Square(heightmap, row, col, m_height);
			}
		}

		// loook for all the tracks in the heightmap, a track is a sequence of squares that are intersected by the height, and that are connected to each other (8-connectivity)
		std::vector<Track> tracks;
		for (size_t row = 0; row < m_numSquaresPerSize; row++)
		{
			for (size_t col = 0; col < m_numSquaresPerSize; col++)
			{
				Track::Square& sq = squares[Track::Square::getIndexFromRowCol(heightmap, row, col)];
				if (sq.intersectedByHeight() && !sq.visited())
				{
					// Found a new track, do a depth-first search to find all the squares connected to this square that are intersected by the height
					Track track(this);
					track.addSquare(sq);
					sq.visited(true);
					
					// look for all the squares connected to this square that are intersected by the height
					std::vector<Track::Square*> stack;
					stack.push_back(&sq);
					while (not stack.empty())
					{
						Track::Square* current = stack.back();
						size_t currentIndex = current - &squares[0];
						stack.pop_back();
						size_t currentRow, currentCol;
						Track::Square::getRowColFromIndex(heightmap, currentIndex, currentRow, currentCol);
						for (int dr = -1; dr <= 1; dr++)
						{
							for (int dc = -1; dc <= 1; dc++)
							{
								if (dr == 0 && dc == 0)		// it's current square, skip it
									continue;
								
								size_t neighborRow = currentRow + dr;
								size_t neighborCol = currentCol + dc;
								if (neighborRow >= 0 and neighborRow < m_numSquaresPerSize and neighborCol >= 0 and neighborCol < m_numSquaresPerSize)
								{
									Track::Square& neighbor = squares[Track::Square::getIndexFromRowCol(heightmap, neighborRow, neighborCol)];
									if (neighbor.intersectedByHeight() && !neighbor.visited())
									{
										neighbor.visited(true);
										stack.push_back(&neighbor);
										track.addSquare(neighbor);
									}
								}
							}
						}
					}
					tracks.push_back(track);
				}
			}
		}
	}

	Level::Level(QuadrantPos& pos, float height)
	{
		m_height = height;
		m_quadrantPos = pos;
	}

	Level::Track::Track(Level* level)
	{
		m_level = level;
	}
}
