#pragma once

//#ifdef _THEWORLD_CLIENT
//	#include <Godot.hpp>
//	#include <ImageTexture.hpp>
//	#include <PoolArrays.hpp>
//#endif

#include "WorldModifier.h"

#ifndef MYAPI 
#define MYAPI   __declspec( dllimport )
#endif 

#include "framework.h"
#include <cfloat>
#include <assert.h> 
#include <guiddef.h>
#include <string> 
#include <chrono>
#include <vector>
#include <queue>
#include <thread>
#include <map>
#include <mutex>
#include <functional>
#include <exception>
#include <filesystem>
#include <plog/Log.h>
#include "gsl\assert"

#pragma warning(push, 1)
#pragma warning (disable : 26495)
#include <Eigen/Dense>
#pragma warning(pop)

#define THEWORLD_SEA_LEVEL				0.0f
#define THEWORLD_SEA_MAX_WAVE_HEIGHT	30.0f
#define EMPTY_QUADRANTS_TERRAIN_HEIGHT	-500.0f
#define EMPTY_QUADRANT_TERRAIN_TEXTURE	"sand"
//#define EMPTY_QUADRANT_TERRAIN_TEXTURE	"Marble021_1K"
#define FLAT_QUADRANTS_TERRAIN_HEIGHT	(THEWORLD_SEA_LEVEL + THEWORLD_SEA_MAX_WAVE_HEIGHT + 1.0f)

#define THEWORLD_DEFAULT_AMPLITUDE_BLEND_FACTOR									2.0f

#define Vector3Zero godot::Vector3(0, 0, 0)
#define Vector3UP godot::Vector3(0, 1, 0)

#define Vector3X godot::Vector3(1, 0, 0)
#define Vector3Y godot::Vector3(0, 1, 0)
#define Vector3Z godot::Vector3(0, 0, 1)

namespace TheWorld_Utils
{
	// https://cellperformance.beyond3d.com/articles/2006/07/branchfree_implementation_of_h_1.html

	__declspec(dllexport) Eigen::Vector3d packNormal(Eigen::Vector3d normal);
	__declspec(dllexport) Eigen::Vector3d unpackNormal(Eigen::Vector3d packedNormal);
	__declspec(dllexport) void packNormal(float normalX, float normalY, float normalZ, float& packedNormalX, float& packedNormalY, float& packedNormalZ);
	__declspec(dllexport) void unpackNormal(float packedNormalX, float packedNormalY, float packedNormalZ, float& normalX, float& normalY, float& normalZ);

	__declspec(dllexport) double deg_to_rad(double deg);
	__declspec(dllexport) double rad_to_deg(double rad);

	__declspec(dllexport) void to_lower_implace(std::string& s);
	__declspec(dllexport) std::string to_lower(std::string s);
	__declspec(dllexport) inline std::wstring convert(const std::string& as);
	__declspec(dllexport) inline std::wstring convert(const std::string& as);

	__declspec(dllexport) std::chrono::system_clock::time_point current_utc_time_point(void);
	__declspec(dllexport) std::time_t current_utc_time_t(void);

	__declspec(dllexport) std::chrono::system_clock::time_point last_write_time_point_utc(const std::string& path);
	__declspec(dllexport) std::time_t last_write_time_t_utc(const std::string& path);
	__declspec(dllexport) std::string last_write_time_str_utc(const std::string& path);								// es: 2023-09-04 19:33:24.7026392
	
	__declspec(dllexport) void last_write_time_point_utc(const std::string& path, const std::chrono::system_clock::time_point time);
	__declspec(dllexport) void last_write_time_t_utc(const std::string& path, const std::time_t time);
	__declspec(dllexport) void last_write_time_str_utc(const std::string& path, const std::string& time /* es: 2023-09-04 19:33:24.7026392 */);

	__declspec(dllexport) std::chrono::system_clock::time_point time_t_to_time_point(const std::time_t time);
	__declspec(dllexport) std::time_t time_point_to_time_t(const std::chrono::system_clock::time_point time);

	__declspec(dllexport) std::chrono::system_clock::time_point str_time_to_time_point(const std::string& time);	// es: 2023-09-04 19:33:24.7026392
	__declspec(dllexport) std::string time_point_to_str_time(std::chrono::system_clock::time_point time);			// es: 2023-09-04 19:33:24.7026392
	__declspec(dllexport) std::time_t str_time_to_time_t(const std::string& time);									// es: 2023-09-04 19:33:24.7026392
	__declspec(dllexport) std::string time_t_to_str_time(std::time_t time, bool onlyseconds = true);				// es: 2023-09-04 19:33:24.7026392

	__declspec(dllexport) bool time_point_zero(std::chrono::system_clock::time_point tp);

	//std::vector<std::string> split(const std::string& s, char seperator)
	//{
	//	std::vector<std::string> output;

	//	std::string::size_type prev_pos = 0, pos = 0;

	//	while ((pos = s.find(seperator, pos)) != std::string::npos)
	//	{
	//		std::string substring(s.substr(prev_pos, pos - prev_pos));

	//		output.push_back(substring);

	//		prev_pos = ++pos;
	//	}

	//	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	//	return output;
	//}

	__declspec(dllexport) void padStrTo(std::string& str, const size_t num, const char paddingChar = ' ');
	template <typename T> __declspec(dllexport) std::string padValueTo(T val, const size_t num, const char paddingChar = '0');
	__declspec(dllexport) std::string padIntValueTo(int val, const size_t num, const char paddingChar = '0');

	class MapManagerProceduralGenerationStatus
	{
	public:
		//MapManagerProceduralGenerationStatus(void)
		//{
		//}
		//MapManagerProceduralGenerationStatus(const MapManagerProceduralGenerationStatus& o)
		//{


		//	*this = o;
		//}
		//void operator=(const MapManagerProceduralGenerationStatus& o)
		//{
		//	m_proceduralProcessingInProgress = o.m_proceduralProcessingInProgress;
		//	m_quadrantsWithHeigthsGenerated = o.m_quadrantsWithHeigthsGenerated;
		//	m_quadrantsToGenerateHeights = o.m_quadrantsToGenerateHeights;
		//	m_numBlendingCycles = o.m_numBlendingCycles;
		//	m_currentBlendingCycle = o.m_currentBlendingCycle;
		//	m_numBlendedQuadrants = o.m_numBlendedQuadrants;
		//	m_numQuadrantsToBlend = o.m_numQuadrantsToBlend;
		//}

	public:
		size_t m_numVerticesPerSize = 0;
		int m_level = 0;
		bool m_proceduralProcessingInProgress = false;
		size_t m_currentHeightGenCycle = 0;
		size_t m_quadrantsWithHeigthsGenerated = 0;
		size_t m_quadrantsToGenerateHeights = 0;
		size_t m_numBlendingCycles = 0;
		size_t m_currentBlendingCycle = 0;
		size_t m_numBlendedQuadrants = 0;
		size_t m_numQuadrantsToBlend = 0;
		size_t m_numQuadrantsToElabClientSide = 0;
	};
	
	class Exception : public std::exception
	{
	public:
		_declspec(dllexport) Exception(const char* function, const char* message = NULL, const char* message2 = NULL, int rc = 0)
		{
			m_exceptionName = "MapManagerException";
			if (message == NULL || strlen(message) == 0)
				m_message = "MapManager Generic Exception - C++ Exception";
			else
			{
				if (message2 == NULL || strlen(message2) == 0)
				{
					m_message = message;
				}
				else
				{
					m_message = message;
					m_message += " - ";
					m_message += message2;
					m_message += " - rc=";
					m_message += std::to_string(rc);
				}
			}
		};
		_declspec(dllexport) ~Exception() {};

		const char* what() const throw ()
		{
			return m_message.c_str();
		}

		const char* exceptionName()
		{
			return m_exceptionName.c_str();
		}
	private:
		std::string m_message;

	protected:
		std::string m_exceptionName;
	};

#define my_assert(cond) {if (!(cond)) {PLOG_DEBUG << "assert failed!";};	assert(cond);	if (!(cond)) {throw new TheWorld_Utils::Exception(__FUNCTION__, std::string(std::string("assert failed! Line number: ") + std::to_string(__LINE__)).c_str());};}

	typedef union FLOAT_32 FLOAT_32;
	union FLOAT_32
	{
		float    f32;
		uint32_t u32;
	};

	struct _RGB
	{
		BYTE r = 0;
		BYTE g = 0;
		BYTE b = 0;
	};

	struct _RGBA
	{
		BYTE r = 0;
		BYTE g = 0;
		BYTE b = 0;
		BYTE a = 0;
	};

	static constexpr float kPi = 3.14159265358979323846f;
	static constexpr float kPi2 = 6.28318530717958647692f;
	static constexpr float kEpsilon = 0.0001f;
	static constexpr float kAreaEpsilon = FLT_EPSILON;
	static constexpr float kNormalEpsilon = 0.001f;

	class MemoryBuffer
	{
	public:
		__declspec(dllexport) MemoryBuffer(void);
		__declspec(dllexport) MemoryBuffer(BYTE* in, size_t size);
		__declspec(dllexport) ~MemoryBuffer(void);
		__declspec(dllexport) void set(BYTE* in, size_t size);
		__declspec(dllexport) void append(BYTE* in, size_t size);
		__declspec(dllexport) void reserve(size_t size);
		__declspec(dllexport) size_t reserved(void) const;
		__declspec(dllexport) void reset(void);
		__declspec(dllexport) BYTE* ptr();
		__declspec(dllexport) size_t size(void) const;
		__declspec(dllexport) bool empty(void) const;
		__declspec(dllexport) void clear(std::string lastActor = "");
		__declspec(dllexport) void adjustSize(size_t size);
		__declspec(dllexport) void adjustCursorAndSize(size_t& cursor, size_t size);
		__declspec(dllexport) void copyFrom(MemoryBuffer& inBuffer, std::string lastActor);
		__declspec(dllexport) void populateFloatVector(std::vector<float>& v) const;
		__declspec(dllexport) void populateUint16Vector(std::vector<uint16_t>& v) const;
		__declspec(dllexport) void toString(std::string& buffer);
		__declspec(dllexport) void allocationSlotSize(size_t _allocationSlotSize);
		template <typename T> __declspec(dllexport) T& at(size_t idx)
		{
			T* ptr = (T*)m_ptr;
			ptr += idx;
			return *ptr;
		}
		template <typename T> __declspec(dllexport) T& at(size_t x, size_t y, size_t size)
		{
			T* ptr = (T*)m_ptr;
			ptr += ((y * size) + x);
			return *ptr;
		}
		template <typename T> __declspec(dllexport) T* at_ptr(size_t x, size_t y, size_t size)
		{
			T* ptr = (T*)m_ptr;
			ptr += ((y * size) + x);
			return ptr;
		}
		template <typename T> __declspec(dllexport) void populateVector(std::vector<T>& v)
		{
			size_t numElements = size() / sizeof(T);
			my_assert(size() == numElements * sizeof(T));
			v.assign((T*)m_ptr, (T*)m_ptr + numElements);
		}

	private:
		MemoryBuffer(const MemoryBuffer&);
		//void operator=(const MemoryBuffer&);

		BYTE* m_ptr;
		size_t m_size;
		size_t m_bufferSize;
		std::string m_lastActor;
		size_t m_allocationSlotSize = 100000;
	};

	class NoiseValues
	{
	public:
		int noiseType;
		int rotationType3D;
		int noiseSeed;
		float frequency;
		int fractalType;
		int fractalOctaves;
		float fractalLacunarity;
		float fractalGain;
		float fractalWeightedStrength;
		float fractalPingPongStrength;

		int cellularDistanceFunction;
		int cellularReturnType;
		float cellularJitter;

		int warpNoiseDomainWarpType;
		int warpNoiseRotationType3D;
		int warpNoiseSeed;
		float warpNoiseDomainWarpAmp;
		float warpNoiseFrequency;
		int warpNoieseFractalType;
		int warpNoiseFractalOctaves;
		float warpNoiseFractalLacunarity;
		float warpNoiseFractalGain;

		unsigned int heightsAmplitude;		// range of heights in WU (noise is from -1 to 1)
		float scaleFactor_JagIntensity;
		float desideredMinHeight;
		bool desideredMinHeigthMandatory;

		bool operator==(const NoiseValues& o) const
		{
			if (noiseType != o.noiseType)
				return false;
			if (rotationType3D != o.rotationType3D)
				return false;
			if (noiseSeed != o.noiseSeed)
				return false;
			if (frequency != o.frequency)
				return false;
			if (fractalType != o.fractalType)
				return false;
			if (fractalOctaves != o.fractalOctaves)
				return false;
			if (fractalLacunarity != o.fractalLacunarity)
				return false;
			if (fractalGain != o.fractalGain)
				return false;
			if (fractalWeightedStrength != o.fractalWeightedStrength)
				return false;
			if (fractalPingPongStrength != o.fractalPingPongStrength)
				return false;
			if (cellularDistanceFunction != o.cellularDistanceFunction)
				return false;
			if (cellularReturnType != o.cellularReturnType)
				return false;
			if (cellularJitter != o.cellularJitter)
				return false;
			if (warpNoiseDomainWarpType != o.warpNoiseDomainWarpType)
				return false;
			if (warpNoiseRotationType3D != o.warpNoiseRotationType3D)
				return false;
			if (warpNoiseSeed != o.warpNoiseSeed)
				return false;
			if (warpNoiseDomainWarpAmp != o.warpNoiseDomainWarpAmp)
				return false;
			if (warpNoiseFrequency != o.warpNoiseFrequency)
				return false;
			if (warpNoieseFractalType != o.warpNoieseFractalType)
				return false;
			if (warpNoiseFractalOctaves != o.warpNoiseFractalOctaves)
				return false;
			if (warpNoiseFractalLacunarity != o.warpNoiseFractalLacunarity)
				return false;
			if (warpNoiseFractalGain != o.warpNoiseFractalGain)
				return false;
			if (heightsAmplitude != o.heightsAmplitude)
				return false;
			if (scaleFactor_JagIntensity != o.scaleFactor_JagIntensity)
				return false;
			if (desideredMinHeight != o.desideredMinHeight)
				return false;
			if (desideredMinHeigthMandatory != o.desideredMinHeigthMandatory)
				return false;
			return true;
		}
	};

	class QuadrantPos
	{
	public:
		enum class DirectionSlot
		{
			Center = -1

			//
			// - o
			//
			, XMinus = 0
			, WestXMinus = 0

			//
			//   o -
			//
			, XPlus = 1
			, EastXPlus = 1

			//   -
			//   o
			//
			, ZMinus = 2
			, NorthZMinus = 2

			//
			//   o
			//   -
			, ZPlus = 3
			, SouthZPlus = 3

			// - 
			//   o
			//   
			, ZMinusXMinus = 4
			, NorthwestZMinusXMinus = 4

			//     - 
			//   o
			//   
			, ZMinusXPlus = 5
			, NortheastZMinusXPlus = 5

			//     
			//   o
			// -    
			, ZPlusXMinus = 6
			, SouthwestZPlusXMinus = 6

			//      
			//   o
			//     -
			, ZPlusXPlus = 7
			, SoutheastZPlusXPlus = 7
		};

		QuadrantPos()
		{
			reset();
		}

		void reset(void)
		{
			m_lowerXGridVertex = m_lowerZGridVertex = m_gridStepInWU = 0.0f;
			m_numVerticesPerSize = m_level = 0;
			m_sizeInWU = 0.0f;
			m_tag.clear();
			m_name.clear();
			m_initialized = false;
			m_quadrantPosX = m_quadrantPosZ = 0;
			m_centerX = m_centerZ = 0.0f;
		}

		bool empty(void) const
		{
			return m_initialized == false;
		}

		QuadrantPos(const QuadrantPos& quadrantPos)
		{
			*this = quadrantPos;
		}

		// x, z pos a point contained in the quadrant in global coord: calculating starting point of the quadrant in global coord
		QuadrantPos(float x, float z, int level, int numVerticesPerSize, float gridStepInWU)
		{
			m_numVerticesPerSize = numVerticesPerSize;
			m_gridStepInWU = gridStepInWU;
			m_sizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
			m_lowerXGridVertex = floor(x / m_sizeInWU) * m_sizeInWU;
			m_lowerZGridVertex = floor(z / m_sizeInWU) * m_sizeInWU;
			m_level = level;
			m_initialized = true;
			resetIndexPos();
			resetName();
			m_centerX = m_lowerXGridVertex + (m_sizeInWU / 2);
			m_centerZ = m_lowerZGridVertex + (m_sizeInWU / 2);
		}

		QuadrantPos(int x, int z, int level, int numVerticesPerSize, float gridStepInWU)
		{
			m_numVerticesPerSize = numVerticesPerSize;
			m_gridStepInWU = gridStepInWU;
			m_sizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
			m_lowerXGridVertex = x * m_sizeInWU;
			m_lowerZGridVertex = z * m_sizeInWU;
			m_level = level;
			m_initialized = true;
			resetIndexPos();
			resetName();
			m_centerX = m_lowerXGridVertex + (m_sizeInWU / 2);
			m_centerZ = m_lowerZGridVertex + (m_sizeInWU / 2);
		}

		__declspec(dllexport) void serialize(TheWorld_Utils::MemoryBuffer& buffer) const;
		__declspec(dllexport) void deserialize(TheWorld_Utils::MemoryBuffer& buffer);

		bool operator<(const QuadrantPos& quadrantPos) const
		{
			assert(m_initialized == true);
			assert(quadrantPos.m_initialized == true);
			assert(m_level == quadrantPos.m_level);
			if (m_level < quadrantPos.m_level)
				return true;
			if (m_level > quadrantPos.m_level)
				return false;
			// m_level == quadrantPos.m_level

			assert(m_sizeInWU == quadrantPos.m_sizeInWU);
			if (m_sizeInWU < quadrantPos.m_sizeInWU)
				return true;
			if (m_sizeInWU > quadrantPos.m_sizeInWU)
				return false;
			// m_sizeInWU == quadrantPos.m_sizeInWU

			if (m_lowerZGridVertex < quadrantPos.m_lowerZGridVertex)
				return true;
			if (m_lowerZGridVertex > quadrantPos.m_lowerZGridVertex)
				return false;
			// m_lowerZGridVertex == quadrantPos.m_lowerZGridVertex

			return m_lowerXGridVertex < quadrantPos.m_lowerXGridVertex;
		}

		bool operator==(const QuadrantPos& quadrantPos) const
		{
			if (m_lowerXGridVertex == quadrantPos.m_lowerXGridVertex
				&& m_lowerZGridVertex == quadrantPos.m_lowerZGridVertex
				&& m_numVerticesPerSize == quadrantPos.m_numVerticesPerSize
				&& m_level == quadrantPos.m_level
				&& m_gridStepInWU == quadrantPos.m_gridStepInWU)
				return true;
			else
				return false;
		}

		void operator=(const QuadrantPos& quadrantPos)
		{
			m_lowerXGridVertex = quadrantPos.m_lowerXGridVertex;
			m_lowerZGridVertex = quadrantPos.m_lowerZGridVertex;
			m_numVerticesPerSize = quadrantPos.m_numVerticesPerSize;
			m_level = quadrantPos.m_level;
			m_gridStepInWU = quadrantPos.m_gridStepInWU;
			m_sizeInWU = quadrantPos.m_sizeInWU;
			m_tag = quadrantPos.m_tag;
			m_name = quadrantPos.m_name;
			//m_initialized = true;
			m_initialized = quadrantPos.m_initialized;
			m_quadrantPosX = quadrantPos.m_quadrantPosX;
			m_quadrantPosZ = quadrantPos.m_quadrantPosZ;
			m_centerX = quadrantPos.m_centerX;
			m_centerZ = quadrantPos.m_centerZ;
			//return *this;
		}

		std::string getIdStr(void) const
		{
			return "ST" + std::to_string(m_gridStepInWU) + "_SZ" + std::to_string(m_numVerticesPerSize) + "_L" + std::to_string(m_level) + "_X" + std::to_string(m_lowerXGridVertex) + "_Z" + std::to_string(m_lowerZGridVertex);
		}

		float getLowerXGridVertex() const
		{
			return m_lowerXGridVertex;
		};
		float getLowerZGridVertex() const
		{
			return m_lowerZGridVertex;
		};
		int getNumVerticesPerSize() const
		{
			return m_numVerticesPerSize;
		};
		int getLevel() const
		{
			return m_level;
		};
		float getGridStepInWU()  const
		{
			return m_gridStepInWU;
		};
		float getSizeInWU() const
		{
			return m_sizeInWU;
		};
		int getIndexPosX(void) const
		{
			//float gridSizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
			//return (int)(m_lowerXGridVertex / gridSizeInWU);
			return m_quadrantPosX;
		}
		int getIndexPosZ(void) const
		{
			//float gridSizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
			//return (int)(m_lowerZGridVertex / gridSizeInWU);
			return m_quadrantPosZ;
		}
		_declspec(dllexport) QuadrantPos getQuadrantPos(enum class DirectionSlot dir, int numSlot = 1);
		void setTag(std::string tag)
		{
			m_tag = tag;
		}
		std::string getTag(void)
		{
			return m_tag;
		}
		_declspec(dllexport) size_t distanceInPerimeter(QuadrantPos& q) const;
		_declspec(dllexport) int distanceInPerimeter(QuadrantPos& fromQuadrant, QuadrantPos::DirectionSlot dir) const;
		bool isInitialized(void) const
		{
			return m_initialized;
		}

		std::string getName(void)
		{
			return m_name;
		}

		float getCenterX() const
		{
			return m_centerX;
		};
		float getCenterZ() const
		{
			return m_centerZ;
		};

	private:
		void resetIndexPos(void)
		{
			m_quadrantPosX = (int)(m_lowerXGridVertex / m_sizeInWU);
			m_quadrantPosZ = (int)(m_lowerZGridVertex / m_sizeInWU);
		}
		void resetName(void)
		{
			//float gridSizeInWU = (m_numVerticesPerSize - 1) * m_gridStepInWU;
			//m_name = "Quadrant-" + std::to_string((int)(m_lowerXGridVertex / gridSizeInWU)) + "-" + std::to_string((int)(m_lowerZGridVertex / gridSizeInWU));
			m_name = "Quadrant " + std::to_string((int)(m_lowerXGridVertex / m_sizeInWU)) + "," + std::to_string((int)(m_lowerZGridVertex / m_sizeInWU));
		}

	private:
		// ID
		float m_lowerXGridVertex;	// starting point in the X axis of the quadrant in global coord
		float m_lowerZGridVertex;	// starting point in the Z axis of the quadrant in global coord
		int m_numVerticesPerSize;
		int m_level;
		float m_gridStepInWU;
		// ID

		bool m_initialized;

		float m_sizeInWU;
		std::string m_tag;
		std::string m_name;
		int m_quadrantPosX;
		int m_quadrantPosZ;
		float m_centerX;
		float m_centerZ;
		// WARNING
		// WARNING: if you add member variables remember to update operator=, serialize, deserialize accordingly
		// WARNING
	};

	class MapTextureRectPos
	{
	public:
		__declspec(dllexport) MapTextureRectPos();
		__declspec(dllexport) MapTextureRectPos(int32_t idxX, int32_t idxY, int32_t level, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize);
		__declspec(dllexport) MapTextureRectPos(float globalPosX, float globalPosY, int32_t level, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize);
		__declspec(dllexport) MapTextureRectPos(TheWorld_Utils::QuadrantPos& quadrantPos, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize);
		__declspec(dllexport) ~MapTextureRectPos(void) {}

		float getGlobalLowerPosX() const
		{
			return m_globalLowerPosX;
		}
		float getGlobalLowerPosY() const
		{
			return m_globalLowerPosY;
		}
		float getGlobalHigherPosX() const
		{
			return m_globalLowerPosX + getGlobalSize();
		}
		float getGlobalHigherPosY() const
		{
			return m_globalLowerPosY + getGlobalSize();
		}
		int32_t getIdxX() const
		{
			return m_idxX;
		}
		int32_t getIdxY() const
		{
			return m_idxY;
		}
		int32_t getNumPixelsPerSize() const
		{
			return (int32_t)getGlobalSize() + 1;
		}
		float getGlobalSize() const
		{
			return m_numQuadrantsPerSize * m_quadrantGlobalSize;
		}
		float getWorldSize() const
		{
			return m_numQuadrantsPerSize * m_quadrantWorldSize;
		}
		float getQuadrantGlobalSize() const
		{
			return m_quadrantGlobalSize;
		}
		float getQuadrantWorldSize() const
		{
			return m_quadrantWorldSize;
		}
		int32_t getLevel() const
		{
			return m_level;
		}
		int32_t getNumQuadrantsPerSize() const
		{
			return m_numQuadrantsPerSize;
		}
		std::string getName() const
		{
			return m_name;
		}

		__declspec(dllexport) bool operator==(const MapTextureRectPos& textureRectPos) const;
		__declspec(dllexport) void operator=(const MapTextureRectPos& textureRectPos);
		__declspec(dllexport) MapTextureRectPos operator+(const MapTextureRectPos& textureRectPos) const;
		__declspec(dllexport) bool operator<(const MapTextureRectPos& textureRectPos) const;

		__declspec(dllexport) bool initialized(void) const;
		__declspec(dllexport) void globalPosToWorldPos(float globalPosX, float globalPosY, float& worldPosX, float& worldPosY) const;
		__declspec(dllexport) void worldPosToGlobalPos(float worldPosX, float worldPosY, float& globalPosX, float& globalPosY) const;
		__declspec(dllexport) void getAllContainedQuadrantPos(std::vector<TheWorld_Utils::QuadrantPos>& quadrantsPos, int worldViewerLevel, int numVerticesPerQuadrantSize, float gridStepInWU) const;
		__declspec(dllexport) bool getQuadrantRelativeIndices(const TheWorld_Utils::QuadrantPos& quadrantPos, int32_t& idxX, int32_t& idxY) const;
		__declspec(dllexport) void serialize(TheWorld_Utils::MemoryBuffer& buffer) const;
		__declspec(dllexport) void deserialize(TheWorld_Utils::MemoryBuffer& buffer);

	private:
		void reset(void);
		void internalSetName(void);

	private:
		float m_globalLowerPosX;
		float m_globalLowerPosY;
		int32_t m_idxX;
		int32_t m_idxY;
		int32_t m_level;
		int32_t m_numQuadrantsPerSize;
		float m_quadrantGlobalSize;
		float m_quadrantWorldSize;
		//float m_globalSize;
		//float m_worldSize;
		std::string m_name;
	};

	class TerrainSideInfo
	{
	public:
		bool needBlend;
		float minHeight;
		float maxHeight;
		bool operator==(const TerrainSideInfo& o) const
		{
			if (needBlend != o.needBlend)
				return false;
			if (minHeight != o.minHeight)
				return false;
			if (maxHeight != o.maxHeight)
				return false;
			return true;
		}
	};

	class TerrainEdit
	{
		class MeshCacheBuffer;

	public:
		__declspec(dllexport) static void generateGroundImage(std::string outdir, std::string groundTypeName, size_t imageSize, bool flipY, MemoryBuffer& colorImage, MemoryBuffer& bumpImage, MemoryBuffer& normalImage, MemoryBuffer& roughImage);
		__declspec(dllexport) static void generateGroundImage(MemoryBuffer& albedoBumpImage, MemoryBuffer& normalRoughnessImage, std::string groundTypeName, size_t imageSize, bool flipY, MemoryBuffer& colorImage, MemoryBuffer& bumpImage, MemoryBuffer& normalImage, MemoryBuffer& roughImage);

		class TerrainNeighbourQuadrantInfo
		{
		public:
			TerrainNeighbourQuadrantInfo(void)
			{
				memset(m_filler, 0, sizeof(m_filler));
			}
			~TerrainNeighbourQuadrantInfo(void)
			{
			}

			bool operator==(const TerrainNeighbourQuadrantInfo& o) const
			{
				if (m_minHeight != o.m_minHeight)
					return false;
				if (m_maxHeight != o.m_maxHeight)
					return false;
				return true;
			}

			float minHeight(void) const
			{
				return m_minHeight;
			}
			void minHeight(float f)
			{
				m_minHeight = f;
			}
			float maxHeight(void) const
			{
				return m_maxHeight;
			}
			void maxHeight(float f)
			{
				m_maxHeight = f;
			}

		private:
			float m_minHeight = FLT_MAX;
			float m_maxHeight = FLT_MIN;
			char m_filler[100];
		};

		enum class TextureType
		{
			lowElevation = 0
			, highElevation = 1
			, dirt = 2
			, rocks = 3
		};
		 
		enum class TerrainType
		{
			notset = 0
			, jagged_1 = 1
			, campaign_1 = 2
			, plateau_1 = 3
			, low_hills = 4
			, high_hills = 5
			, low_mountains = 6
			, low_mountains_grow = 7
			, high_mountains_1 = 8
			, high_mountains_1_grow = 9
			, high_mountains_2 = 10
			, high_mountains_2_grow = 11
			, noise_1 = 12
			, flat = 13
			, empty = 14
			, coastal = 15
			, sea_abyss_1 = 16
			, sea_abyss_2 = 17
			, sea_abyss_3 = 18
			, high_mountains_3 = 19
			, high_mountains_3_grow = 20
			, high_mountains_4 = 21
			, high_mountains_4_grow = 22
		};

		__declspec(dllexport) static std::string getDefaultTextureNameForTerrainType(enum class TerrainType terrainType, enum class TextureType textureType);

		size_t size;
		enum class TerrainType terrainType;
		bool needUploadToServer = false;
		bool heightsNeedRegen = false;
		bool normalsNeedRegen = false;
		bool splatmapNeedRegen = false;
		bool colormapNeedRegen = false;
		bool globalmapNeedRegen = false;

		float minHeight;
		float maxHeight;

		TerrainSideInfo eastSideXPlus;		// X growing
		TerrainSideInfo westSideXMinus;		// X lowering
		TerrainSideInfo southSideZPlus;		// Z growing
		TerrainSideInfo northSideZMinus;	// Z lowering

		NoiseValues noise;

		struct
		{
			char lowElevationTexName_r[51] = "";
			char highElevationTexName_g[51] = "";
			char dirtTexName_b[51] = "";
			char rocksTexName_a[51] = "";
			float slopeVerticalFactor = 4.0f;
			float slopeFlatFactor = 1.0f;
			float dirtOnRocksFactor = 2.0f;
			float highElevationFactor = 4.0f;
			float lowElevationFactor = 2.0f;
			size_t splatMapMode = 1;
			float amplitudeBlendFactor = THEWORLD_DEFAULT_AMPLITUDE_BLEND_FACTOR;
			float postGenerateElevation = 0;
			bool ignoreDesideredMinHeight = false;
			size_t numBlendingCycles = 2;
			float blendingBendingFactor = 1.0f;
			float minRandomBendingExtension = 0.2f;
			float maxRandomBendingExtension = 0.5f;
#define QUADRANT_SEA_INFO_SET_NOTSET	-1
#define QUADRANT_SEA_INFO_SET_TRUE		1
#define QUADRANT_SEA_INFO_SET_FALSE		0
			int32_t seaInfoSet = QUADRANT_SEA_INFO_SET_FALSE;		// used as bool
			int32_t canContainSea = QUADRANT_SEA_INFO_SET_FALSE;	// used as bool
		} extraValues;

#pragma pack (1)
		TerrainNeighbourQuadrantInfo eastNeighbourXPlus;		// X growing
		TerrainNeighbourQuadrantInfo westNeighbourXMinus;		// X lowering
		TerrainNeighbourQuadrantInfo southNeighbourZPlus;		// Z growing
		TerrainNeighbourQuadrantInfo northNeighbourZMinus;		// Z lowering

		struct
		{
			char m_filler[100];
		} extraValues1;
#pragma pack ()
		// WARNING: this struct can safely grow appending new data at the end (may be need an alignment filler to modify sizeof(extraValues))
		// remember to update operator== and DBJson (breakpoint in TerrainEdit::deserialize to verify in debug correct behavior)

		__declspec(dllexport) TerrainEdit(enum class TerrainEdit::TerrainType terrainType = TerrainEdit::TerrainType::empty);
		__declspec(dllexport) void setTextureNameForTerrainType(enum class TerrainEdit::TextureType textureType);
		__declspec(dllexport) void init(enum class TerrainEdit::TerrainType terrainType);
		__declspec(dllexport) void serialize(TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void deserialize(TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) static std::string terrainTypeString(enum class TerrainEdit::TerrainType terrainType);
		__declspec(dllexport) static enum class TerrainEdit::TerrainType terrainTypeEnum(std::string& terrainType);
		__declspec(dllexport) void adjustValues(TerrainEdit* northSideZMinus, TerrainEdit* southSideZPlus, TerrainEdit* westSideXMinus, TerrainEdit* eastSideXPlus);
		__declspec(dllexport) bool operator==(const TerrainEdit& terrainEdit) const;
		__declspec(dllexport) bool detectSeaInQuadrant(void);
		__declspec(dllexport) bool seaQuadrant(void) const;
	};

	template<> MYAPI void MemoryBuffer::populateVector<float>(std::vector<float>& v);

	template <class TimeT = std::chrono::milliseconds, class ClockT = std::chrono::steady_clock> class Timer
	{
		using timep_t = typename ClockT::time_point;
		timep_t _start = ClockT::now(), _end = {};

	public:
		Timer(std::string method = "", std::string headerMsg = "", bool tickMsg = false, bool tockMsg = false)
		{
			_counterEverStarted = false;
			_counterStarted = false;
			_headerMsg = headerMsg;
			_save_headerMsg = headerMsg;
			_method = method;
			_tickMsg = tickMsg;
			_tockMsg = tockMsg;
			_start = _end = ClockT::now();
		}

		~Timer()
		{
			if (_tockMsg)
				tock();
		}

		void tick()
		{
			_end = timep_t{};
			_start = ClockT::now();
			_counterStarted = true;
			_counterEverStarted = true;

			if (_tickMsg)
			{
				std::string method = _method;
				if (method.length() > 0)
					method += " ";
				std::string headerMsg = _headerMsg;
				if (headerMsg.length() > 0)
					headerMsg = "- " + headerMsg;
				else
					headerMsg = "-";
				PLOG_DEBUG << std::string("START ") + method + headerMsg;
			}
		}

		void tock()
		{
			if (_counterStarted)
			{
				_end = ClockT::now();
				_counterStarted = false;

				if (_tockMsg)
				{
					std::string method = _method;
					if (method.length() > 0)
						method += " ";
					std::string headerMsg = _headerMsg;
					if (headerMsg.length() > 0)
						headerMsg = "- " + headerMsg;
					else
						headerMsg = "-";
					PLOG_DEBUG << std::string("ELAPSED ") + method + headerMsg + " ==> " + std::to_string(duration().count()).c_str() + " ms";
				}
			}
		}

		bool counterEverStarted(void)
		{
			return _counterEverStarted;
		}
		
		void headerMsg(std::string headerMsg)
		{
			_headerMsg = headerMsg;
		}

		void headerMsgSuffix(std::string _headerMsgSuffix)
		{
			_headerMsg = _save_headerMsg + _headerMsgSuffix;
		}

		void method(std::string method)
		{
			_method = method;
		}

		bool counterStarted(void)
		{
			return _counterStarted;
		}

		template <class TT = TimeT>	TT partialDuration() const
		{
			return std::chrono::duration_cast<TT>(ClockT::now() - _start);
		}

		template <class TT = TimeT>	TT duration() const
		{
			Expects(_end != timep_t{} && "toc before reporting");
			//assert(_end != timep_t{} && "toc before reporting");
			return std::chrono::duration_cast<TT>(_end - _start);
		}

	private:
		bool _counterEverStarted;
		bool _counterStarted;
		std::string _headerMsg;
		std::string _save_headerMsg;
		std::string _method;
		bool _tickMsg;
		bool _tockMsg;

	};

#define TimerMs TheWorld_Utils::Timer<std::chrono::milliseconds, std::chrono::steady_clock>
#define TimerMcs TheWorld_Utils::Timer<std::chrono::microseconds, std::chrono::steady_clock>

	class GridVertex;

	class MeshCacheBuffer
	{
	public:
		class CacheQuadrantData
		{
		public:
			CacheQuadrantData(bool allocate = false)
			{
				if (allocate)
					allocateBuffers();
			}
			~CacheQuadrantData(void)
			{
				if (m_allocated)
					releaseBuffers();
			}
			void allocateBuffers()
			{
				releaseBuffers();

				m_allocated = true;
				terrainEditValues = new TheWorld_Utils::MemoryBuffer();
				heights16Buffer = new TheWorld_Utils::MemoryBuffer();
				heights32Buffer = new TheWorld_Utils::MemoryBuffer();
				normalsBuffer = new TheWorld_Utils::MemoryBuffer();
				splatmapBuffer = new TheWorld_Utils::MemoryBuffer();
				colormapBuffer = new TheWorld_Utils::MemoryBuffer();
				globalmapBuffer = new TheWorld_Utils::MemoryBuffer();
				m_terrainDataPtr = (TheWorld_Utils::TerrainEdit*)terrainEditValues->ptr();
			}
			void releaseBuffers()
			{
				if (m_allocated)
				{
					m_allocated = false;
					delete(terrainEditValues);
					terrainEditValues = nullptr;
					m_terrainDataPtr = nullptr;
					delete(heights16Buffer);
					heights16Buffer = nullptr;
					delete(heights32Buffer);
					heights32Buffer = nullptr;
					delete(normalsBuffer);
					normalsBuffer = nullptr;
					delete(splatmapBuffer);
					splatmapBuffer = nullptr;
					delete(colormapBuffer);
					colormapBuffer = nullptr;
					delete(globalmapBuffer);
					globalmapBuffer = nullptr;
				}
			}
			bool buffersAllocated(void) const
			{
				return m_allocated;
			}
			TheWorld_Utils::TerrainEdit* terrainData(void)
			{
				if (terrainEditValues == nullptr)
					m_terrainDataPtr = nullptr;
				else
					m_terrainDataPtr = (TheWorld_Utils::TerrainEdit*)terrainEditValues->ptr();
				
				return m_terrainDataPtr;
			}

		public:
			std::string meshId;
			TheWorld_Utils::MemoryBuffer* terrainEditValues = nullptr;
			float minHeight = 0.0f;
			float maxHeight = 0.0f;
			TheWorld_Utils::MemoryBuffer* heights16Buffer = nullptr;
			TheWorld_Utils::MemoryBuffer* heights32Buffer = nullptr;
			TheWorld_Utils::MemoryBuffer* normalsBuffer = nullptr;
			TheWorld_Utils::MemoryBuffer* splatmapBuffer = nullptr;
			TheWorld_Utils::MemoryBuffer* colormapBuffer = nullptr;
			TheWorld_Utils::MemoryBuffer* globalmapBuffer = nullptr;
			bool heightsUpdated = false;
			bool normalsUpdated = false;
			bool texturesUpdated = false;

		private:
			__declspec(dllexport) void operator=(const CacheQuadrantData& c)
			{
				throw std::exception("CacheQuadrantData cannot be assigned!");
			}

		private:
			bool m_allocated = false;
			//TheWorld_Utils::TerrainEdit m_terrainData;
			TheWorld_Utils::TerrainEdit* m_terrainDataPtr;
		};

	public:
		// char "0"
		// size_t	mesh id size
		// char*	mesh id
		// size_t	class TerrainEdit size
		// TerrainEdit deserialized
		// size_t num vertices
		//		float	min heigth		==> num vertices > 0
		//		float	man heigth		==> num vertices > 0
		//		float16 heightmap		(with size = num vertices * sizeof(uint16_t)) ==> num vertices > 0
		//		float32 heightmap		(with size = num vertices * sizeof(float)) ==> num vertices > 0
		//		RGB normalmap			(with size = num vertices * sizeof(RGB) oppure = sizeof(RGB) se il primo colore ha r=0, g=0, b=0) ==> num vertices > 0

		__declspec(dllexport) MeshCacheBuffer(void);
		__declspec(dllexport) ~MeshCacheBuffer(void);
		__declspec(dllexport) MeshCacheBuffer(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level, float lowerXGridVertex, float lowerZGridVertex);
		__declspec(dllexport) MeshCacheBuffer(const MeshCacheBuffer& c);
		__declspec(dllexport) MeshCacheBuffer(std::string& cacheDir, std::string& mapName, TheWorld_Utils::QuadrantPos& quadrantPos);

		__declspec(dllexport) void operator=(const MeshCacheBuffer& c);
		bool operator<(const MeshCacheBuffer& c) const;
		bool operator==(const MeshCacheBuffer& c) const;

		__declspec(dllexport) static uint16_t halfFromFloat(uint32_t f);
		__declspec(dllexport) static uint32_t halfToFloat(uint16_t f);

		__declspec(dllexport) TheWorld_Utils::QuadrantPos getQuadrantPos(void)
		{
			return TheWorld_Utils::QuadrantPos(m_lowerXGridVertex, m_lowerZGridVertex, m_level, (int)m_numVerticesPerSize, m_gridStepInWU);
		}
		
		__declspec(dllexport) static std::string cacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level);
		__declspec(dllexport) static std::string generateNewMeshId(void);
		__declspec(dllexport) static std::string generateNewMeshIdForEmptyBuffer(void);
		__declspec(dllexport) static bool isEmptyBuffer(std::string meshId);
		__declspec(dllexport) static bool firstMeshIdMoreRecent(std::string& firstMeshId, std::string& secondMeshId);
		__declspec(dllexport) static std::chrono::system_clock::time_point getTimeFromMeshId(std::string& meshId, std::string& meshIdWithoutBinaryTime);
		__declspec(dllexport) static std::string meshIdToPrint(std::string& meshId);
		__declspec(dllexport) static std::string meshIdToPrint1(std::string meshId);
		__declspec(dllexport) std::string getMeshIdFromDisk(bool workDir = false, TerrainEdit* terrainEdit = nullptr, bool updateLastAccessTime = false);
		__declspec(dllexport) static void getAllDiskCache(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level, std::map<TheWorld_Utils::QuadrantPos, TheWorld_Utils::MeshCacheBuffer>& diskCacheMap, std::time_t& fromFileTimeType, bool workDir = false);
		__declspec(dllexport) static void clearWorkCacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level);
		__declspec(dllexport) std::string getMeshIdStr(void);
		__declspec(dllexport) std::string getCacheIdStr(void);
		__declspec(dllexport) bool refreshMapsFromDisk(size_t numVerticesPerSize, float gridStepInWU, std::string meshId, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer);
		__declspec(dllexport) void refreshMapsFromBuffer(const BYTE* buffer, const size_t bufferSize, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache);
		__declspec(dllexport) void refreshMapsFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache);
		__declspec(dllexport) void refreshMapsFromBuffer(std::string& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, bool updateCache);
		__declspec(dllexport) void refreshCacheQuadrantDataFromBuffer(const BYTE* buffer, const size_t bufferSize, CacheQuadrantData& cacheQuadrantData, bool updateCache);
		__declspec(dllexport) void refreshCacheQuadrantDataFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache);
		__declspec(dllexport) void refreshCacheQuadrantDataFromBuffer(std::string& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache);
		__declspec(dllexport) void refreshInternalCacheQuadrantDataFromDisk(bool workDir = false, bool force = false, bool renewMeshId = false);
		__declspec(dllexport) CacheQuadrantData* internalCacheQuadrantData(void);
		__declspec(dllexport) void readBufferFromDisk(std::string meshId, TheWorld_Utils::MemoryBuffer& buffer, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) void readBufferFromDisk(std::string meshId, std::string& buffer, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) void writeBufferToDiskCache(std::string& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr) const;
		__declspec(dllexport) void writeBufferToDiskCache(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr) const;
		__declspec(dllexport) void writeBufferToDiskCache(const BYTE* buffer, const size_t bufferSize, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr) const;
		__declspec(dllexport) void setBufferFromHeights(std::string meshId, size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::MemoryBuffer& terrainEditValuesBuffer, std::vector<float>& vectGridHeights, std::string& buffer, float& minAltitude, float& maxAltitude, bool generateNormals);
		__declspec(dllexport) void setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, std::string& buffer);
		__declspec(dllexport) void setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, std::string& buffer);
		__declspec(dllexport) void generateHeightsWithNoise(size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights);
		__declspec(dllexport) void applyWorldModifier(int level, size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights, WorldModifier& wm);
		__declspec(dllexport) void generateNormalsForBlendedQuadrants(size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& east_float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& south_float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, bool& requestedExit);
		__declspec(dllexport) void generateSplatmap(size_t numVerticesPerSize, float gridStepInWU, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer,
													float slopeVerticalFactor = 4.0f, float slopeFlatFactor = 1.0f, float dirtOnRocksFactor = 2.0f, float highElevationFactor = 4.0f, float lowElevationFactor = 2.0f, size_t splatMapMode = 1);
		__declspec(dllexport) void deprecated_generateNormals(size_t numVerticesPerSize, float gridStepInWU, std::vector<float>& vectGridHeights, TheWorld_Utils::MemoryBuffer& normalsBuffer);
		__declspec(dllexport) void deprecated_generateNormals(size_t numVerticesPerSize, float gridStepInWU, std::vector<float>& vectGridHeights, BYTE* normalsBuffer, const size_t normalsBufferSize, size_t& usedBufferSize);
		__declspec(dllexport) void calcMinMaxHeight(size_t numVerticesPerSize, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& heights32Buffer);
		//__declspec(dllexport) bool checkBlendingQuadrantNeeded(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
		//	CacheQuadrantData& data,
		//	CacheQuadrantData& northData,
		//	CacheQuadrantData& southData,
		//	CacheQuadrantData& westData,
		//	CacheQuadrantData& eastData,
		//	CacheQuadrantData& northwestData,
		//	CacheQuadrantData& northeastData,
		//	CacheQuadrantData& southwestData,
		//	CacheQuadrantData& southeastData);
		__declspec(dllexport) bool blendQuadrant(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);

		//__declspec(dllexport) std::string getCacheDir()
		//{
		//	return m_cacheDir;
		//}

		__declspec(dllexport) std::string getMeshId(void)
		{
			return m_meshId;
		}
		__declspec(dllexport) void generateNewInternalMeshId(void)
		{
			m_meshId = generateNewMeshId();;
		}
		__declspec(dllexport) std::string getMapName(void)
		{
			return m_mapName;
		}
		__declspec(dllexport) float getGridStepInWU(void) const
		{
			return m_gridStepInWU;
		}
		__declspec(dllexport) size_t getNumVerticesPerSize(void) const
		{
			return m_numVerticesPerSize;
		}
		__declspec(dllexport) int getLevel(void) const
		{
			return m_level;
		}
		__declspec(dllexport) float getLowerXGridVertex(void) const
		{
			return m_lowerXGridVertex;
		}
		__declspec(dllexport) float getLowerZGridVertex(void) const
		{
			return m_lowerZGridVertex;
		}
		__declspec(dllexport) std::string getCacheFilePath(void) const
		{
			return m_cacheFilePath;
		}
		__declspec(dllexport) std::string getWorkCacheFilePath(void) const
		{
			return m_cacheWorkFilePath;
		}
		__declspec(dllexport) static std::string getWorkCacheDirPathStatic(std::string cacheDir)
		{
			return cacheDir + "\\Work\\";
		}

	private:
		__declspec(dllexport) bool blendBorders(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwestData,
			CacheQuadrantData& southeastData);
		__declspec(dllexport) bool blendQuadrantOnNorthSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		__declspec(dllexport) bool blendQuadrantOnSouthSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		__declspec(dllexport) bool blendQuadrantOnWestSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		__declspec(dllexport) bool blendQuadrantOnEastSide(size_t numVerticesPerSize, float gridStepInWU, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);

	private:
		std::string m_cacheFilePath;
		std::string m_cacheWorkFilePath;
		std::string m_cacheDir;
		std::string m_meshId;
		//std::string m_buffer;
		std::string m_mapName;
		float m_gridStepInWU;
		size_t m_numVerticesPerSize;
		int m_level;
		float m_lowerXGridVertex;
		float m_lowerZGridVertex;
		CacheQuadrantData m_internalCacheQuadrantData;
	};
	
	std::string ToString(GUID* guid);

	class Utils
	{
	public:
		__declspec(dllexport) static void plogInit(plog::Severity sev, plog::IAppender* appender);
		__declspec(dllexport) static void plogSetMaxSeverity(plog::Severity sev);
		__declspec(dllexport) static void plogDenit(void);

		static std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);

		static void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

		static bool isEqualWithLimitedPrecision(float num1, float num2, int precision)
		{
			float diff = abs(num1 - num2);

			float value = 0.0F;
			if (precision == 7)
				value = 0.0000001F;
			else if (precision == 6)
				value = 0.000001F;
			else if (precision == 5)
				value = 0.00001F;
			else if (precision == 4)
				value = 0.0001F;
			else if (precision == 3)
				value = 0.001F;
			else if (precision == 2)
				value = 0.01F;
			else if (precision == 1)
				value = 0.1F;

			if (diff < value)
				return true;

			return false;
		}

		static bool isEqualVectorWithLimitedPrecision(Eigen::Vector3d v1, Eigen::Vector3d v2, int precision)
		{
			if (isEqualWithLimitedPrecision((float)v1.x(), (float)v2.x(), precision) && isEqualWithLimitedPrecision((float)v1.y(), (float)v2.y(), precision) && isEqualWithLimitedPrecision((float)v1.z(), (float)v2.z(), precision))
				return true;
			else
				return false;
		}

		static int align(int x, int a)
		{
			return (x + a - 1) & ~(a - 1);
		}

		/*
		 * Case Insensitive String Comparision
		 */
		//static bool caseInSensStringEqual(std::string& str1, std::string& str2)
		//{
		//	return boost::iequals(str1, str2);
		//}

		/*
		 * Case Insensitive String Comparision
		 */
		//static bool caseInSensWStringEqual(std::wstring& str1, std::wstring& str2)
		//{
		//	return boost::iequals(str1, str2);
		//}

		static float square(float f) 
		{
			return f * f;
		}

		static bool isPowerOfTwo(int n)
		{
			if (n == 0)
				return false;

			return (ceil(log2(n)) == floor(log2(n)));
		}

		/** Return the next power of two.
		* @see http://graphics.stanford.edu/~seander/bithacks.html
		* @warning Behaviour for 0 is undefined.
		* @note isPowerOfTwo(x) == true -> nextPowerOfTwo(x) == x
		* @note nextPowerOfTwo(x) = 2 << log2(x-1)
		*/
		static uint32_t nextPowerOfTwo(uint32_t x) 
		{
			assert(x != 0);
			// On modern CPUs this is supposed to be as fast as using the bsr instruction.
			x--;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			return x + 1;
		}

		template <typename T>
		static T max2(const T& a, const T& b) 
		{
			return a > b ? a : b;
		}

		template <typename T>
		static T min2(const T& a, const T& b) 
		{
			return a < b ? a : b;
		}

		template <typename T>
		static T max3(const T& a, const T& b, const T& c) 
		{
			return max2(a, max2(b, c));
		}

		/// Return the maximum of the three arguments.
		template <typename T>
		static T min3(const T& a, const T& b, const T& c) 
		{
			return min2(a, min2(b, c));
		}

		/// Clamp between two values.
		template <typename T>
		static T clamp(const T& x, const T& a, const T& b) 
		{
			return Utils::min2(Utils::max2(x, a), b);
		}

		template <typename T>
		static void swap(T& a, T& b) 
		{
			T temp = a;
			a = b;
			b = temp;
		}

		union FloatUint32 {
			float f;
			uint32_t u;
		};

		static bool isFinite(float f) 
		{
			FloatUint32 fu(0);
			fu.f = f;
			return fu.u != 0x7F800000u && fu.u != 0x7F800001u;
		}

		static bool isNan(float f) 
		{
			return f != f;
		}

		static int ftoi_ceil(float val) 
		{
			return (int)ceilf(val);
		}

		static bool isZero(const float f, const float epsilon) 
		{
			return fabs(f) <= epsilon;
		}

		
		// Robust floating point comparisons:
		// http://realtimecollisiondetection.net/blog/?p=89
		//static bool equal(const float f0, const float f1, const float epsilon = 0.00001);
		static std::vector<std::string> split(std::string text, char delim)
		{
			std::string line;
			std::vector<std::string> vec;
			std::stringstream ss(text);
			while (std::getline(ss, line, delim)) {
				vec.push_back(line);
			}
			return vec;
		}
	
		static float getDistance(float x1, float y1, float x2, float y2)
		{
			return sqrtf((powf((x2 - x1), 2.0) + powf((y2 - y1), 2.0)));
		}
	};

	class GDN_TheWorld_Exception : public std::exception
	{
	public:
		_declspec(dllexport) GDN_TheWorld_Exception(const char* function, const char* message = NULL, const char* message2 = NULL, int rc = 0)
		{
			m_exceptionName = "MapManagerException";
			if (message == NULL || strlen(message) == 0)
				m_message = "MapManager Generic Exception - C++ Exception";
			else
			{
				if (message2 == NULL || strlen(message2) == 0)
				{
					m_message = message;
				}
				else
				{
					m_message = message;
					m_message += " - ";
					m_message += message2;
					m_message += " - rc=";
					m_message += std::to_string(rc);
				}
			}
		};
		_declspec(dllexport) ~GDN_TheWorld_Exception() {};

		const char* what() const throw ()
		{
			return m_message.c_str();
		}

		const char* exceptionName()
		{
			return m_exceptionName.c_str();
		}
	private:
		std::string m_message;

	protected:
		std::string m_exceptionName;
	};


	template <typename F>
	struct FinalAction {
		FinalAction(F f) : final_{ f } {}
		~FinalAction()
		{
			if (enabled_) final_(); 
		}
		void disable() 
		{
			enabled_ = false; 
		};
	private:
		F final_;
		bool enabled_{ true };
	};

	template <typename F> FinalAction<F> finally(F f)
	{
		return FinalAction<F>(f); 
	}

	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> MsTimePoint;

	class ThreadInitDeinit
	{
		friend class ThreadPool;
		virtual void threadInit(void) = 0;
		virtual void threadDeinit(void) = 0;
	};

	class ThreadPool
	{
	public:
		__declspec(dllexport) void Start(std::string label, size_t num_threads = 0, /*const std::function<void()>* threadInitFunction = nullptr, const std::function<void()>* threadDeinitFunction = nullptr,*/ ThreadInitDeinit* threadInitDeinit = nullptr);
		__declspec(dllexport) void QueueJob(const std::function<void()>& job);
		__declspec(dllexport) void Stop();
		__declspec(dllexport) bool busy();
		__declspec(dllexport) size_t getNumWorkingThreads(size_t& m_maxThreads);
		__declspec(dllexport) bool allThreadsWorking(void);

	private:
		void ThreadLoop();

		std::string m_label;
		bool m_should_terminate = false;           // Tells threads to stop looking for jobs
		size_t m_numWorkingThreads = 0;
		std::map<DWORD, size_t> m_workingThreads;
		std::mutex m_queue_mutex;                  // Prevents data races to the job queue
		std::mutex m_threadmap_mutex;                  // Prevents data races to the job queue
		std::condition_variable m_mutex_condition; // Allows threads to wait on new jobs or termination 
		std::vector<std::thread> m_threads;
		std::queue<std::function<void()>> m_jobs;
		//const std::function<void()>* m_threadInitFunction = nullptr;
		//const std::function<void()>* m_threadDeinitFunction = nullptr;
		ThreadInitDeinit* m_threadInitDeinit = nullptr;
		TheWorld_Utils::MsTimePoint m_lastDiagnosticTime = std::chrono::time_point_cast<TheWorld_Utils::MsTimePoint::duration>(std::chrono::system_clock::now());
		bool m_lastAllWorkingStatus = false;
	};

	template <typename T>
	void serializeToByteStream(T in, BYTE* stream, size_t& size)
	{
		T* pIn = &in;
		BYTE* pc = reinterpret_cast<BYTE*>(pIn);
		for (size_t i = 0; i < sizeof(T); i++)
		{
			stream[i] = *pc;
			pc++;
		}
		size = sizeof(T);
	}

	template <typename T>
	T deserializeFromByteStream(BYTE* stream, size_t& size)
	{
		T* pOut = reinterpret_cast<T*>(stream);
		size = sizeof(T);
		return *pOut;
	}

	/*static float roundToDigit(float num, int digit)
	{
		//float value = (int)(num * pow(10.0, digit) + .5);
		//float v = (float)value / (int)pow(10.0, digit);
		//return v;

		char str[40];
		sprintf(str, "%.6f", num);
		sscanf(str, "%f", &num);
		return num;
	}*/

	static bool equal(const float f0, const float f1, const float epsilon = 0.00001)
	{
		//return fabs(f0-f1) <= epsilon;
		return fabs(f0 - f1) <= epsilon * TheWorld_Utils::Utils::max3(1.0f, fabsf(f0), fabsf(f1));
	}
	
	//void X_aligned_memcpy_sse2(void* dest, const void* src, const unsigned long size)
	//{

	//	__asm
	//	{
	//		mov esi, src;    //src pointer
	//		mov edi, dest;   //dest pointer

	//		mov ebx, size;   //ebx is our counter 
	//		shr ebx, 7;      //divide by 128 (8 * 128bit registers)


	//	loop_copy:
	//		prefetchnta 128[ESI]; //SSE2 prefetch
	//		prefetchnta 160[ESI];
	//		prefetchnta 192[ESI];
	//		prefetchnta 224[ESI];

	//		movdqa xmm0, 0[ESI]; //move data from src to registers
	//		movdqa xmm1, 16[ESI];
	//		movdqa xmm2, 32[ESI];
	//		movdqa xmm3, 48[ESI];
	//		movdqa xmm4, 64[ESI];
	//		movdqa xmm5, 80[ESI];
	//		movdqa xmm6, 96[ESI];
	//		movdqa xmm7, 112[ESI];

	//		movntdq 0[EDI], xmm0; //move data from registers to dest
	//		movntdq 16[EDI], xmm1;
	//		movntdq 32[EDI], xmm2;
	//		movntdq 48[EDI], xmm3;
	//		movntdq 64[EDI], xmm4;
	//		movntdq 80[EDI], xmm5;
	//		movntdq 96[EDI], xmm6;
	//		movntdq 112[EDI], xmm7;

	//		add esi, 128;
	//		add edi, 128;
	//		dec ebx;

	//		jnz loop_copy; //loop please
	//	loop_copy_end:
	//	}
	//}
}

