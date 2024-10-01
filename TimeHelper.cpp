#include "TimeHelper.h"

#ifdef _WIN32
#define FILE_TIME__FILE_TIME_OFFSET ( std::filesystem::__std_fs_file_time_epoch_adjustment ) // ( 116444736000000000ull )
#define FILE_TIME__FILE_TIME_GAIN   ( 10000000ull )
#else
#define FILE_TIME__FILE_TIME_OFFSET ( 12009080074ull )
#define FILE_TIME__FILE_TIME_GAIN   ( 1000000000ull )
#endif

#ifdef _WIN32

std::time_t TimeHelper::to_time_t(const std::filesystem::file_time_type file_time)
{
    std::time_t                            rv = 0ull;
    const std::filesystem::file_time_type* ptr = &file_time;
    uint64_t* timeptr = (uint64_t*)ptr;
    uint64_t                               time = *timeptr;
    if (time >= FILE_TIME__FILE_TIME_OFFSET)
    {
        time -= FILE_TIME__FILE_TIME_OFFSET;
        time += (FILE_TIME__FILE_TIME_GAIN >> 1); // do correct rounding by adding halv of dividend before division
        time /= FILE_TIME__FILE_TIME_GAIN;
        rv = time;
    }
    return rv;
}
std::filesystem::file_time_type TimeHelper::to_FileTimeType(const std::time_t time)
{
    uint64_t                         time_t1 = time * FILE_TIME__FILE_TIME_GAIN;      // time_t to 100ns
    uint64_t                         time_t2 = time_t1 + FILE_TIME__FILE_TIME_OFFSET; // time_t to offset 01.01.1601
    std::filesystem::file_time_type* ptr = (std::filesystem::file_time_type*)&time_t2;
    std::filesystem::file_time_type  rv = *ptr;

    return rv;
}

#else

std::time_t TimeHelper::to_time_t(const std::filesystem::file_time_type file_time)
{
    const std::filesystem::file_time_type* ftptr = &file_time;
    const uint64_t                         ft = *((uint64_t*)ftptr);
    const uint64_t                         offs = FILE_TIME__FILE_TIME_OFFSET;
    const uint64_t                         ts = ft / FILE_TIME__FILE_TIME_GAIN;
    std::time_t                            rv = 0;
    if (offs < ts)
    {
        rv = (std::time_t)(ts - offs);
    }
    return rv;
}

std::filesystem::file_time_type TimeHelper::to_FileTimeType(const std::time_t time)
{
    uint64_t                        offs = FILE_TIME__FILE_TIME_OFFSET;
    uint64_t                        t1 = time + offs;
    uint64_t                        t2 = t1 * FILE_TIME__FILE_TIME_GAIN;
    std::filesystem::file_time_type ft = *((std::filesystem::file_time_type*)&t2);
    return ft;
}

#endif
