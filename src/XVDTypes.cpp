/**********************************************************/
/* XVDLith: Monolithic XVD Parser / playground tool       */
/*                    2024 (c) TorusHyperV                */
/*                                                        */
/*  XVDTypes.cpp - Implementation of helper methods to do */
/*                XVD math and manipulation logic.        */
/*                                                        */
/**********************************************************/

///////////////////////////////////////
// C includes
///////////////////////////////////////
#include "XVDTypes.h"
#include <stdio.h>
#include <stdint.h>

//////////////////////////////////////////
// AUXILIARY METHODS                    //
//////////////////////////////////////////
uint32_t BytesToPages(uint64_t size_bytes)
{
    // e.g.
    // 4096 bytes == 1 page needed
    // 4097 bytes == 2 pages needed
    return ((size_bytes + XVD_PAGE_SIZE - 1)/ XVD_PAGE_SIZE); 
}

uint64_t PagesToBytes(uint64_t num_pages)
{
    return num_pages * XVD_PAGE_SIZE;
}

uint64_t BytesToBlocks(uint64_t size_bytes)
{
    return (size_bytes + XVD_BLOCK_SIZE -1) / XVD_BLOCK_SIZE;
}

uint64_t BlocksToBytes(uint64_t num_blocks)
{
    return num_blocks * XVD_BLOCK_SIZE;
}

uint64_t PagesToBlocks(uint32_t num_pages)
{
    return (PagesToBytes(num_pages) + XVD_BLOCK_SIZE -1) / XVD_BLOCK_SIZE;
}

uint64_t AlignSizeToPageBoundary(uint64_t size_bytes)
{
    uint64_t quotient  = size_bytes / XVD_PAGE_SIZE;
    uint64_t remainder = size_bytes % XVD_PAGE_SIZE;
    if(remainder != 0)
    {
        auto aligned_size = (quotient + 1) * XVD_PAGE_SIZE;
        return aligned_size;
    }
    else
        return size_bytes;
}

std::string MsGUIDToString(MS_GUID guid)
{
    char buffer[100];
    sprintf(buffer, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
                    guid.Data1, guid.Data2, guid.Data3, 
                    guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
                    guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(buffer);
}

std::string MsVersionToString(uint64_t version, bool extended)
{
    char buffer[50];
    if(!extended){
        sprintf(buffer, "%d.%d.%d.%d",
                        (version >> 48) & 0x7FFF,
                        (version >> 32) & 0xFFFF,
                        (version >> 16) & 0xFFFF,
                        version & 0xFFFF);}
    else
    {
        sprintf(buffer, "%d.%d.%d.%d.%d",
                        (version >> 63) & 0x0001,
                        (version >> 48) & 0x7FFF,
                        (version >> 32) & 0xFFFF,
                        (version >> 16) & 0xFFFF,
                        version & 0xFFFF);}

    return std::string(buffer);
}

std::string FiletimeToString(uint64_t filetime)
{
    // FILETIME starts from January 1, 1601
    // UNIX epoch starts from January 1, 1970
    // Difference between 1601 and 1970 in seconds
    const uint64_t EPOCH_DIFFERENCE_SECONDS = 11644473600ULL;
    const uint64_t HUNDRED_NANOSECONDS_PER_SECOND = 10000000ULL;

    // Convert FILETIME (100-nanosecond intervals since 1601) to seconds
    uint64_t unix_timestamp = (filetime / HUNDRED_NANOSECONDS_PER_SECOND) - EPOCH_DIFFERENCE_SECONDS;

    // Convert to time_t (standard UNIX time format)
    std::time_t time = static_cast<std::time_t>(unix_timestamp);

    // Convert to human-readable format
    std::tm* gmt_time = std::gmtime(&time);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", gmt_time);

    return std::string(buffer);
}