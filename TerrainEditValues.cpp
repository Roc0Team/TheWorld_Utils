#include "TerrainEditValues.h"
#include "WorldModifier.h"
#include "FastNoiseLite.h"
#include "Utils.h"

namespace TheWorld_Utils
{
	std::string TerrainEdit::getDefaultTextureNameForTerrainType(enum class TerrainEdit::TerrainType terrainType, enum class TerrainEdit::TextureType textureType)
	{
		std::string ret = "";

		switch (terrainType)
		{
		case TerrainType::empty:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "Marble021_1K";
				//ret = "Ground037_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "Marble021_1K";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "Marble021_1K";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "Marble021_1K";
			break;
		case TerrainType::flat:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass";
			//ret = "Rock028_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "grass";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "grass";
			break;
		case TerrainType::campaign_1:
		case TerrainType::plateau_1:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "grass";
				//ret = "Rock028_1K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "leaves";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks07";
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
				ret = "Ground003_2K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "PaintedPlaster017_1K";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "Ground049C_1K";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks07";
			break;
		case TerrainType::coastal:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "sand";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "grass";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "leaves";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks07";
			break;
		case TerrainType::sea_abyss_1:
		case TerrainType::sea_abyss_2:
		case TerrainType::sea_abyss_3:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "sand";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "sand";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "sand";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "sand";
			break;
		default:
			if (textureType == TerrainEdit::TextureType::lowElevation)
				ret = "Ground003_2K";
			else if (textureType == TerrainEdit::TextureType::highElevation)
				ret = "Ground003_2K";
			else if (textureType == TerrainEdit::TextureType::dirt)
				ret = "leaves";
			else if (textureType == TerrainEdit::TextureType::rocks)
				ret = "rocks07";
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 0.0f;
			extraValues.ignoreDesideredMinHeight = true;

			if (terrainType == TerrainEdit::TerrainType::empty)
			{
				minHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
				maxHeight = EMPTY_QUADRANTS_TERRAIN_HEIGHT;
			}
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 0.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::campaign_1:
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 0.0f;
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 0.0f;
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 0.0f;
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 500.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::low_mountains:
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 500.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::low_mountains_grow:
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 500.0f;
			extraValues.ignoreDesideredMinHeight = false;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1:
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

			extraValues.numBlendingCycles = 2;
			extraValues.postGenerateElevation = 1000.0f;
			extraValues.ignoreDesideredMinHeight = true;
		}
		break;
		case TerrainEdit::TerrainType::high_mountains_1_grow:
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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

			extraValues.numBlendingCycles = 2;
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
