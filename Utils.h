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
#include <variant>
#include <plog/Log.h>
#include "gsl\assert"

#pragma warning(push, 1)
#pragma warning (disable : 26495)
#include <Eigen/Dense>
#pragma warning(pop)

constexpr auto THEWORLD_QUADRANT_NAMEDATA_NORTHQUADSPLATMAPROW = "NorthQuadSplatmapRow";
constexpr auto THEWORLD_QUADRANT_NAMEDATA_WESTQUADSPLATMAPROW = "WestQuadSplatmapRow";
constexpr auto THEWORLD_QUADRANT_NAMEDATA_NORTHWESTQUADSPLATMAPTEXEL = "NorthWestQuadSplatmapTexel";

constexpr auto THEWORLD_QUADRANT_DISTANCE_FOR_BLENDING = 2;

#define THEWORLD_SEA_LEVEL				0.0f
#define THEWORLD_SEA_MAX_WAVE_HEIGHT	30.0f
#define EMPTY_QUADRANTS_TERRAIN_HEIGHT	-500.0f
#define EMPTY_QUADRANT_TERRAIN_TEXTURE	"beach_1"
#define UNKNOWN_QUADRANT_TERRAIN_TEXTURE	"beach_1"
//#define EMPTY_QUADRANT_TERRAIN_TEXTURE	"Marble021_1K"
#define FLAT_QUADRANTS_TERRAIN_HEIGHT	(THEWORLD_SEA_LEVEL + THEWORLD_SEA_MAX_WAVE_HEIGHT + 1.0f)

#define THEWORLD_DEFAULT_AMPLITUDE_BLEND_FACTOR									2.0f

#define Vector3Zero godot::Vector3(0, 0, 0)
#define Vector3UP godot::Vector3(0, 1, 0)

#define Vector3X godot::Vector3(1, 0, 0)
#define Vector3Y godot::Vector3(0, 1, 0)
#define Vector3Z godot::Vector3(0, 0, 1)

#define TRACK_QUADRANT_FOR_DEBUG(text, hash) trackQuadrantForDebug(__FUNCTION__, text, hash)

namespace TheWorld_Utils
{
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

	// https://cellperformance.beyond3d.com/articles/2006/07/branchfree_implementation_of_h_1.html

	__declspec(dllexport) Eigen::Vector3d packNormal(Eigen::Vector3d normal);
	__declspec(dllexport) Eigen::Vector3d unpackNormal(Eigen::Vector3d packedNormal);
	__declspec(dllexport) void packNormal(float normalX, float normalY, float normalZ, float& packedNormalX, float& packedNormalY, float& packedNormalZ);
	__declspec(dllexport) void unpackNormal(float packedNormalX, float packedNormalY, float packedNormalZ, float& normalX, float& normalY, float& normalZ);

	__declspec(dllexport) std::string getModuleLoadPath(void);

	__declspec(dllexport) double deg_to_rad(double deg);
	__declspec(dllexport) double rad_to_deg(double rad);

	__declspec(dllexport) void to_lower_implace(std::string& s);
	__declspec(dllexport) std::string to_lower(std::string s);
	__declspec(dllexport) inline std::wstring convert(const std::string& as);
	__declspec(dllexport) inline std::wstring convert(const std::string& as);

	__declspec(dllexport) std::chrono::system_clock::time_point current_utc_time_point(void);
	__declspec(dllexport) std::time_t current_utc_time_t(void);

	__declspec(dllexport) bool is_time_point_null(std::chrono::system_clock::time_point tp);
	__declspec(dllexport) bool null_time_point(std::chrono::system_clock::time_point tp);
	__declspec(dllexport) std::chrono::system_clock::time_point null_time_point(void);

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

	__declspec(dllexport) std::string milliseconds_to_hhhmmss(size_t milliseconds);

	// does not support negative values
	__declspec(dllexport) std::string quickAddThousandSeparators(std::string value, char thousandSep = ',');
	// support negative values
	__declspec(dllexport) std::string addThousandSeparators(std::string value, char thousandSep = ',', char decimalSep = '.', char sourceDecimalSep = '.');

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
		size_t m_quadrantsToProcessServerSide = 0;
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
		__declspec(dllexport) MemoryBuffer(const MemoryBuffer&);
		__declspec(dllexport) MemoryBuffer(const std::string&);
		__declspec(dllexport) ~MemoryBuffer(void);
		template <typename T> __declspec(dllexport) void init(size_t numElements)
		{
			size_t size = numElements * sizeof(T);
			reserve(size);
			m_size = size;
			memset(m_ptr, 0, size);
		}
		template <typename T> __declspec(dllexport) void init(size_t numElements, T& element)
		{
			size_t size = numElements * sizeof(T);
			reserve(size);
			m_size = size;
			T* ptr = (T*)m_ptr;
			for (size_t i = 0; i < numElements; i++)
			{
				ptr[i] = element;
			}
		}
		__declspec(dllexport) void getPart(MemoryBuffer& outBuffer, size_t offset, int64_t length = -1);

		__declspec(dllexport) void set(BYTE* in, size_t size);
		__declspec(dllexport) void setPart(MemoryBuffer& inBuffer, size_t offset);
		__declspec(dllexport) void setPart(BYTE* in, size_t size, size_t offset);
		__declspec(dllexport) void setPartOfRow(BYTE* in, size_t size, size_t row, size_t col, size_t rowSizeInBytes);	// z-based col/row expressed in bytes
		template <typename T> __declspec(dllexport) void setPartOfRow(T* in, size_t numElements, size_t row, size_t col, size_t numElementsPerRow)
		{
			// z-based col/row expressed in num elements
			size_t offset = (row * numElementsPerRow + col) * sizeof(T);
			setPart((BYTE*)in, numElements * sizeof(T), offset);
		}
		__declspec(dllexport) void append(BYTE* in, size_t size);
		__declspec(dllexport) void reserve(size_t size);
		template <typename T> __declspec(dllexport) void reserve(size_t numElements)
		{
			size_t size = numElements * sizeof(T);
			reserve(size);
		}
		__declspec(dllexport) size_t reserved(void) const;
		__declspec(dllexport) void reset(void);
		__declspec(dllexport) BYTE* ptr();
		__declspec(dllexport) size_t size(void) const;
		template <typename T> __declspec(dllexport) size_t size(void) const
		{
			float numElements = float(m_size) / sizeof(T);
			my_assert(float(m_size) == numElements * sizeof(T));
			return size_t(numElements);
		}
		__declspec(dllexport) bool empty(void) const;
		__declspec(dllexport) void clear(std::string lastActor = "");
		__declspec(dllexport) void adjustSize(size_t size);
		template <typename T> void adjustSize(size_t numElements)
		{
			size_t size = numElements * sizeof(T);
			adjustSize(size);
		}
		__declspec(dllexport) void adjustCursorAndSize(size_t& cursor, size_t size);
		__declspec(dllexport) void copyFrom(MemoryBuffer& inBuffer, std::string lastActor);
		__declspec(dllexport) void populateFloatVector(std::vector<float>& v) const;
		__declspec(dllexport) void populateUint16Vector(std::vector<uint16_t>& v) const;
		__declspec(dllexport) void toString(std::string& buffer);
		__declspec(dllexport) void fromString(std::string& buffer);
		__declspec(dllexport) void allocationSlotSize(size_t _allocationSlotSize);
		__declspec(dllexport) void operator=(const MemoryBuffer& o);
		__declspec(dllexport) void operator=(const std::string& o);
		__declspec(dllexport) bool operator==(const MemoryBuffer& o) const;
		template <typename T> __declspec(dllexport) void getNorthBorder(size_t numElementsPerBorder, MemoryBuffer& border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			border.set(ptr(), borderSize);
		}
		template <typename T> __declspec(dllexport) void getNorthBorder(size_t numElementsPerBorder, BYTE* border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			memcpy(border, ptr(), borderSize);
		}
		template <typename T> __declspec(dllexport) void getSouthBorder(size_t numElementsPerBorder, MemoryBuffer& border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			BYTE* offset = ptr() + borderSize * (numElementsPerBorder - 1);
			border.set(offset, borderSize);
		}
		template <typename T> __declspec(dllexport) void getSouthBorder(size_t numElementsPerBorder, BYTE* border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			BYTE* offset = ptr() + borderSize * (numElementsPerBorder - 1);
			memcpy(border, offset, borderSize);
		}
		template <typename T> __declspec(dllexport) void getWestBorder(size_t numElementsPerBorder, MemoryBuffer& border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			border.reserve(borderSize);
			border.adjustSize(borderSize);
			for (int idx = 0; idx < numElementsPerBorder; idx++)
			{
				T* element = get_at_ptr<T>(idx, 0, numElementsPerBorder);
				border.set_at_ref<T>(*element, idx, 0, numElementsPerBorder);
			}
		}
		template <typename T> __declspec(dllexport) void getWestBorder(size_t numElementsPerBorder, BYTE* border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			T* westBorderCursor = (T*)border;
			for (int idx = 0; idx < numElementsPerBorder; idx++)
			{
				T* element = get_at_ptr<T>(idx, 0, numElementsPerBorder);
				*westBorderCursor = *element;
				westBorderCursor++;
			}
		}
		template <typename T> __declspec(dllexport) void getEastBorder(size_t numElementsPerBorder, MemoryBuffer& border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			border.reserve(borderSize);
			border.adjustSize(borderSize);
			for (int idx = 0; idx < numElementsPerBorder; idx++)
			{
				T* element = get_at_ptr<T>(idx, numElementsPerBorder - 1, numElementsPerBorder);
				border.set_at_ref<T>(*element, idx, 0, numElementsPerBorder);
			}
		}
		template <typename T> __declspec(dllexport) void getEastBorder(size_t numElementsPerBorder, BYTE* border)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			T* eastBorderCursor = (T*)border;
			for (int idx = 0; idx < numElementsPerBorder; idx++)
			{
				T* element = get_at_ptr<T>(idx, numElementsPerBorder - 1, numElementsPerBorder);
				*eastBorderCursor = *element;
				eastBorderCursor++;
			}
		}
		template <typename T> __declspec(dllexport) void getBorders(size_t numElementsPerBorder, BYTE* northBorder, BYTE* southBorder, BYTE* westBorder, BYTE* eastBorder)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);

			// North Border Row
			//getNorthBorder<T>(numElementsPerBorder, northBorder);	// this is the same as the code below
			{
				memcpy(northBorder, ptr(), borderSize);
			}

			// South Border Row
			//getSouthBorder<T>(numElementsPerBorder, southBorder);	// this is the same as the code below
			{
				BYTE* offset = ptr() + borderSize * (numElementsPerBorder - 1);
				memcpy(southBorder, offset, borderSize);
			}

			// West Border Row
			//getWestBorder<T>(numElementsPerBorder, westBorder);	// this is the same as the code below
			{
				T* westBorderCursor = (T*)westBorder;
				for (int idx = 0; idx < numElementsPerBorder; idx++)
				{
					T* element = get_at_ptr<T>(idx, 0, numElementsPerBorder);
					*westBorderCursor = *element;
					westBorderCursor++;
				}
			}

			// East Border Row
			//getEastBorder<T>(numElementsPerBorder, eastBorder);	// this is the same as the code below
			{
				T* eastBorderCursor = (T*)eastBorder;
				for (int idx = 0; idx < numElementsPerBorder; idx++)
				{
					T* element = get_at_ptr<T>(idx, numElementsPerBorder - 1, numElementsPerBorder);
					*eastBorderCursor = *element;
					eastBorderCursor++;
				}
			}
		}
		template <typename T> __declspec(dllexport) void getBorders(size_t numElementsPerBorder, MemoryBuffer& borderBuffer) // first row upper border, second row lower border, third row left border, fourth row right border
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);
			borderBuffer.reserve(borderSize * 4);	// reserve for 4 borders (UP/NORTH,DOWN/SOUTH,LEFT/WEST,RIGHT/EAST)
			borderBuffer.adjustSize(borderSize * 4);
			getBorders<T>(numElementsPerBorder,
							borderBuffer.ptr() + (borderSize * 0),		// UP/NORTH border
							borderBuffer.ptr() + (borderSize * 1),		// DOWN/SOUTH border
							borderBuffer.ptr() + (borderSize * 2),		// LEFT/WEST border
							borderBuffer.ptr() + (borderSize * 3));		// RIGHT/EAST border
		}
		template <typename T> __declspec(dllexport) void getBorders(size_t numElementsPerBorder, MemoryBuffer& northUpBorder, MemoryBuffer& southDownBorder, MemoryBuffer& westLeftBorder, MemoryBuffer& eastRightBorder)
		{
			size_t borderSize = numElementsPerBorder * sizeof(T);

			// North/Up Border Row
			//getNorthBorder<T>(numElementsPerBorder, northUpBorder);	// this is the same as the code below
			{
				northUpBorder.set(ptr(), borderSize);
			}

			// South/Down Border Row
			//getSouthBorder<T>(numElementsPerBorder, southDownBorder);	// this is the same as the code below
			{
				BYTE* offset = ptr() + borderSize * (numElementsPerBorder - 1);
				southDownBorder.set(offset, borderSize);
			}

			// West/Left Border Row
			//getWestBorder<T>(numElementsPerBorder, westLeftBorder);	// this is the same as the code below
			{
				westLeftBorder.reserve(borderSize);
				westLeftBorder.adjustSize(borderSize);
				for (int idx = 0; idx < numElementsPerBorder; idx++)
				{
					T* element = get_at_ptr<T>(idx, 0, numElementsPerBorder);
					westLeftBorder.set_at_ref<T>(*element, idx, 0, numElementsPerBorder);
				}
			}

			// East/Right Border Row
			//getEastBorder<T>(numElementsPerBorder, eastRightBorder);	// this is the same as the code below
			{
				eastRightBorder.reserve(borderSize);
				eastRightBorder.adjustSize(borderSize);
				for (int idx = 0; idx < numElementsPerBorder; idx++)
				{
					T* element = get_at_ptr<T>(idx, numElementsPerBorder - 1, numElementsPerBorder);
					eastRightBorder.set_at_ref<T>(*element, idx, 0, numElementsPerBorder);
				}
			}
		}
		__declspec(dllexport) static size_t getPosXFromIndex(size_t idx, size_t numElementsPerRow)	// 0-based pos X from 0-base index in a row-based buffer
		{
			return idx % numElementsPerRow;
		}
		__declspec(dllexport) static size_t getPosYFromIndex(size_t idx, size_t numElementsPerRow)	// 0-based pos Y from 0-base index in a row-based buffer
		{
			return idx / numElementsPerRow;
		}
		template <typename T> __declspec(dllexport) T get_at(size_t idx)
		{
			// z-based idx
			return get_at_ref<T>(idx);
		}
		template <typename T> __declspec(dllexport) T& get_at_ref(size_t idx)
		{
			// z-based idx
			T* ptr = (T*)m_ptr;
			ptr += idx;
			return *ptr;
		}
		template <typename T> __declspec(dllexport) T get_at(size_t row, size_t col, size_t numElementsPerRow)
		{
			// z-based col/row
			return get_at_ref<T>(row, col, numElementsPerRow);
		}
		template <typename T> __declspec(dllexport) T& get_at_ref(size_t row, size_t col, size_t numElementsPerRow)
		{
			// z-based col/row
			T* ptr = (T*)m_ptr;
			ptr += ((row * numElementsPerRow) + col);
			return *ptr;
		}
		template <typename T> __declspec(dllexport) T& _deprecated_get_at_ref(size_t col, size_t row, size_t numElementsPerRow)		// deprecated: use get_at_ref instead with row and column in reversed order 
		{
			// z-based col/row
			T* ptr = (T*)m_ptr;
			ptr += ((row * numElementsPerRow) + col);
			return *ptr;
		}
		template <typename T> __declspec(dllexport) T* get_at_ptr(size_t row, size_t col, size_t numElementsPerRow)
		{
			// z-based col/row
			T* ptr = (T*)m_ptr;
			ptr += ((row * numElementsPerRow) + col);
			return ptr;
		}
		template <typename T> __declspec(dllexport) void populateVector(std::vector<T>& v)
		{
			size_t numElements = size() / sizeof(T);
			my_assert(size() == numElements * sizeof(T));
			v.assign((T*)m_ptr, (T*)m_ptr + numElements);
		}
		template <typename T> __declspec(dllexport) void set_at(T element, size_t row, size_t col, size_t numElementsPerRow)
		{
			// z-based col/row
			set_at_ref(element, col, row, numElementsPerRow);
		}
		template <typename T> __declspec(dllexport) void set_at_ref(T& element, size_t col, size_t row, size_t numElementsPerRow)
		{
			// z-based col/row
			T* ptr = (T*)m_ptr;
			ptr += ((row * numElementsPerRow) + col);
			*ptr = element;
			return;
		}

		template <typename T> __declspec(dllexport) void serializeToBufferAndTruncate(T in, size_t& offset)
		{
			serializeToBuffer<T>(in, offset, true);
		}

		template <typename T> __declspec(dllexport) void serializeToBuffer(T in, size_t& offset, bool truncate)
		{
			if (int64_t(offset) < 0)			// to append to the end of the buffer
				offset = size();
			
			reserve(offset + sizeof(T));		// to avoid memory overflow
			size_t size = 0;
			serializeToByteStream<T>(in, ptr() + offset, size);
			if (truncate)
				adjustCursorAndSize(offset, size);
			else
				offset += size;
		}
		__declspec(dllexport) void serializeToBufferAndTruncate(BYTE* in, size_t size, size_t& offset);
		__declspec(dllexport) void serializeToBuffer(BYTE* in, size_t size, size_t& offset, bool truncate);
		__declspec(dllexport) void serializeToBufferAndTruncate(MemoryBuffer& in, size_t& offset);
		__declspec(dllexport) void serializeToBuffer(MemoryBuffer& in, size_t& offset, bool truncate);
		__declspec(dllexport) void serializeToBufferAndTruncate(std::string& in, size_t& offset);
		__declspec(dllexport) void serializeToBuffer(std::string& in, size_t& offset, bool truncate);
		__declspec(dllexport) void serializeStringToBufferAndTruncate(std::string& s, size_t& offset);
		__declspec(dllexport) void serializeStringToBuffer(std::string& s, size_t& offset, bool truncate);
		__declspec(dllexport) void serializeMemoryBufferToBufferAndTruncate(MemoryBuffer& buffer, size_t& offset);
		__declspec(dllexport) void serializeMemoryBufferToBuffer(MemoryBuffer& buffer, size_t& offset, bool truncate);
		
		template <typename T> __declspec(dllexport) T deserializeFromBuffer(size_t& offset)
		{
			T out;
			size_t size = 0;
			out = deserializeFromByteStream<T>(ptr() + offset, size);
			offset += size;
			return out;
		}
		__declspec(dllexport) void deserializeFromBuffer(BYTE* out, size_t size, size_t& offset);
		__declspec(dllexport) std::string deserializeStringFromBuffer(size_t& offset);
		__declspec(dllexport) MemoryBuffer deserializeMemoryBufferFromBuffer(size_t& offset);

	private:

		BYTE* m_ptr = nullptr;
		size_t m_size = 0;
		size_t m_bufferSize = 0;
		std::string m_lastActor;
		size_t m_allocationSlotSize = 100000;
	};

	class NamedDataBuffer
	{
	public:
		typedef std::variant<std::monostate, int32_t, int64_t, size_t, float, double, std::string, MemoryBuffer> NamedDataBufferVariant;

		enum class Type
		{
			NotSet = -1
			, Unknown = 0
			, Int32	= 1		// int32_t
			, Int64	= 2		// int64_t
			, Size = 3		// size_t
			, Float	= 4		// float
			, Double = 5	// double
			, String = 6	// std::string
			, Buffer = 7	// TheWorld_Utils::MemoryBuffer
			, Group = 8		// TheWorld_Utils::MemoryBuffer
		};

		class NamedDataBufferData
		{
		public:
			NamedDataBufferData(std::string name, enum class Type type, int32_t flags, NamedDataBufferVariant v)
			{
				this->m_name = name;
				this->m_type = type;
				this->m_v = v;
				this->m_flags = flags;
			}
			NamedDataBufferData(void)
			{
			}
			NamedDataBufferData(const NamedDataBufferData& o)
			{
				*this = o;
			}

			void operator=(const NamedDataBufferData& o)
			{
				this->m_name = o.m_name;
				this->m_type = o.m_type;
				this->m_v = o.m_v;
				this->m_flags = o.m_flags;
			}

			bool empty(void)
			{
				return NamedDataBuffer::emptyVariant(m_v);
			}

			std::string name(void) const
			{
				return m_name;
			}
			int32_t flags(void) const
			{
				return m_flags;
			}
			enum class Type type(void) const
			{
				return m_type;
			}
			NamedDataBufferVariant& v(void)
			{
				return m_v;
			}
			template <typename T> T get(void)
			{
				return std::get<T>(m_v);
			}
			template <typename T> operator T() const
			{
				return std::get<T>(m_v);
			}
			//std::string toString(void) const	// useless sane as get<std::string>()
			//{
			//	std::string s = std::get<std::string>(m_v);
			//	return s;
			//}


		private:
			std::string m_name;
			int32_t m_flags = 0;
			enum class Type m_type = Type::NotSet;
			NamedDataBufferVariant m_v;
		};

	public:
		__declspec(dllexport) static bool emptyVariant(NamedDataBufferVariant& v);
		template <typename T> __declspec(dllexport) static bool correctType(NamedDataBufferVariant& v)
		{
			return std::holds_alternative<T>(v);
		}

		__declspec(dllexport) NamedDataBuffer(void);
		__declspec(dllexport) NamedDataBuffer(NamedDataBuffer& buffer);
		__declspec(dllexport) NamedDataBuffer(MemoryBuffer& buffer);
		__declspec(dllexport) ~NamedDataBuffer(void);

		__declspec(dllexport) void initialize(void);
		__declspec(dllexport) bool initialized(void) const;
		__declspec(dllexport) void deinitialize(void);
		__declspec(dllexport) bool deinitialized(void) const;
		__declspec(dllexport) void free(void);
		__declspec(dllexport) bool freed(void) const;
		__declspec(dllexport) void resetEmpty(void);

		__declspec(dllexport) size_t internalBufferSize(void) const;
		__declspec(dllexport) size_t memoryOccupation(void) const;

		__declspec(dllexport) size_t serializedSize(void);
		__declspec(dllexport) void serialize(BYTE* ptr, size_t& size, bool preSerializationDone = false);
		__declspec(dllexport) void serialize(MemoryBuffer& buffer);
		__declspec(dllexport) void deserialize(BYTE* ptr, size_t& size);
		__declspec(dllexport) void deserialize(MemoryBuffer& buffer);

		__declspec(dllexport) MemoryBuffer& getInternalBuffer(void);

		__declspec(dllexport) void operator=(NamedDataBuffer& o);
		//__declspec(dllexport) bool operator==(NamedDataBuffer& o);

		template <typename T> __declspec(dllexport) NamedDataBufferVariant getVariant(std::string name, int32_t& flags)
		{
			enum class Type type = getType<T>();
			if (type == Type::Unknown or type == Type::NotSet or type == Type::Group)
				throw new TheWorld_Utils::Exception(__FUNCTION__, "Unsupported type for getVariant");

			T element = T();
			bool found = false;
			if (type == Type::Buffer or type == Type::String)
				found = get(element, name, flags);
			else
				found = get<T>(element, name, flags);
			
			if (!found)
				return NamedDataBufferVariant();
			return element;
		}
		
		__declspec(dllexport) NamedDataBufferVariant getVariant(std::string name, int32_t& flags)
		{
			int64_t offset = find(name);
			if (offset < 0)
				return NamedDataBufferVariant();

			enum class Type type;
			size_t lenBuffer;
			getElementInfo(offset, name, type, lenBuffer, flags);
			if (type == Type::Unknown or type == Type::NotSet or type == Type::Group)
				throw new TheWorld_Utils::Exception(__FUNCTION__, "Unsupported type for getVariant");

			if (type == Type::String)
			{
				std::string element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if (type == Type::Buffer)
			{
				MemoryBuffer element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if(type == Type::Int32)
			{
				int32_t element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if (type == Type::Int64)
			{
				int64_t element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if (type == Type::Size)
			{
				size_t element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if (type == Type::Float)
			{
				float element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else if (type == Type::Double)
			{
				double element;
				bool found = get(element, name, flags);
				my_assert(found);
				return element;
			}
			else
				return NamedDataBufferVariant();
		}

		__declspec(dllexport) bool add(MemoryBuffer& buffer, std::string name, bool canReplace = true);
		__declspec(dllexport) bool add(std::string& s, std::string name, bool canReplace = true);
		template <typename T> __declspec(dllexport) bool add(T& element, std::string name, bool canReplace = true)
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
				bool added = groupDataBuffer.add<T>(element, name);
				if (added)
					added = addGroup(groupDataBuffer, groupName);
				return added;
			}

			enum class Type type = getType<T>();
			if (type == Type::Unknown or type == Type::NotSet or type == Type::Buffer or type == Type::String)
				return false;
			
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
				m_internalBuffer->serializeToBuffer<int32_t>(static_cast<int32_t>(type), _offset, false);	// new data type
				size_t offset = _offset;	// saving offset pointing to data size: offset is now pointing to the data size
				size_t oldDataSize = m_internalBuffer->deserializeFromBuffer<size_t>(_offset);	// get old data size to know how many bytes to remove from the buffer: _offset is now pointing to the data
				size_t newDataSize = sizeof(T);
				if (newDataSize == oldDataSize)
				{
					m_internalBuffer->serializeToBuffer<T>(element, _offset, false);	// replacing size and string with the new one of the same length
				}
				else if (newDataSize < oldDataSize)
				{
					m_internalBuffer->serializeToBuffer<size_t>(sizeof(T), offset, false);	// replacing new size of the buffer: after serialization offset will point to the start of the data 
					m_internalBuffer->serializeToBuffer<T>(element, offset, false);	// replacing old data with the new one of smaller length: after serialization offset will point to the end of the new data
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
					m_internalBuffer->serializeToBuffer<size_t>(sizeof(T), offset, false);	// replacing new size of the buffer: after serialization offset will point to the start of the data 
					m_internalBuffer->serializeToBuffer<T>(element, offset, false);	// replacing old data with the new one of greater length: after serialization offset will point to the end of the new data
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
				m_internalBuffer->serializeToBufferAndTruncate<size_t>(sizeof(T), offset);
				m_internalBuffer->serializeToBufferAndTruncate<T>(element, offset);
			}
			
			return true;
		}

		__declspec(dllexport) bool get(MemoryBuffer& buffer, std::string name, int32_t& flags);
		__declspec(dllexport) bool get(std::string& s, std::string name, int32_t& flags);
		template <typename T> __declspec(dllexport) bool get(T& element, std::string name, int32_t& flags)
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
				return groupDataBuffer.get<T>(element, name, flags);
			}

			int64_t _offset = find(name);
			if (_offset < 0)
				return false;

			size_t offset = size_t(_offset);
			
			std::string _name;
			enum class Type type;
			size_t lenBuffer;
			getElementInfo(offset, _name, type, lenBuffer, flags);
			
			if (lenBuffer != sizeof(T))
				return false;

			if (!isSameType<T>(type))
				return false;

			offset = offset + sizeof(size_t) + _name.size() + sizeof(int32_t) + sizeof(int32_t) + sizeof(size_t);	// skip name length, name, flags, type info and buffer length info to get to the buffer data
			
			element = m_internalBuffer->deserializeFromBuffer<T>(offset);
			
			return true;
		}

		//template <typename T> __declspec(dllexport) const T& operator[](std::string name)
		//{
		//	int32_t _flags = 0;
		//	return std::get<T>(getVariant<T>(name, _flags));
		//}
		__declspec(dllexport) const NamedDataBufferVariant operator[](std::string name)
		{
			int32_t _flags = 0;
			return getVariant(name, _flags);
		}

		__declspec(dllexport) bool addGroup(NamedDataBuffer& buffer, std::string name, bool canReplace = true);
		__declspec(dllexport) bool addGroup(MemoryBuffer& buffer, std::string name, bool canReplace = true);
		__declspec(dllexport) bool getGroup(NamedDataBuffer& buffer, std::string name, int32_t& flags);
		__declspec(dllexport) bool getGroup(MemoryBuffer& buffer, std::string name, int32_t& flags);

		__declspec(dllexport) bool remove(std::string name);

		__declspec(dllexport) bool exists(std::string name);
		template <typename T> __declspec(dllexport) bool exists(std::string name)
		{
			int64_t offset = find<T>(name);
			return offset >= 0;
		}

		__declspec(dllexport) void getNames(std::vector<std::string>& names);
		__declspec(dllexport) void getValues(std::vector<NamedDataBufferData>& values);
		__declspec(dllexport) void getValues(std::map<std::string, NamedDataBuffer::NamedDataBufferData>& values);

	private:
		__declspec(dllexport) void preSerialize(void);
		__declspec(dllexport) void preDeserialize(void);
		__declspec(dllexport) void attachInternalBuffer(MemoryBuffer& buffer);
		//__declspec(dllexport) void detachInternalBuffer(MemoryBuffer& buffer);
		__declspec(dllexport) bool exists(std::string name, int64_t& offset);
		__declspec(dllexport) void getElementInfo(size_t offset, std::string& name, enum class Type& type, size_t& lenBuffer, int32_t& flags);
		__declspec(dllexport) static std::string extractFirstGroup(std::string& name);
		__declspec(dllexport) bool addMemoryBuffer(MemoryBuffer& buffer, std::string name, bool canReplace, enum class Type type);
		__declspec(dllexport) bool getMemoryBuffer(MemoryBuffer& buffer, std::string name, enum class Type& type, int32_t& flags);
		__declspec(dllexport) void getNames(std::vector<std::string>& names, std::vector<enum class Type>& types);

		template <typename T> __declspec(dllexport) enum class Type getType(void)
		{
			if (std::is_same<T, int32_t>::value)
				return Type::Int32;
			if (std::is_same<T, int64_t>::value)
				return Type::Int64;
			if (std::is_same<T, size_t>::value)
				return Type::Size;
			if (std::is_same<T, float>::value)
				return Type::Float;
			if (std::is_same<T, double>::value)
				return Type::Double;
			if (std::is_same<T, std::string>::value)
				return Type::String;
			if (std::is_same<T, TheWorld_Utils::MemoryBuffer>::value)
				return Type::Buffer;
			return Type::Unknown;
		}
		
		template <typename T> __declspec(dllexport) bool isSameType(enum class Type type)
		{
			if (type == Type::Unknown || type == Type::NotSet)
				return false;
			if (type == Type::Int32 && std::is_same<T, int32_t>::value)
				return true;
			if (type == Type::Int64 && std::is_same<T, int64_t>::value)
				return true;
			if (type == Type::Size && std::is_same<T, size_t>::value)
				return true;
			if (type == Type::Float && std::is_same<T, float>::value)
				return true;
			if (type == Type::Double && std::is_same<T, double>::value)
				return true;
			if (type == Type::String && std::is_same<T, std::string>::value)
				return true;
			if (type == Type::Buffer && std::is_same<T, TheWorld_Utils::MemoryBuffer>::value)
				return true;
			if (type == Type::Group && std::is_same<T, TheWorld_Utils::MemoryBuffer>::value)
				return true;
			return false;
		}

		__declspec(dllexport) int64_t find(std::string name);
		template <typename T> __declspec(dllexport) int64_t find(std::string name)
		{
			int64_t offset = find(name);
			if (offset < 0)
				return offset;

			std::string _name;
			enum class Type type;
			size_t lenBuffer;
			int32_t flags;
			getElementInfo(size_t(offset), _name, type, lenBuffer, flags);
			
			if (isSameType<T>(type))
				return size_t(offset);
			
			return -1;
		}

	private:
		// adding data requires uodate of preSerialize/preDeserialize methods to include the new data in the serialization and deserialization process inside internal buffer as hidden datas (es: starting with "__")
		TheWorld_Utils::MemoryBuffer* m_internalBuffer = nullptr;
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
			, WestXMinus = 0
			, XMinus = 0

			//
			//   o -
			//
			, EastXPlus = 1
			, XPlus = 1

			//   -
			//   o
			//
			, NorthZMinus = 2
			, ZMinus = 2

			//
			//   o
			//   -
			, SouthZPlus = 3
			, ZPlus = 3

			// - 
			//   o
			//   
			, NorthwestZMinusXMinus = 4
			, ZMinusXMinus = 4

			//     - 
			//   o
			//   
			, NortheastZMinusXPlus = 5
			, ZMinusXPlus = 5

			//     
			//   o
			// -    
			, SouthwestZPlusXMinus = 6
			, ZPlusXMinus = 6

			//      
			//   o
			//     -
			, SoutheastZPlusXPlus = 7
			, ZPlusXPlus = 7
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
			return uninitialized();
		}

		bool uninitialized(void) const
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
			// we can compare only initialized quadrants with same size in WUs and num vertices per size
			assert(m_initialized == true);
			assert(quadrantPos.m_initialized == true);
			assert(m_sizeInWU == quadrantPos.m_sizeInWU);
			assert(m_numVerticesPerSize == quadrantPos.m_numVerticesPerSize);

			// quadrant pos with lower level value are less than quadrant pos with higher level value: using quandrant pos as index in a map force elements to be sorted by value from lower to higher
			if (m_level < quadrantPos.m_level)
				return true;
			if (m_level > quadrantPos.m_level)
				return false;
			// m_level == quadrantPos.m_level

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
			return "ST" + std::to_string(m_gridStepInWU) + "_SZ" + std::to_string(m_numVerticesPerSize) + "_L" + std::to_string(m_level) + "_X" + std::to_string(m_lowerXGridVertex) + "_Z" + std::to_string(m_lowerZGridVertex) + "_PosX" + std::to_string(m_quadrantPosX) + "_PosZ" + std::to_string(m_quadrantPosZ);
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
		_declspec(dllexport) QuadrantPos getQuadrantPos(enum class DirectionSlot dir, int numSlot = 1) const;
		_declspec(dllexport) void getNeighbours(std::vector<TheWorld_Utils::QuadrantPos>& neighbours, size_t maxDistance = 1) const;
		void setTag(std::string tag)
		{
			m_tag = tag;
		}
		std::string getTag(void) const
		{
			return m_tag;
		}
		_declspec(dllexport) size_t distanceInPerimeter(QuadrantPos& q) const;
		_declspec(dllexport) int distanceInPerimeter(QuadrantPos& fromQuadrant, QuadrantPos::DirectionSlot dir) const;
		bool isInitialized(void) const
		{
			return m_initialized;
		}

		_declspec(dllexport) bool isQuadrantTrackedForDebug(void) const;
		_declspec(dllexport) void trackQuadrantForDebug(std::string owner, std::string text, std::string hash) const;

		std::string getName(void) const
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

		__declspec(dllexport) static MapTextureRectPos getFromQuadrant(TheWorld_Utils::QuadrantPos& quadrantPos, int32_t numQuadrantsPerSize, float quadrantGlobalSize, float quadrantWorldSize);

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
		bool almostEqual(const TerrainSideInfo& o, float tolerance = 0.05) const
		{
			if (abs(minHeight - o.minHeight) > tolerance)
				return false;
			if (abs(maxHeight - o.maxHeight) > tolerance)
				return false;
			return true;
		}
	};

	class TerrainEdit
	{
		class MeshCacheBuffer;

	public:
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
			unknown = -1
			, notset = 0
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
			, desert_flat_1 = 23
			, desert_low_dunes = 24
			, desert_low_mountains = 25
			, desert_low_mountains_grow = 26
			, desert_high_mountains_1 = 27
			, desert_high_mountains_1_grow = 28
			// Warning: update maxTerrainTypeValue
		};
#define minTerrainTypeValue -1
#define maxTerrainTypeValue 28

		__declspec(dllexport) static std::string getDefaultTextureNameForTerrainType(enum class TerrainType terrainType, enum class TextureType textureType);
		__declspec(dllexport) static std::string getIconNameForTerrainType(enum class TerrainType terrainType);

		size_t size;
		enum class TerrainType terrainType;
		bool needUploadToServer = false;
		bool m_heightsNeedRegen = false;
		bool m_normalsNeedRegen_notUsed = false;
		bool m_splatmapNeedRegen_notUsed = false;
		bool m_colormapNeedRegen_notUsed = false;
		bool m_globalmapNeedRegen = false;

		float minHeight;
		float maxHeight;

		TerrainSideInfo eastSideXPlus;		// X growing
		TerrainSideInfo westSideXMinus;		// X lowering
		TerrainSideInfo southSideZPlus;		// Z growing
		TerrainSideInfo northSideZMinus;	// Z lowering

		NoiseValues noise;

		constexpr static float THEWORLD_SPLATMAP_HIGH_ELEVATION_FACTOR = 4.0f;
		constexpr static float THEWORLD_SPLATMAP_LOW_ELEVATION_FACTOR = 2.0f;
		constexpr static float THEWORLD_SPLATMAP_MIN_HEIGHT = 0.0f;
		constexpr static float THEWORLD_SPLATMAP_MAX_HEIGHT = 2000.0f;

		struct
		{
			char lowElevationTexName_r[51] = "";
			char highElevationTexName_g[51] = "";
			char dirtTexName_b[51] = "";
			char rocksTexName_a[51] = "";
			float slopeVerticalFactor = 4.0f;
			float slopeFlatFactor = 1.0f;
			float dirtOnRocksFactor = 2.0f;
			float highElevationFactor_not_used = THEWORLD_SPLATMAP_HIGH_ELEVATION_FACTOR;
			float lowElevationFactor_not_used = THEWORLD_SPLATMAP_LOW_ELEVATION_FACTOR;
			size_t splatMapMode = 1;
			float amplitudeBlendFactor = THEWORLD_DEFAULT_AMPLITUDE_BLEND_FACTOR;
			float postGenerateElevation = 0;
			bool ignoreDesideredMinHeight = false;
			size_t numBlendingCycles = 2;
			float blendingBendingFactor = 1.0f;
			float minRandomBendingExtension = 0.2f;
			float maxRandomBendingExtension = 0.5f;
#define QUADRANT_SEA_INFO_NOTSET	-1
#define QUADRANT_SEA_INFO_TRUE		1
#define QUADRANT_SEA_INFO_FALSE		0
			int32_t seaInfoSet = QUADRANT_SEA_INFO_FALSE;		// used as bool
			int32_t canContainSea = QUADRANT_SEA_INFO_FALSE;	// used as bool
		} extraValues;

//#pragma pack (push, 1)
		TerrainNeighbourQuadrantInfo deprecated_eastNeighbourXPlus;		// X growing	W A R N I N G: NOT USED AND DEPRECATED
		TerrainNeighbourQuadrantInfo deprecated_westNeighbourXMinus;	// X lowering	W A R N I N G: NOT USED AND DEPRECATED
		TerrainNeighbourQuadrantInfo deprecated_southNeighbourZPlus;	// Z growing	W A R N I N G: NOT USED AND DEPRECATED
		TerrainNeighbourQuadrantInfo deprecated_northNeighbourZMinus;	// Z lowering	W A R N I N G: NOT USED AND DEPRECATED

		struct
		{
			char filler1[100] = "";
			char filler2[4] = "";
			int32_t continentId = 0;
			char filler3[4] = "";
			// If globalmapNeedRegen is true following layers are the ones which will be used to cook the global map else are the ones used to cook actual global map
			struct
			{
				char lowElevationTexName_r[51] = "";
				char highElevationTexName_g[51] = "";
				char dirtTexName_b[51] = "";
				char rocksTexName_a[51] = "";
			} northGroundTextures;
			struct
			{
				char lowElevationTexName_r[51] = "";
				char highElevationTexName_g[51] = "";
				char dirtTexName_b[51] = "";
				char rocksTexName_a[51] = "";
			} westGroundTextures;
			struct
			{
				char lowElevationTexName_r[51] = "";
				char highElevationTexName_g[51] = "";
				char dirtTexName_b[51] = "";
				char rocksTexName_a[51] = "";
			} northwestGroundTextures;
		} extraValues1;
//#pragma pack (pop)
		// WARNING: this struct can safely grow appending new data at the end (may be need an alignment filler to modify sizeof(extraValues1))
		// remember to update operator== and DBJson (breakpoint in TerrainEdit::deserialize to verify in debug correct behavior)

		__declspec(dllexport) TerrainEdit(enum class TerrainEdit::TerrainType terrainType = TerrainEdit::TerrainType::empty);
		__declspec(dllexport) void setTextureNameForTerrainType(enum class TerrainEdit::TextureType textureType);
		__declspec(dllexport) void init(enum class TerrainEdit::TerrainType terrainType);
		__declspec(dllexport) void serialize(TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void deserialize(TheWorld_Utils::MemoryBuffer& buffer);
		void changeOldTextureNames(void);
		static std::string getNewTextureName(std::string name);
		__declspec(dllexport) static std::string terrainTypeString(enum class TerrainEdit::TerrainType terrainType);
		__declspec(dllexport) static enum class TerrainEdit::TerrainType terrainTypeEnum(std::string& terrainType);
		__declspec(dllexport) void adjustValues(TerrainEdit* northSideZMinus, TerrainEdit* southSideZPlus, TerrainEdit* westSideXMinus, TerrainEdit* eastSideXPlus);
		__declspec(dllexport) bool operator==(const TerrainEdit& terrainEdit) const;
		//__declspec(dllexport) bool detectSeaInQuadrant(void);
		__declspec(dllexport) bool canContainSea(void) const;
		__declspec(dllexport) void getGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const;
		__declspec(dllexport) void getNorthGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const;
		__declspec(dllexport) void setNorthGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName);
		__declspec(dllexport) void getWestGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const;
		__declspec(dllexport) void setWestGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName);
		__declspec(dllexport) void getNorthwestGroundTextures(std::string& lowElevationTexName, std::string& highElevationTexName, std::string& dirtTexName, std::string& rocksTexName) const;
		__declspec(dllexport) void setNorthwestGroundTextures(std::string lowElevationTexName, std::string highElevationTexName, std::string dirtTexName, std::string rocksTexName);
		void heightsNeedRegen(bool b)
		{
			if (b)
				m_heightsNeedRegen = b;
			else
				m_heightsNeedRegen = b;
		}
		bool heightsNeedRegen(void) const
		{
			return m_heightsNeedRegen;
		}
		//void normalsNeedRegen(bool b)
		//{
		//	if (b)
		//		m_normalsNeedRegen = b;
		//	else
		//		m_normalsNeedRegen = b;
		//}
		//bool normalsNeedRegen(void) const
		//{
		//	return m_normalsNeedRegen;
		//}
		//void splatmapNeedRegen(bool b)
		//{
		//	if (b)
		//		m_splatmapNeedRegen = b;
		//	else
		//		m_splatmapNeedRegen = b;
		//}
		//bool splatmapNeedRegen(void) const
		//{
		//	return m_splatmapNeedRegen;
		//}
		//void colormapNeedRegen(bool b)
		//{
		//	if (b)
		//		m_colormapNeedRegen = b;
		//	else
		//		m_colormapNeedRegen = b;
		//}
		//bool colormapNeedRegen(void) const
		//{
		//	return m_colormapNeedRegen;
		//}
		void globalmapNeedRegen(bool b)
		{
			if (b)
				m_globalmapNeedRegen = b;
			else
				m_globalmapNeedRegen = b;
		}
		bool globalmapNeedRegen(void) const
		{
			return m_globalmapNeedRegen;
		}
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
			if (_end == timep_t{})
				return std::chrono::duration_cast<TT>(ClockT::now() - _start);
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
#define TimerNans TheWorld_Utils::Timer<std::chrono::nanoseconds, std::chrono::steady_clock>

	class GridVertex;

	class MeshCacheBuffer
	{
	public:
		__declspec(dllexport) static bool cacheQuadrantFormat0(void);
		__declspec(dllexport) static bool cacheQuadrantFormat1(void);
		static bool s_cacheQuadrantFormat1;

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
				namedDataBuffer = new TheWorld_Utils::NamedDataBuffer();
			}
			void releaseBuffers()
			{
				if (m_allocated)
				{
					m_allocated = false;
					delete(terrainEditValues);
					terrainEditValues = nullptr;
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
					delete(namedDataBuffer);
					namedDataBuffer = nullptr;
				}
			}
			bool buffersAllocated(void) const
			{
				return m_allocated;
			}
			TheWorld_Utils::TerrainEdit* terrainData(void)
			{
				if (terrainEditValues == nullptr)
				{
					return nullptr;
				}
				else
				{
					return (TheWorld_Utils::TerrainEdit*)terrainEditValues->ptr();
				}
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
			TheWorld_Utils::NamedDataBuffer* namedDataBuffer = nullptr;
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

		__declspec(dllexport) static void calcNormal(TheWorld_Utils::MemoryBuffer& heightmap, size_t index, size_t numVerticesPerSize, float gridStepInWU, float& normalX, float& normalY, float& normalZ, float& h);
		__declspec(dllexport) static struct TheWorld_Utils::_RGBA calcSplat(TheWorld_Utils::MemoryBuffer& heightmap, size_t index, size_t numVerticesPerSize, float gridStepInWU, TerrainEdit* terrainEdit);

		__declspec(dllexport) static uint16_t halfFromFloat(float f);
		__declspec(dllexport) static uint16_t halfFromFloat(uint32_t f);
		__declspec(dllexport) static uint32_t halfToFloat(uint16_t f);
		__declspec(dllexport) static float halfToFloat_1(uint16_t f);

		__declspec(dllexport) TheWorld_Utils::QuadrantPos getQuadrantPos(void)
		{
			return TheWorld_Utils::QuadrantPos(m_lowerXGridVertex, m_lowerZGridVertex, m_level, (int)m_numVerticesPerSize, m_gridStepInWU);
		}
		
		__declspec(dllexport) static std::string cacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level);
		__declspec(dllexport) static std::string generateNewMeshId(void);
		__declspec(dllexport) static std::string generateNewMeshIdForEmptyBuffer(void);
		__declspec(dllexport) static bool isEmptyBuffer(std::string meshId);
		__declspec(dllexport) static bool firstMeshIdMoreRecent(std::string firstMeshId, std::string secondMeshId);
		__declspec(dllexport) static bool firstMeshIdMoreRecentRef(std::string& firstMeshId, std::string& secondMeshId);
		__declspec(dllexport) static std::chrono::system_clock::time_point getTimeFromMeshId(std::string& meshId, std::string& meshIdWithoutBinaryTime);
		__declspec(dllexport) static std::chrono::system_clock::time_point getTimeFromMeshId(std::string meshId);
		__declspec(dllexport) static std::string meshIdToPrint(std::string& meshId);
		__declspec(dllexport) static std::string meshIdToPrint1(std::string meshId);
		__declspec(dllexport) std::string getMeshIdFromDisk(bool workDir = false, TerrainEdit* terrainEdit = nullptr, bool updateLastAccessTime = false);
		__declspec(dllexport) void getTerrainEdit(TerrainEdit& terrainEdit, bool workDir = false, bool updateLastAccessTime = false);
		__declspec(dllexport) static void getAllDiskCache(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level, std::map<TheWorld_Utils::QuadrantPos, TheWorld_Utils::MeshCacheBuffer>& diskCacheMap, std::time_t& fromFileTimeType, bool workDir = false);
		__declspec(dllexport) static void clearWorkCacheDir(std::string cacheDir, std::string mapName, float gridStepInWU, size_t numVerticesPerSize, int level);
		__declspec(dllexport) std::string getMeshIdStr(void);
		__declspec(dllexport) std::string getCacheIdStr(void);
		__declspec(dllexport) bool refreshMapsFromDisk(size_t numVerticesPerSize, float gridStepInWU, std::string meshId, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshMapsFromBuffer(const BYTE* buffer, const size_t bufferSize, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshMapsFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshMapsFromBuffer(std::string& buffer, std::string& meshIdFromBuffer, TheWorld_Utils::MemoryBuffer& terrainEditValues, float& minAltitde, float& maxAltitude, TheWorld_Utils::MemoryBuffer& float16HeigthsBuffer, TheWorld_Utils::MemoryBuffer& float32HeigthsBuffer, TheWorld_Utils::MemoryBuffer& normalsBuffer, TheWorld_Utils::MemoryBuffer& splatmapBuffer, TheWorld_Utils::MemoryBuffer& colormapBuffer, TheWorld_Utils::MemoryBuffer& globalmapBuffer, TheWorld_Utils::NamedDataBuffer& namedDataBuffer, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshCacheQuadrantDataFromBuffer(const BYTE* buffer, const size_t bufferSize, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshCacheQuadrantDataFromBuffer(TheWorld_Utils::MemoryBuffer& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshCacheQuadrantDataFromBuffer(std::string& buffer, CacheQuadrantData& cacheQuadrantData, bool updateCache, bool overrideEmptyMaps);
		__declspec(dllexport) bool refreshInternalCacheQuadrantDataFromDisk(bool force, bool renewMeshId);
		__declspec(dllexport) bool refreshInternalCacheQuadrantDataFromDisk(bool workDir, bool force, bool renewMeshId);
		__declspec(dllexport) void refreshInternalCacheQuadrantDataFromBuffer(std::string& buffer, bool force = false);
		__declspec(dllexport) CacheQuadrantData* internalCacheQuadrantData(void);
		__declspec(dllexport) void readBufferFromDisk(std::string meshId, TheWorld_Utils::MemoryBuffer& buffer, bool workDir = false, TerrainEdit* terrainEdit = nullptr, bool onlyTerrainEdit = false);
		__declspec(dllexport) void readBufferFromDisk(std::string meshId, std::string& buffer, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) std::string writeBufferToDiskCache(std::string& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) std::string writeBufferToDiskCache2(std::string& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) std::string writeBufferToDiskCache(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) std::string writeBufferToDiskCache2(TheWorld_Utils::MemoryBuffer& buffer, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) std::string writeBufferToDiskCache(const BYTE* buffer, const size_t bufferSize, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr) ;
		__declspec(dllexport) std::string writeBufferToDiskCache2(const BYTE* buffer, const size_t bufferSize, std::string& meshId, bool renewMeshId = false, bool workDir = false, TerrainEdit* terrainEdit = nullptr);
		__declspec(dllexport) void setDummyBuffer(std::string meshId, std::string& buffer, TheWorld_Utils::TerrainEdit* terrainEdit);
		__declspec(dllexport) void setDummyBuffer_Format0(std::string meshId, std::string& buffer);
		__declspec(dllexport) void setDummyBuffer_Format1(std::string meshId, std::string& buffer, TheWorld_Utils::TerrainEdit* terrainEdit);
		__declspec(dllexport) void setBufferFromCacheQuadrantData_Format1(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void setBufferFromCacheQuadrantData(size_t numVerticesPerSize, CacheQuadrantData& cacheQuadrantData, std::string& buffer);
		__declspec(dllexport) void setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, TheWorld_Utils::MemoryBuffer& buffer);
		__declspec(dllexport) void setEmptyBuffer(size_t numVerticesPerSize, std::string& meshId, std::string& buffer);
		__declspec(dllexport) void generateHeightsWithNoise(size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights);
		__declspec(dllexport) void applyWorldModifier(int level, size_t numVerticesPerSize, float gridStepInWU, float lowerXGridVertex, float lowerZGridVertex, TerrainEdit* terrainEdit, std::vector<float>& vectGridHeights, WorldModifier& wm);
		static __declspec(dllexport) void calcMinMaxHeight(size_t numVerticesPerSize, TheWorld_Utils::TerrainEdit* terrainEdit, TheWorld_Utils::MemoryBuffer& heights32Buffer);
		static __declspec(dllexport) bool blendQuadrant(TheWorld_Utils::QuadrantPos pos, bool lastPhase, bool renewMeshIdOnUpdate,
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
		static __declspec(dllexport) bool blendBorders(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwestData,
			CacheQuadrantData& southeastData);
		static __declspec(dllexport) bool blendQuadrantOnNorthSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		static __declspec(dllexport) bool blendQuadrantOnSouthSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		static __declspec(dllexport) bool blendQuadrantOnWestSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
			CacheQuadrantData& data,
			CacheQuadrantData& northData,
			CacheQuadrantData& southData,
			CacheQuadrantData& westData,
			CacheQuadrantData& eastData,
			CacheQuadrantData& northwestData,
			CacheQuadrantData& northeastData,
			CacheQuadrantData& southwesthData,
			CacheQuadrantData& southeastData);
		static __declspec(dllexport) bool blendQuadrantOnEastSide(TheWorld_Utils::QuadrantPos& pos, bool lastPhase,
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
		__declspec(dllexport) static bool StringContains(std::string s, std::string search);

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

