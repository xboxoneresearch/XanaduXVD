/**********************************************************/
/* XVDLith: Monolithic XVD Parser / playground tool       */
/*                    2024 (c) TorusHyperV                */
/*                                                        */
/*  XanaduXVD.h - Header of an XVD manipulator class      */
/*                                                        */
/**********************************************************/
#pragma once

///////////////////////////////////////
// XanaduXVD includes
///////////////////////////////////////
#include "XVDTypes.h"

///////////////////////////////////////
// C includes
///////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////
// C++ includes
///////////////////////////////////////
#include <unordered_map>
#include <filesystem>
#include <cmath>
#include <map>

class XanaduXVD
{
public:
    XanaduXVD(const char* filename);
    ~XanaduXVD(){};
    int Start(bool unsafe_mode, bool debug_mode); // Opens the XVD file descriptor, allocates memory, basic header verification, etc.
    int Stop();                                   // Closes the XVD file descriptor, frees memory, commits changes (if any)

///////////////////////////////////////
// INTERNAL TYPES                    //
///////////////////////////////////////
private:
    enum XanaduXVDErr
    {
        FILE_NOT_FOUND   = 1,
        PERMISION_DENIED = 2,
        INVALID_HEADER   = 3,
        INVALID_SIZE     = 4
    };

///////////////////////////////////////
// INTERNAL XVD MANIPULATION METHODS //
///////////////////////////////////////
protected:
    bool     IsValidHeader();
    void     ParseHeader();
    uint64_t FindEmbeddedXVDPosition();
    uint64_t FindEmbeddedXVDSize();
    uint64_t FindMDUPosition();
    uint64_t FindMDUSize();
    uint64_t FindHashTreePosition();
    uint64_t FindHashTreeSize();
    uint64_t FindUserDataPosition();
    uint64_t FindUserDataSize();
    uint64_t FindXVCPosition();
    uint64_t FindXVCSize();
    uint64_t FindDynHeaderPosition();
    uint64_t FindDynHeaderSize();
    uint64_t FindDrivePosition();
    uint64_t FindDriveSize();
    uint64_t FindDynamicOccupancy();
    uint64_t HashTreeSizeFromPageNum(uint64_t num_pages_to_hash, bool resilient);
    uint64_t FindOccupiedDriveSizeFromBAT(uint64_t bat_offset, uint64_t bat_size);
    uint64_t ComputeUsedDriveSizeInDynamicXVD();

///////////////////////////////////////
// PUBLIC FUNCTIONALITY / METHODS    //
///////////////////////////////////////
public:
    int InfoDump();
    int ExtractEmbeddedXVD(const char* output_filename);
    int ExtractUserData(const char* output_filename);
    int VerifyHashTree();
    int RebuildHashTree();

///////////////////////////////////////
// CLASS ATTRIBUTES                  //
///////////////////////////////////////
private:
    // File related variables
    FILE*       mFD;
    size_t      mFilesize   = 0;
    std::string mFilename   = "";

    // tool related variables
    bool        mUnsafeMode = false; // Allows opening and playing with invalid XVD files (use at your own risk!)
    bool        mDebugMode  = false; // Enables debug stdout prints
    bool        mIsStarted  = false; // Specifies wether Start() has been called and was successful. This implies several things

    // Variables related with the XVD being parsed
    XvdHeader   mHeader{};
    uint64_t    mSectorSize; // Sector size is used when parsing the GPT in the Drive region (I think)
};