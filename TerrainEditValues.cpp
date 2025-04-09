#include "TerrainEditValues.h"
#include "WorldModifier.h"
#include "FastNoiseLite.h"
#include "Utils.h"

namespace TheWorld_Utils
{
	std::string TerrainEdit::getIconNameForTerrainType(enum class TerrainType terrainType)
	{
		std::string iconName = "";

		switch (terrainType)
		{
		case TerrainType::notset:
			break;
		case TerrainType::jagged_1:
			break;
		case TerrainType::campaign_1:
			iconName = "campaign_1_a.png";
			break;
		case TerrainType::plateau_1:
			iconName = "plateau_1_a.png";
			break;
		case TerrainType::low_hills:
			iconName = "low_hills_a.png";
			break;
		case TerrainType::high_hills:
			iconName = "high_hills_a.png";
			break;
		case TerrainType::low_mountains:
			iconName = "low_mountains_a.png";
			break;
		case TerrainType::low_mountains_grow:
			iconName = "low_mountains_a.png";
			break;
		case TerrainType::high_mountains_1:
			iconName = "high_mountains_1_a.png";
			break;
		case TerrainType::high_mountains_1_grow:
			iconName = "high_mountains_1_a.png";
			break;
		case TerrainType::high_mountains_2:
			iconName = "high_mountains_2_a.png";
			break;
		case TerrainType::high_mountains_2_grow:
			iconName = "high_mountains_2_a.png";
			break;
		case TerrainType::high_mountains_3:
			iconName = "high_mountains_3_a.png";
			break;
		case TerrainType::high_mountains_3_grow:
			iconName = "high_mountains_3_a.png";
			break;
		case TerrainType::high_mountains_4:
			iconName = "high_mountains_4_a.png";
			break;
		case TerrainType::high_mountains_4_grow:
			iconName = "high_mountains_4_a.png";
			break;
		case TerrainType::noise_1:
			break;
		case TerrainType::flat:
			iconName = "flat_a.png";
			break;
		case TerrainType::empty:
			break;
		case TerrainType::coastal:
			iconName = "coastal_a.png";
			break;
		case TerrainType::sea_abyss_1:
			iconName = "sea_abyss_1_a.png";
			break;
		case TerrainType::sea_abyss_2:
			iconName = "sea_abyss_2_a.png";
			break;
		case TerrainType::sea_abyss_3:
			iconName = "sea_abyss_3_a.png";
			break;
		case TerrainType::desert_flat_1:
			iconName = "desert_flat_1_a.png";
			break;
		case TerrainType::desert_low_dunes:
			iconName = "desert_low_dunes_a.png";
			break;
		case TerrainType::desert_low_mountains:
			iconName = "desert_low_mountains_a.png";
			break;
		case TerrainType::desert_low_mountains_grow:
			iconName = "desert_low_mountains_a.png";
			break;
		case TerrainType::desert_high_mountains_1:
			iconName = "desert_high_mountains_1_a.png";
			break;
		case TerrainType::desert_high_mountains_1_grow:
			iconName = "desert_high_mountains_1_a.png";
			break;
		}

		return iconName;
	}

	std::string TerrainEdit::getDefaultTextureNameForTerrainType(enum class TerrainEdit::TerrainType terrainType, enum class TerrainEdit::TextureType textureType)
	{
		std::string ret = "";

		switch (terrainType)
		{
		case TerrainType::empty:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = EMPTY_QUADRANT_TERRAIN_TEXTURE;
				//ret = "Ground037_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = EMPTY_QUADRANT_TERRAIN_TEXTURE;
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = EMPTY_QUADRANT_TERRAIN_TEXTURE;
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = EMPTY_QUADRANT_TERRAIN_TEXTURE;
			break;
		case TerrainType::flat:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass_2";
			//ret = "Rock028_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass_2";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "grass_2";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "grass_2";
			break;
		case TerrainType::campaign_1:
		case TerrainType::plateau_1:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass_2";
				//ret = "Rock028_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass_2";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "dirt_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		case TerrainType::high_mountains_1:
		case TerrainType::high_mountains_1_grow:
		case TerrainType::high_mountains_2:
		case TerrainType::high_mountains_2_grow:
		case TerrainType::high_mountains_3:
		case TerrainType::high_mountains_3_grow:
		case TerrainType::high_mountains_4:
		case TerrainType::high_mountains_4_grow:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "snow_1";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "rocks_2";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		case TerrainType::coastal:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "beach_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass_2";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "dirt_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		case TerrainType::sea_abyss_1:
		case TerrainType::sea_abyss_2:
		case TerrainType::sea_abyss_3:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "beach_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "beach_1";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "beach_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "beach_1";
			break;
		case TerrainType::desert_flat_1:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "desert_1";
			break;
		case TerrainType::desert_low_dunes:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "desert_1";
			break;
		case TerrainType::desert_low_mountains:
		case TerrainType::desert_low_mountains_grow:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "desert_2";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "rocks_2";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		case TerrainType::desert_high_mountains_1:
		case TerrainType::desert_high_mountains_1_grow:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "desert_2";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "desert_2";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "desert_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		default:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass_1";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass_1";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "dirt_1";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks_1";
			break;
		}

		return ret;
	}

	void TerrainEdit::setTextureNameForTerrainType(enum class TerrainEdit::TextureType textureType)
	{
		std::string texName = getDefaultTextureNameForTerrainType(terrainType, textureType);
		if (textureType == TerrainEdit::TextureType::lowElevation)
			strcpy_s(extraValues.lowElevationTexName_r, sizeof(extraValues.lowElevationTexName_r), texName.c_str());
		else if (textureType == TerrainEdit::TextureType::highElevation)
			strcpy_s(extraValues.highElevationTexName_g, sizeof(extraValues.highElevationTexName_g), texName.c_str());
		else if (textureType == TerrainEdit::TextureType::dirt)
			strcpy_s(extraValues.dirtTexName_b, sizeof(extraValues.dirtTexName_b), texName.c_str());
		else if (textureType == TerrainEdit::TextureType::rocks)
			strcpy_s(extraValues.rocksTexName_a, sizeof(extraValues.rocksTexName_a), texName.c_str());
	}

	void TerrainEdit::init(enum class TerrainEdit::TerrainType terrainType)
	{
		std::string s = getDefaultTextureNameForTerrainType(terrainType, TerrainEdit::TextureType::lowElevation);
		strcpy_s(extraValues.lowElevationTexName_r, sizeof(extraValues.lowElevationTexName_r), s.c_str());
		s = getDefaultTextureNameForTerrainType(terrainType, TerrainEdit::TextureType::highElevation);
		strcpy_s(extraValues.highElevationTexName_g, sizeof(extraValues.highElevationTexName_g), s.c_str());
		s = getDefaultTextureNameForTerrainType(terrainType, TerrainEdit::TextureType::dirt);
		strcpy_s(extraValues.dirtTexName_b, sizeof(extraValues.dirtTexName_b), s.c_str());
		s = getDefaultTextureNameForTerrainType(terrainType, TerrainEdit::TextureType::rocks);
		strcpy_s(extraValues.rocksTexName_a, sizeof(extraValues.rocksTexName_a), s.c_str());

		switch (terrainType)
		{
		case TerrainEdit::TerrainType::empty:
		case TerrainEdit::TerrainType::flat:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 100;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 50.0f;
			extraValues.ignoreDesideredMinHeight = true;

			if (terrainType == TerrainEdit::TerrainType::empty)
			{
				minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;

				eastSideXPlus.needBlend = true;
				eastSideXPlus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				eastSideXPlus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;

				westSideXMinus.needBlend = true;
				westSideXMinus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				westSideXMinus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;

				southSideZPlus.needBlend = true;
				southSideZPlus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				southSideZPlus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;

				northSideZMinus.needBlend = true;
				northSideZMinus.minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				northSideZMinus.maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			}
			else
				extraValues.amplitudeBlendFactor = 0.5;

		}
		break;
		case TerrainEdit::TerrainType::jagged_1:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 100;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 50.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::campaign_1:
		case TerrainEdit::TerrainType::desert_flat_1:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 100;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 50.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::plateau_1:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 100;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 50.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::low_hills:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 1000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 300.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::desert_low_dunes:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.15f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 1000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 300.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_hills:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 500.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::low_mountains:
		case TerrainEdit::TerrainType::desert_low_mountains:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2500;
			noise.scaleFactor_JagIntensity = 1.0f / 1.5f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 700.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::low_mountains_grow:
		case TerrainEdit::TerrainType::desert_low_mountains_grow:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2500;
			noise.scaleFactor_JagIntensity = 1.0f / 1.5f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;
			
			extraValues.postGenerateElevation = 700.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1:
		case TerrainEdit::TerrainType::desert_high_mountains_1:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 4000;
			noise.scaleFactor_JagIntensity = 1.0f / 1.5f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 1000.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1_grow:
		case TerrainEdit::TerrainType::desert_high_mountains_1_grow:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 4000;
			noise.scaleFactor_JagIntensity = 1.0f / 1.5f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 1000.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_2:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 2000.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_2_grow:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 2000.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_3:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 3000.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_3_grow:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 3000.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_4:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 4000.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_4_grow:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.0005f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 5;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 0.5f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 9000;
			noise.scaleFactor_JagIntensity = 1.0f / 3.0f;
			//noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 4000.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::coastal:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = -1000.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;

			extraValues.postGenerateElevation = 0.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_1:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = -2000.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 0.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_2:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = -3000.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 0.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::sea_abyss_3:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 1337;
			noise.frequency = 0.00008f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 3;
			noise.fractalLacunarity = 2.0f;
			noise.fractalGain = 10.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 2.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 1337;
			noise.warpNoiseDomainWarpAmp = 30.0f;
			noise.warpNoiseFrequency = 0.005f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 5;
			noise.warpNoiseFractalLacunarity = 2.0f;
			noise.warpNoiseFractalGain = 0.5f;

			noise.heightsAmplitude = 2000;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = -4000.0f;
			noise.desideredMinHeigthMandatory = true;

			//extraValues.numBlendingCycles = 2;
			extraValues.amplitudeBlendFactor = 0.5;

			extraValues.postGenerateElevation = 0.0f;
			
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::noise_1:
		default:
		{
			noise.noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin;
			noise.rotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.noiseSeed = 0;
			noise.frequency = 0.0f;
			noise.fractalType = FastNoiseLite::FractalType::FractalType_FBm;
			noise.fractalOctaves = 0;
			noise.fractalLacunarity = 0.0f;
			noise.fractalGain = 0.0f;
			noise.fractalWeightedStrength = 0.0f;
			noise.fractalPingPongStrength = 0.0f;

			noise.cellularDistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
			noise.cellularReturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
			noise.cellularJitter = 1.0f;

			noise.warpNoiseDomainWarpType = -1;
			noise.warpNoiseRotationType3D = FastNoiseLite::RotationType3D::RotationType3D_None;
			noise.warpNoiseSeed = 0;
			noise.warpNoiseDomainWarpAmp = 0.0f;
			noise.warpNoiseFrequency = 0.0f;
			noise.warpNoieseFractalType = FastNoiseLite::FractalType::FractalType_None;
			noise.warpNoiseFractalOctaves = 0;
			noise.warpNoiseFractalLacunarity = 0.0f;
			noise.warpNoiseFractalGain = 0.0f;

			noise.heightsAmplitude = 0;
			noise.scaleFactor_JagIntensity = 1.0f;
			noise.desideredMinHeight = 0.0f;
			noise.desideredMinHeigthMandatory = false;
		}
		break;
		}
	}
}
