cmake_minimum_required(VERSION 3.21)

project(TheWorld_Utils LANGUAGES C CXX)

if(NOT WIN32 OR NOT MSVC)
	message(FATAL_ERROR "TheWorld_Utils CMake build requires Windows and MSVC.")
endif()

if(NOT DEFINED THEWORLD_EXTERNAL_ROOT)
	set(THEWORLD_EXTERNAL_ROOT "${PROJECT_SOURCE_DIR}/..")
endif()
if(NOT DEFINED THEWORLD_LIB_DIR)
	set(THEWORLD_LIB_DIR "${THEWORLD_EXTERNAL_ROOT}/TheWorld_GDN_Viewer/x64")
endif()

set(_include_dirs
	"${THEWORLD_EXTERNAL_ROOT}/GSL/include"
	"${PROJECT_SOURCE_DIR}/plog/include"
	"${THEWORLD_EXTERNAL_ROOT}/eigen-5.0.0"
)
foreach(_include_dir IN LISTS _include_dirs)
	if(NOT IS_DIRECTORY "${_include_dir}")
		message(FATAL_ERROR "Required include directory not found: ${_include_dir}")
	endif()
endforeach()

add_library(TheWorld_Utils SHARED
	dllmain.cpp
	half.c
	pch.cpp
	Profiler.cpp
	TimeHelper.cpp
	TerrainEditValues.cpp
	Utils.cpp
	WorldModifier.cpp
)

target_include_directories(TheWorld_Utils PRIVATE ${_include_dirs})
target_compile_features(TheWorld_Utils PRIVATE c_std_11 cxx_std_20)
target_compile_definitions(TheWorld_Utils PRIVATE
	THEWORLDUTILS_EXPORTS
	_WINDOWS
	_USRDLL
	UNICODE
	_UNICODE
	"$<$<CONFIG:Debug>:_DEBUG>"
	"$<$<CONFIG:Release>:NDEBUG>"
)
target_link_libraries(TheWorld_Utils PRIVATE Rpcrt4)
target_compile_options(TheWorld_Utils PRIVATE /W3 /sdl /permissive- /Zc:__cplusplus)

set_target_properties(TheWorld_Utils PROPERTIES
	CXX_EXTENSIONS OFF
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
	PREFIX ""
	OUTPUT_NAME_DEBUG "TheWorld_Utils_d"
	OUTPUT_NAME_RELEASE "TheWorld_Utils"
	RUNTIME_OUTPUT_DIRECTORY "${THEWORLD_LIB_DIR}/$<0:>"
	ARCHIVE_OUTPUT_DIRECTORY "${THEWORLD_LIB_DIR}/$<0:>"
)
