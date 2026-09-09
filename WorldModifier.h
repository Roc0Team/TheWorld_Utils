#pragma once

#include "Utils.h"

#include <string> 
#include <ctime>

namespace TheWorld_Utils
{
	class WorldModifier;
	
	enum class WMType
	{
		none = -1
		,elevator = 0
		,depressor = 1
		//,flattener = 2
	};

	class WorldModifierPos
	{
		friend WorldModifier;
	public:
		_declspec(dllexport) WorldModifierPos()
		{
			m_level = 0;
			m_posX = 0.0f;
			m_posZ = 0.0f;
			m_AOE = 0.0f;
			m_type = WMType::none;
		}

		_declspec(dllexport) WorldModifierPos(int level, float posX, float posZ, float AOE, enum class WMType type)
		{
			m_level = level;
			m_posX = posX;
			m_posZ = posZ;
			m_AOE = AOE;
			m_type = type;
		}

		_declspec(dllexport) WorldModifierPos(const WorldModifierPos& wmp)
		{
			*this = wmp;
		}

		void operator=(const WorldModifierPos& wmp)
		{
			m_level = wmp.m_level;
			m_posX = wmp.m_posX;
			m_posZ = wmp.m_posZ;
			m_AOE = wmp.m_AOE;
			m_type = wmp.m_type;
		}

		// needed to use an istance of WorldModifierPos as a key in a map (to keep the map sorted by m_posZ and by m_posX for equal m_posZ)
		bool operator<(const WorldModifierPos& p) const
		{
			if (m_level < p.m_level)
				return true;
			else if (m_level > p.m_level)
				return false;
			else
			{
				if (m_posZ < p.m_posZ)
					return true;
				else if (m_posZ > p.m_posZ)
					return false;
				else
				{
					if (m_posX < p.m_posX)
						return true;
					else if (m_posX > p.m_posX)
						return false;
					else
					{
						if (m_AOE < p.m_AOE)
							return true;
						else if (m_AOE > p.m_AOE)
							return false;
						else
							return m_type < p.m_type;
					}
				}
			}
		}

		_declspec(dllexport) int getLevel(void) const
		{
			return m_level;
		};
		_declspec(dllexport) float getPosX(void) const
		{
			return m_posX;
		};
		_declspec(dllexport) float getPosZ(void) const
		{
			return m_posZ;
		};
		_declspec(dllexport) float getAOE(void) const
		{
			return m_AOE;
		};
		_declspec(dllexport) enum class WMType getType(void) const
		{
			return m_type;
		};

		_declspec(dllexport) std::string idStr(void) const;

	private:
		int m_level;
		float m_posX;
		float m_posZ;
		float m_AOE;
		enum class WMType m_type;
	};

	class WorldModifier
	{
	public:
		_declspec(dllexport) WorldModifier();
		WorldModifier(const WorldModifier& wm)
		{
			*this = wm;
		}

		_declspec(dllexport) WorldModifier(WorldModifierPos& pos);
		_declspec(dllexport) ~WorldModifier();

		_declspec(dllexport) WorldModifierPos getPos(void) const
		{
			return m_pos;
		};

		_declspec(dllexport) float getRadius(void) const
		{
			return m_radius; 
		};
		_declspec(dllexport) float getAzimuth(void) const
		{
			return m_azimuth; 
		};
		_declspec(dllexport) float getAzimuthDegree(void) const
		{
			return m_azimuthDegree; 
		};

		void operator=(const WorldModifier& wm)
		{
			m_pos = wm.m_pos;
			m_radius = wm.m_radius;
			m_azimuth = wm.m_azimuth;
			m_azimuthDegree = wm.m_azimuthDegree;
		}

	private:
		WorldModifierPos m_pos;
		
		std::time_t m_timeStampUTC;

		float m_radius;
		float m_azimuth;
		float m_azimuthDegree;
	};
}


