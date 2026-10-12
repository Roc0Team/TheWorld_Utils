#pragma once

// D:\TheWorld\Client\TheWorld_MapManager\Data\DBJson\TheWorldMap.TestMap03.json
constexpr auto THEWORLD_SEA_LEVEL = 0.0f;
constexpr auto THEWORLD_SEA_LEVEL_TRACK_NAME = "SEA_LEVEL_TRACKS";
constexpr auto  THEWORLD_SEA_MAX_WAVE_HEIGHT = 30.0f;
constexpr auto EMPTY_QUADRANTS_TERRAIN_HEIGHT = -500.0f;
constexpr auto EMPTY_QUADRANT_TERRAIN_TEXTURE = "beach_1";
constexpr auto UNKNOWN_QUADRANT_TERRAIN_TEXTURE = "beach_1";
//constexpr auto EMPTY_QUADRANT_TERRAIN_TEXTURE = "Marble021_1K";
constexpr auto FLAT_QUADRANTS_TERRAIN_HEIGHT = (THEWORLD_SEA_LEVEL + THEWORLD_SEA_MAX_WAVE_HEIGHT + 1.0f);
constexpr auto THEWORLD_DEFAULT_AMPLITUDE_BLEND_FACTOR = 2.0f;

constexpr auto THEWORLD_QUADRANT_DISTANCE_FOR_BLENDING = 2;

constexpr auto THEWORLD_CHECK_FOR_UNUSED_FILES_INTERVAL = 3600;	// 1 hour in seconds
constexpr auto THEWORLD_DELETE_IMPORTED_FILES_UNUSED_FOR_SECONDS = 259200;	// 3 days in seconds
