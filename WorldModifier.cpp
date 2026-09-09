#include "pch.h"

#define _USE_MATH_DEFINES // for C++

#include "math.h"
#include "assert.h"
#include "WorldModifier.h"
#include <Rpc.h>

namespace TheWorld_Utils
{
	std::string WorldModifierPos::idStr(void) const
	{
		//GUID newGuid;
		//RPC_STATUS ret_val = ::UuidCreate(&newGuid);
		//if (ret_val != RPC_S_OK)
		//{
		//	std::string msg = "UuidCreate in error with rc " + std::to_string(ret_val);
		//	PLOG_ERROR << msg;
		//	throw(GDN_TheWorld_Exception(__FUNCTION__, msg.c_str()));
		//}
		//char bufferGuid[37];
		//sprintf_s(bufferGuid, sizeof(bufferGuid),
		//	"%08lX-%04X-%04X-%04X-%012llX",
		//	newGuid.Data1, newGuid.Data2, newGuid.Data3,
		//	((newGuid.Data4[0] << 8) | newGuid.Data4[1]),
		//	((unsigned long long)newGuid.Data4[2] << 40) |
		//	((unsigned long long)newGuid.Data4[3] << 32) |
		//	((unsigned long long)newGuid.Data4[4] << 24) |
		//	((unsigned long long)newGuid.Data4[5] << 16) |
		//	((unsigned long long)newGuid.Data4[6] << 8) |
		//	((unsigned long long)newGuid.Data4[7]));

		//std::chrono::system_clock::time_point now = current_utc_time_point();
		//std::time_t now_time_t_utc = std::chrono::system_clock::to_time_t(now);
		//std::string now_str = TheWorld_Utils::time_t_to_str_time(now_time_t_utc);
		//std::replace(now_str.begin(), now_str.end(), ' ', '_');
		//std::replace(now_str.begin(), now_str.end(), ':', '-');

		//std::string str = std::format("{}", now);						// DEBUG
		//BYTE shortBuffer[256 + 1];
		//size_t size = 0;
		//TheWorld_Utils::serializeToByteStream<std::chrono::system_clock::time_point>(now, shortBuffer, size);

		//std::string newId = std::string(bufferGuid)
		//	+ ":" + now_str;
		//+ ":" + std::string((char*)shortBuffer, size);

		std::string id = "L" + std::to_string(m_level) + "_" + "X" + std::to_string(m_posX) + "_" + "Z" + std::to_string(m_posZ) + "_" + "E" + std::to_string(m_AOE) + "_" + "T" + std::to_string((int)m_type);
		
		return id;
	}

	WorldModifier::WorldModifier()
	{
		m_radius = 0.0;
		m_azimuth = 0.0;
		m_azimuthDegree = 0.0;
		m_timeStampUTC = std::time(nullptr);
	}

	WorldModifier::WorldModifier(WorldModifierPos& pos) : m_pos(pos)
	{
		m_radius = sqrtf(powf(m_pos.getPosX(), 2.0) + powf(m_pos.getPosZ(), 2.0));
		if ((m_pos.getPosX() == 0 && m_pos.getPosZ() == 0) || m_radius == 0)
		{
			m_radius = 0;
			m_azimuth = 0;
			m_azimuthDegree = 0;
		}
		else
		{
			//angle of radius with x-axis (complementar of 2PI if Z < 0)
			m_azimuth = acosf(m_pos.getPosX() / m_radius);
			if (m_pos.getPosZ() < 0)
				m_azimuth = float(M_PI) * (float)2.0 - m_azimuth;
			m_azimuthDegree = (m_azimuth * 180) / float(M_PI);
		}
		std::chrono::system_clock::time_point now = current_utc_time_point();
		m_timeStampUTC = std::chrono::system_clock::to_time_t(now);
	}

	WorldModifier::~WorldModifier()
	{
	}
}