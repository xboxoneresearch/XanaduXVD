/**********************************************************/
/* XanaduXVD: Monolithic XVD Parser / playground tool     */
/*                    2024 (c) TorusHyperV                */
/*                                                        */
/*  XanaduXVD.cpp - Implementation of an XVD manipulator  */
/*                                                        */
/**********************************************************/

///////////////////////////////////////
// Project includes
///////////////////////////////////////
#include "XanaduXVD.h"

///////////////////////////////////////
// Windows includes (replace with wine later)
///////////////////////////////////////
#include <Windows.h>

// FIXME: Where does the PLS section fit in the schemes below???

//////////////////////////////////////////
//***      Static XVD structure      ***//
//////////////////////////////////////////
//              HEADER                  //
//////////////////////////////////////////
//               eXVD                   //
//////////////////////////////////////////
//                MDU                   //
//////////////////////////////////////////
//             HashTree                 //
//////////////////////////////////////////
//             UserData                 //
//////////////////////////////////////////
//              XVC INFO                //
//////////////////////////////////////////
//            XVC_REGION_0              //
//////////////////////////////////////////
//                ...                   //
//////////////////////////////////////////
//            XVC_REGION_N              //
//////////////////////////////////////////
//               DRIVE                  //
//////////////////////////////////////////


// Note: all regions: eXVD, MDU, HashTree, UserData, XVC*, DynHeader
//       are optional, and if they exist, this is indicated by the
//       header.

//////////////////////////////////////////
//***      Dynamic XVD structure     ***//
//////////////////////////////////////////
//              HEADER                  //
//////////////////////////////////////////
//               eXVD                   //
//////////////////////////////////////////
//                MDU                   //
//////////////////////////////////////////
//             HashTree                 //
//////////////////////////////////////////
//             UserData                 //
//////////////////////////////////////////
//              XVC INFO                //
//////////////////////////////////////////
//            XVC_REGION_0              //
//////////////////////////////////////////
//                ...                   //
//////////////////////////////////////////
//            XVC_REGION_N              //
//////////////////////////////////////////
//            Dyn Header                //
//////////////////////////////////////////
//               DRIVE                  //
//////////////////////////////////////////


//////////////////////////////////////////
// CONSTRUCTION & INTERNAL METHODS      //
//////////////////////////////////////////
XanaduXVD::XanaduXVD(const char* filename) : mFilename(filename)
{}

int XanaduXVD::Start(bool unsafe_mode, bool debug_mode)
{
    // Config object attributes
    mDebugMode  = debug_mode;
    mUnsafeMode = unsafe_mode;

    // 1. Open XVD File
    FILE* f = fopen(mFilename.c_str(), "rb");
    if (f == NULL) {
        fprintf(stderr, "ERR: Failed to open file '%s'!\n", mFilename.c_str());
        return 2;
    }

    // Save file descriptor
    mFD = f;

    // Get file size from filesystem directly
    mFilesize = std::filesystem::file_size(mFilename.c_str());

    // Check if the file_size makes sense
    if (mFilesize < XVD_HEADER_INCL_SIGNATURE) {
        fprintf(stderr, "ERR: File '%s' too small (%ju bytes) to be XVD!\n",
                mFilename.c_str(), mFilesize);
        return 3;
    }

    // Allocate a buffer to read the header into memory
    char* buffer = (char*)calloc(1, XVD_HEADER_INCL_SIGNATURE);

    // Read the header from file
    fread(buffer, 1, XVD_HEADER_INCL_SIGNATURE, mFD);

    // Cast the buffer to the header struct
    const XvdHeader *xvd=(const struct XvdHeader *)buffer;

    // Copy into our object's memory
    mHeader = *xvd;
    free(buffer);

    // 2. Basic format verification (magic verification, sizes, etc)
    if(!IsValidHeader())
    {
        fprintf(stderr, "ERR: File '%s' -> Header verification failed\n", mFilename.c_str());
        return 4;
    }

    // If we have verified the header, we can do some parsing
    // ParseHeader()

    return 0;
}

int XanaduXVD::Stop()
{
    // Close XVD File
    fclose(mFD);

    // free(buffer)s
    return 0;
}

bool XanaduXVD::IsValidHeader()
{
    // Check MAGIC
    if (memcmp(mHeader.magic, MAGIC, 8)) {
        fprintf(stderr, "ERR: File '%s' -> Invalid magic, not msft-xvd, got: %s!\n",
                mFilename.c_str(), mHeader.magic);
        return 4;
    }

    // Print flags about the XVD
    fprintf(stdout, "DBG: Xvd Type:                       %s\n\n", mHeader.xvd_type == XvdType::FIXED ? "Fixed" : "Dynamic");
    fprintf(stdout, "DBG: Content Type:                   %s\n\n", ContentTypeStr(mHeader.content_type));
    fprintf(stdout, "DBG: ReadOnly:                       %s\n", (mHeader.flags.ReadOnly == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: ResiliencyEnabled:              %s\n", (mHeader.flags.ResiliencyEnabled == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: DataIntegrityDisabled:          %s\n", (mHeader.flags.DataIntegrityDisabled == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: EncryptionDisabled:             %s\n", (mHeader.flags.EncryptionDisabled == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: LegacySectorSize:               %s\n", (mHeader.flags.LegacySectorSize == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: SraReadOnly:                    %s\n", (mHeader.flags.SraReadOnly == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: TrimSupported:                  %s\n", (mHeader.flags.TrimSupported == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: StreamingRoamable:              %s\n", (mHeader.flags.StreamingRoamable == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: RoamingEnabled:                 %s\n", (mHeader.flags.RoamingEnabled == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: TitleSpecific:                  %s\n", (mHeader.flags.TitleSpecific == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: DiffusiveDisabled:              %s\n", (mHeader.flags.DiffusiveDisabled == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: PointerXvd:                     %s\n", (mHeader.flags.PointerXvd == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: RegionIdInXts:                  %s\n", (mHeader.flags.RegionIdInXts == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: SpoofedDuid:                    %s\n", (mHeader.flags.SpoofedDuid == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: Reserved0 (prev TrimSupported): %s\n", (mHeader.flags.Reserved0 == 1) ? "Yes" : "No");
    fprintf(stdout, "DBG: Reserved Area:                  0x%8x\n", mHeader.flags.Reserved);
    printf("\n");

    // Check XVD format version
    // At the moment, XanaduXVD only supports v2 and v3, other versions have not been found in the wild
    if(mHeader.format_version != 3)
    {
        printf("Rare XVD Format Version found: %d\n", mHeader.format_version);
        if(mHeader.format_version != 2)
            return false;
    }

    // Check if xvd type is dynamic or fixed
    if(mHeader.xvd_type != XvdType::FIXED && mHeader.xvd_type != XvdType::DYNAMIC)
    {
        fprintf(stderr, "ERR: File '%s' -> Invalid Xvd Type '%i'!\n", 
                mFilename.c_str(), mHeader.xvd_type);
        return false;
    }
    else if(mHeader.block_size != XVD_BLOCK_SIZE) // Also check the block size is "standard"
    {
        fprintf(stderr, "ERR: File '%s' -> Invalid Block Size '%x'! - unsupported\n",
                mFilename.c_str(), mHeader.block_size);
        return false;
    }

    // TODO Add some more checks in the future? Think of more checks

    // Consider the size check as a validity check. Add extra override to disabled this
    uint64_t computed_filesize = 0;
    if(mHeader.xvd_type == XvdType::FIXED)
    {
        computed_filesize = 
        (
            XVD_HEADER_INCL_SIGNATURE    +  // Header + Sig
            FindEmbeddedXVDSize()        +  // Size of the embedded XVD
            FindMDUSize()                +  // Size of mutable XVC info
            FindHashTreeSize()           +  // Size of the HashTree
            FindUserDataSize()           +  // Size of user data region
            FindXVCSize()                +  // Size of XVC Region
            FindDynHeaderSize()          +  // This will be 0 anyways since it's a fixed XVD...
            FindDriveSize()                 // Size of the static Drive
        );
    }
    else
    {
        computed_filesize = 
        (
            XVD_HEADER_INCL_SIGNATURE     +  // Header + Sig
            FindEmbeddedXVDSize()         +  // Size of the embedded XVD
            FindMDUSize()                 +  // Size of mutable XVC info
            FindHashTreeSize()               // Size of the HashTree (in this case it'll be computed differently)
        );

        // On Dynamic XVDs, data can be added or deleted, thus changing the container size.
        // This affects the UserData, XVC and Drive regions, but also the BAT/DynamicHeader,
        // which keeps tracks of the mappings for the previously mentioned regions, thus it
        // also changes.
        computed_filesize += FindDynamicOccupancy();
    }

    // Some extra debug prints
    if(mDebugMode)
    {
        // Print all the individual sizes to take into account for debugging purposes
        fprintf(stdout, "\n");
        fprintf(stdout, "DBG: XvdHeader_W_SIGNATURE: 0x%16x\n",  XVD_HEADER_INCL_SIGNATURE);
        fprintf(stdout, "DBG: embedded_xvd_length:    0x%16x\n", FindEmbeddedXVDSize());
        fprintf(stdout, "DBG: mutable_data_length:    0x%16x\n", FindMDUSize());
        fprintf(stdout, "DBG: hash_tree_length:       0x%16x\n", FindHashTreeSize());
        
        if(mHeader.xvd_type == XvdType::FIXED)
        {
            fprintf(stdout, "DBG: user_data_length:       0x%16x\n", FindUserDataSize());
            fprintf(stdout, "DBG: xvc_info_length:        0x%16x\n", FindXVCSize());
            fprintf(stdout, "DBG: dynamic_header_length:  0x%16x\n", FindDynHeaderSize());
            fprintf(stdout, "DBG: drive_size:             0x%16llx\n\n", FindDriveSize());
        }
        else
        {
            fprintf(stdout, "DBG: user_data_length:       0x%16x\n", FindUserDataSize());
            fprintf(stdout, "DBG: xvc_info_length:        0x%16x\n", FindXVCSize());
            fprintf(stdout, "DBG: dynamic_header_length:  0x%16x\n", FindDynHeaderSize());
            fprintf(stdout, "DBG: drive_size:             0x%16llx\n\n", FindDriveSize());
        }
    }

    if (mFilesize != computed_filesize)
    {
        fprintf(stderr, "ERR: File '%s' -> Invalid calculated filesize, real: 0x%llx, calculated: 0x%llx!\n", mFilename.c_str(), mFilesize, computed_filesize);
        if(computed_filesize > mFilesize){
            fprintf(stderr, "ERR: File '%s' -> calculated size bigger than real filesize\n", mFilename.c_str());}
        else{
            fprintf(stderr, "ERR: File '%s' -> calculated size smaller than real filesize\n", mFilename.c_str());}

        return false;
    }
    else if(mDebugMode)
        fprintf(stdout, "OUT: File '%s' -> Size matches. real: 0x%llx, calculated: 0x%llx!\n", mFilename.c_str(), mFilesize, computed_filesize);

    // TODO: Add print about the XVD Having unusual flags set

    // PLS Check (debug for now)
    if(mDebugMode && (mHeader.pls_size != 0))
    {
        printf("XVD Has PLS Size: %d\n", mHeader.pls_size);
    }

    return true;
}

void XanaduXVD::ParseHeader()
{
    // Check what important flags we need to take care of
    if(mHeader.flags.LegacySectorSize)
        mSectorSize = SECTOR_SIZE_LEGACY;
    else
        mSectorSize = SECTOR_SIZE_MODERN;
}

//////////////////////////////////////////
// eXVD                                 //
//////////////////////////////////////////
uint64_t XanaduXVD::FindEmbeddedXVDPosition()
{
    // Embedded XVD, if existing, is always the first thing after
    // the header. Header is a fixed max size of 0x3000, so the eXVD
    // will always be at that offset. (if it is present)
    return 0x3000;
}

uint64_t XanaduXVD::FindEmbeddedXVDSize()
{
    if(AlignSizeToPageBoundary(mHeader.embedded_xvd_length) != mHeader.embedded_xvd_length)
    {
        printf("INFO: embedded_xvd_length alignment problem - should probably not use alignment\n");
    }

    // If the header says eXVD length is 0, it means the XVD does
    // not come with an embedded XVD.
    return AlignSizeToPageBoundary(mHeader.embedded_xvd_length);
}

//////////////////////////////////////////
// MDU                                  //
//////////////////////////////////////////
uint64_t XanaduXVD::FindMDUPosition()
{
    // Mutable Data always comes after EmbeddedXVD. Thus
    // It will be found immediately after the EmbeddedXVD

    // Otherwise, mutable starts at the end of EmbeddedXVD
    return FindEmbeddedXVDPosition() + FindEmbeddedXVDSize();
}

uint64_t XanaduXVD::FindMDUSize()
{
    // If header says there are no mutable pages, then there
    // is no mutable data region.
    return PagesToBytes(mHeader.mutable_page_num); 
}

//////////////////////////////////////////
// HashTree                             //
//////////////////////////////////////////
uint64_t XanaduXVD::FindHashTreePosition()
{
    // HashTree always comes after MDU. Thus it will be found
    // immediately after the mutable data region.
    return FindMDUPosition() + FindMDUSize();
}

uint64_t XanaduXVD::FindHashTreeSize()
{
    // If data integrity is disabled, resiliency doesn't make any sense
    bool data_integrity_en  = !(mHeader.flags.DataIntegrityDisabled);
    bool has_resiliency_en  = mHeader.flags.ResiliencyEnabled;

    // If data integrity isn't enabled, there isn't a HashTree! Duh!
    if(!data_integrity_en)
        return 0;

    // The size of the HashTree depends pretty much on the size of
    // the data that it hashes. In other words, the more data pages
    // that need to be hashed, the bigger the HashTree would be.

    // Most specifically, the following regions seem to be being hashed:
    // - Drive
    // - UserData
    // - XVC INFO
    // - Dynamic Header

    // So the first step is to find the size of each region (in disk) in
    // pages. For a static XVD we can just read the sizes of the data from
    // the headers, and then compute the size of the resulting HashTree
    // that would be obtained from hashing all of those data pages.
    if(mHeader.xvd_type == XvdType::FIXED)
    {
        auto hashed_pages = BytesToPages(
                                FindDriveSize()    
                                + FindUserDataSize()
                                + FindXVCSize()
                                + FindDynHeaderSize());

        // Compute the size that the HashTree will have
        return HashTreeSizeFromPageNum(hashed_pages, has_resiliency_en);
    }
    else
    {
        // In the case of a Dynamic XVD, we have to "estimate" the sizes
        // of the BAT+UserData+XVC+Drive, since we cannot trust the values in the
        // header, because 'drive_size' is the maximum size of the drive region, not
        // the actual occupancy / size in disk of the region.
        // Once those sizes are estimated, we can compute the size of the HashTree.

        // We could use the value of mHeader.drive_size because although it represents
        // the MAXIMUM allocated size of this virtual drive, the hash_tree seems to be
        // allocated for that maximum size, and not the actual occupancy on the physical file in disk.
        // PENDING: So are parts of the HashTree region just empty?

        // Compute the size of drive + userdata + xvc from the DynHeader size in the header
        uint64_t bat_entries = mHeader.dynamic_header_length / BAT_ENTRY_SIZE;
        //printf("bat entries: %d\n", bat_entries);

        auto total_blocks_mapped = bat_entries;

        // Need to find how many of those blocks are actually allocated... 

        // 2. Compute the estimated size of the HashTree given the BAT size.
        //    In disk, the computed space must end up being page aligned!
        uint64_t size_bytes = BlocksToBytes(total_blocks_mapped); 
        size_bytes = AlignSizeToPageBoundary(size_bytes);

        // REMOVE ME LATER
        // check: this should be exactly the same as adding all the header sizes
        // (taking into account, in disk, they must be page aligned)
        /*auto head_size = AlignSizeToPageBoundary(mHeader.drive_size) +
                         AlignSizeToPageBoundary(mHeader.user_data_length) +
                         AlignSizeToPageBoundary(mHeader.dynamic_header_length) +
                         AlignSizeToPageBoundary(mHeader.xvc_data_length);

        printf("head_size:          %d\n", head_size);
        printf("bat_computed_size:  %d\n", size_bytes);
        */

        auto exact_division = (size_bytes % XVD_PAGE_SIZE) == 0;
        auto size_in_pages = (size_bytes) / XVD_PAGE_SIZE + (exact_division ? 0 : 1);
        return HashTreeSizeFromPageNum(size_in_pages, has_resiliency_en);
    }
}

uint64_t XanaduXVD::HashTreeSizeFromPageNum(uint64_t num_pages_to_hash, bool resilient)
{
    // Computes the size of an XVD HashTree given a number of pages

    // We need to figure out, how many hashes (or pages dedicated to
    // storing the hashes) are required to store these hashing information.
    // Basically it's a division of: #num_pages_to_hash / #hashes_per_page
    // (NOTE: Remember a page is, almost always, 4k in size)

    // However, we're not done yet. Because the computation above is just for
    // the first level of the HashTree. Remember a HashTree consists of levels
    // of hashes of hashes of hashes... Only the first level are actually hashes
    // of the disk data.

    // So how many level are there? Well, if the HashTree were to be a binary
    // tree, we'd end up with so so many levels (since each level could only
    // contain half the hashes from the previous level, until reaching the top hash)
    // that it'd be very inefficient. Thus, it seems that MS engineers opted for 
    // hashing a larger number of hashes, to generate a higher-level hash. More
    // specifically, the hashing algorithm seems to take as much as 170 (0xAA) hashes
    // at once, and it spits a new hash from all of them. Thus, each level that the
    // tree has, increases the number of hashes exponentially by 170. It can look
    // something like this... 
    /*

    DATA
    PAGES             ---

    L0                ...

    L1                ...

    L2                ...

    L3  [h0]          [h1]  ...    [h169]  
          \            |            /
           \           |           /
            \          |          /
             \         |         /
              \        |        /
               \       |       /
                \      |      /
                 \     |     /
                  \    |    /
                   \   |   /
                    \  |  /
                     \ | /
    R             [ROOT HASH]

        
    */
    // Terrible drawing but you get the idea. Get an AI to do something better

    // The drawing above represents the top hash, which is generated from 170
    // hashes. Each of those hashes, would at the same time, be derived from
    // 170 hashes on the lower layer. And so on. We want to compute the size of the whole tree.

    // According to XVDTool and the other tools out there, the XVD design just
    // works with 4 levels (not counting the root hash). This makes for a total
    // of
    //                 1   hash   (root hash)
    // 170                 hashes (level 3)
    // 170^2 (28900)       hashes (level 2)
    // 170^3 (4913000)     hashes (level 1)
    // 170^4 (835.210.000) hashes (level 0)
    // 
    // This makes up for a possible maximum total of 835.210.000 hashes JUST on
    // the lowest level (0) of the tree. Now, if we assume that each hash guards
    // a 4k page, then:
    // 835.210.000 hashes * 4096 bytes = 3.421.020.160.000 bytes that can be guarded with the HashTree.
    // Which is basically up to 3Tb of information that can be integrity guarded. Very cool!
    //
    // Some extra MATH FUN. What would be the size of the maximum possible HashTree in an XVD?
    // i.e.: a HashTree for a 3Tb (drive + userdata + xvc info + dynheader) 
    // Or in other words that'd be:
    // how many 4K pages are needed for the Level 0's 835.210.000 hashes, plus
    // how many 4K pages are needed for the Level 1's 835.210.000/170 (4913000) hashes, plus
    // how many 4K pages are needed for the Level 2's 4913000/170 (28900) hashes, plus
    // how many 4K pages are needed for the Level 3's 28900/170 (170) hashes, plus
    // 32 bytes (the size of the final root hash)
    // 
    // 835210000 hashes * 24 bytes/hash = 20.045.040.000 bytes
    // 4913000   hashes * 24 bytes/hash =    117.912.000 bytes
    // 28900     hashes * 24 bytes/hash =    693.600     bytes
    // 170       hashes * 24 bytes/hash =      4.080     bytes (~1 Page)
    //                    root hash     =         32     bytes
    //---------------------------------------------------------- +
    //                                      20163649712  bytes = 18Gbytes
    //
    // Awesome. Maintaining the integrity of 3Tb of data only costs
    // us 18Gb. That's 18*10^9 / 3*10^12 = 0.006%  overhead  (in vague terms of data quantities).
    // Microsoft folks really tuned the HashTree with that 170 number which yields
    // great results. (But please lord do not make us ever have to parse a 3Tb XVD file)

    // Back to the code. How can we know the exact size the HashTree is going to occupy?
    // We know the number of pages of the data that is hashed. At minimum, one tree level
    // will always be required. Level type 3 can account for 170 hashes, so 170 pages, which
    // is equivalent to 696.320 bytes (0,66 Mb). So very few XVDs will just use one level.
    // With two levels, and assuming the leaf level, a Level type 2 level, were to be used to the max,
    // it'd be possible to store 28900 hashes, or 118.374.400 bytes (112,89 Mb). Still not much. Not sufficient for games.
    // With a full Level type 3 leaf level, it'd be 20.123.648.000 bytes (18,74 Gb). Okay for some small games.
    // Thus it can be expected that most big games will require a HashTree with a leaf level of
    // type 4. (Capable of holding 835210000 hashes of pages)

    // In any case, we need to compute how many (lowest level, data) hashes are needed for the pages 
    // that need to be hashed, and how many levels (as extra levels will mean more hashes). 
    // The actual level occupancy discourse above is just a curiosity. Here's the logic:
    

    // NEW EXPLAINATION BELOW
    /******************************************************************************************\
                                XVD HashTree THEORY OF OPERATION

    XVD Uses a custom HashTree structure to ensure integrity of the data sections of the disk.
    "Data Sections" includes: UserData, XVC, DynamicHeader and Drive regions of the disk. Other
    regions are not considered data and are not covered by the HashTree.

    The XVD HashTree uses SHA256 as the hashing algorithm. Usually a SHA256 digest is 32bit in
    size. This is the case of the XVD's root hash, which is stored in the header. However, for
    the rest of the hashes, they are truncated to 24 bits.

    The minimum unit of data that gets hashed is a page (4Kb).

    The XVD HashTree is divided in levels, although the concept of level is somewhat loose.
    There exists 4 levels (from 0, the leaves level, to 3) and they don't have a fixed size.

    Level 0: it is as big as many hashes are needed for all the data pages. It is always present
    
                            REMEMBER: Unit of data for hashing is the page, thus

    Level 1: contains hashes of the pages of level 0, grouped in sets of up to 170 pages.

    Level 2: contains hashes of the pages of level 1, grouped in sets of up to 170 pages.

    Level 3: contains hashes of the pages of level 2, grouped in sets of up to 170 pages.

    ROOT HASH: Generated from the latest level existing (which can only be a page in size).

    170 (0xAA) is the magic number that ensures that a lot of data can be hashed while
    never needing more than 4 levels. The math above tells us it's enough to ensure
    integrity for up to 3Tb of data, with only 4 levels. 


    example 1: 
            - There are 800 pages of data (UserData+XVC+BAT+Drive)
            - Level 0 will consist of 800 hashes
            - Each hash is 24 bit so 800*24 = 19200 bits
            - Level 0 will be 19200 bits (2400 bytes) (aprox half a 4k page)
            - ROOT HASH is generated from Level 0

    example 2: 
            - There are 9000 pages of data (UserData+XVC+BAT+Drive)
            - Level 0 will consist of 9000 hashes
            - Each hash is 24 bit so 9000*24 = 216000 bits
            - Level 0 will be 216000 bits (27000 bytes, 26Kb) (6,5 pages, rounded to 7 pages)
            - Level 1 will contain one page for each 170 pages in Level 0. Thus, in this case,
              it will only contain one page.
            - ROOT HASH is generated from Level 1

    example 2: 
            - There are 700000 pages of data (UserData+XVC+BAT+Drive)
            - Level 0 will consist of 700000 hashes
            - Each hash is 24 bit so 700000*24 = 16800000 bits
            - Level 0 will be 16800000 bits (2100000 bytes, 2.050,78125 Kb) (512,69 pages, rounded to 513 pages)
            - Level 1 will contain one page for each 170 pages in Level 0. Thus, in this case,
              it will contain (513/170 = 3,017 = 4 rounded) pages. 
            - Level 2 will contain one page for each 170 pages in Level 1. In this case that means it contains only 1 page.
            - ROOT HASH is generated from Level 2
    
    As you can see, the moment we reach a level that only contains one page, the HashTree is done. At that point it's
    possible to generate the Root Hash.

    In the XVD physical file, ironically, levels are stored in a somewhat inverted order to my explanation above
    (It is stored resembling a good old traditional inverted tree data structure):
    LEVEL_3
    LEVEL_2
    LEVEL_1
    LEVEL_0

    \*******************************************************************************************/

    // Count of how many pages each tree will be in size. See explanation above
    size_t total_hashtree_pages = 0;
    size_t pages_of_level[4] = {0, 0, 0, 0};
    #define LVL_0 0
    #define LVL_1 1
    #define LVL_2 2
    #define LVL_3 3

    #define HASHES_PER_PAGE 170
    
    // Compute the size of level 0, the leaf/data level
    auto is_exact_division = (num_pages_to_hash % 170) == 0;
    pages_of_level[LVL_0]  = (num_pages_to_hash / 170) + (is_exact_division ? 0 : 1);

    // If this level is already enough, down to one page, it's possible to compute the root hash so the HashTree ends here.
    total_hashtree_pages += pages_of_level[LVL_0];
    if(pages_of_level[LVL_0] <= 1)
    {
        goto done;
    }

    // Compute the size of level 1 
    is_exact_division     = (pages_of_level[LVL_0] % 170) == 0;
    pages_of_level[LVL_1] = (pages_of_level[LVL_0] / 170) + (is_exact_division ? 0 : 1);

    // If this level is already enough, down to one page, it's possible to compute the root hash so the HashTree ends here.
    total_hashtree_pages += pages_of_level[LVL_1];
    if(pages_of_level[LVL_1] <= 1)
    {
        goto done;
    }

    // Compute the size of level 2
    is_exact_division     = (pages_of_level[LVL_1] % 170) == 0;
    pages_of_level[LVL_2] = (pages_of_level[LVL_1] / 170) + (is_exact_division ? 0 : 1);

    // If this level is already enough, down to one page, it's possible to compute the root hash so the HashTree ends here.
    total_hashtree_pages += pages_of_level[LVL_2];
    if(pages_of_level[LVL_2] <= 1)
    {
        goto done;
    }

    // Compute the size of level 3
    is_exact_division     = (pages_of_level[LVL_2] % 170) == 0;
    pages_of_level[LVL_3] = (pages_of_level[LVL_2] / 170) + (is_exact_division ? 0 : 1);

    // XVD Does not really support more than 3 levels, so if there exists a level 3, it MUST be finally only one page in size.
    // Let's actually check for that, to make sure nothing went wrong.
    total_hashtree_pages += pages_of_level[LVL_3];
    if(pages_of_level[LVL_3] != 1) [[unlikely]]
    {
        printf("Here be dragons!!! BUG!\n");
    }

done:
    // Final check: If the resiliency flag was enabled, the size of the tree is just doubled
    if(resilient)
    {
        total_hashtree_pages = 2 * total_hashtree_pages;

        // No actual resilient xvd has ever been found so let's print something
        printf("Call the engineers, a rare resilient XVD has been found!\n");
    }

    // I know, I know... this code could be a little more optimal, sure. But it can't
    // get more readable or easier to understand. That's what we want when documenting
    // obscure formats for which there aren't public standards. Don't you think so?

    // Return the size in bytes
    return total_hashtree_pages * XVD_PAGE_SIZE;
}

//////////////////////////////////////////
// UserData                             //
//////////////////////////////////////////
uint64_t XanaduXVD::FindUserDataPosition()
{
    return FindHashTreePosition() + FindHashTreeSize();
}

uint64_t XanaduXVD::FindUserDataSize()
{
    return AlignSizeToPageBoundary(mHeader.user_data_length); // UserData seemed to require alignment for the computations, yep
}

//////////////////////////////////////////
// XVC Region(s)                        //
//////////////////////////////////////////
uint64_t XanaduXVD::FindXVCPosition()
{
    return FindUserDataPosition() + FindUserDataSize();
}

uint64_t XanaduXVD::FindXVCSize()
{

    if(AlignSizeToPageBoundary(mHeader.xvc_data_length) != mHeader.xvc_data_length)
    {
        printf("INFO: xvc_data_length alignment problem - should probably not use alignment\n");
    }

    return AlignSizeToPageBoundary(mHeader.xvc_data_length); // Alignment here might or might not be needed, but adding it just in case
}

// The XVC Region is itself divided in multiple regions (XVC_INFO, XVC_*****, etc)
// FindXVCSegmentsPositions?

//////////////////////////////////////////
// DynHeader                            //
//////////////////////////////////////////
uint64_t XanaduXVD::FindDynHeaderPosition()
{
    return FindXVCPosition() + FindXVCSize();
}

uint64_t XanaduXVD::FindDynHeaderSize()
{
    return mHeader.dynamic_header_length; // NO Alignment 
}

//////////////////////////////////////////
// Drive                                //
//////////////////////////////////////////
uint64_t XanaduXVD::FindDrivePosition()
{
    // If dynamic XVD, the drive will be after the DynHeader
    if(mHeader.xvd_type == XvdType::DYNAMIC)
        return FindDynHeaderPosition() + FindDynHeaderSize();

    // If static XVD, the drive will be after the XVC_REGION
    return FindXVCPosition() + FindXVCSize();
}

uint64_t XanaduXVD::FindDriveSize()
{
    // Test stuff
    if(AlignSizeToPageBoundary(mHeader.drive_size) != mHeader.drive_size)
    {
        printf("found alignment problem - working w/o alignment\n");
        return mHeader.drive_size;
    }

    // This depends wether the XVD is static (easy, just read header
    // and add all the size fields)
    if(mHeader.xvd_type == XvdType::FIXED)
        return AlignSizeToPageBoundary(mHeader.drive_size);
    else
        return ComputeUsedDriveSizeInDynamicXVD(); // a whole ordeal
}

uint64_t XanaduXVD::ComputeUsedDriveSizeInDynamicXVD()
{
    uint64_t ocupancy = FindDynamicOccupancy();

    // This might seem like a random computation, but if you draw it it's very easy.
    // It all comes together once you remember that the mHeader.drive_size is not the actual occupancy of the disk,
    // but the maximum size that it can take.
    auto unused_drive_space = (mHeader.drive_size + mHeader.user_data_length + mHeader.xvc_data_length + mHeader.dynamic_header_length) - ocupancy;
    auto used_drive_space = mHeader.drive_size - unused_drive_space;

    long double percent = (((long double)used_drive_space) / ((long double)mHeader.drive_size) * 100);
    //printf("used drive size: %llu (%Lf %%)\n", used_drive_space, percent);
    
    return used_drive_space;
}

uint64_t XanaduXVD::FindDynamicOccupancy()
{
    /******************************************************************************************\
                                        BAT THEORY OF OPERATION

    BAT (Block Allocation Table) - also called Dynamic Header in XVDs - is a concept borrowed
    from the VHD (Virtual Hard Disk) fileformat. The BAT is simply a table that contains entries.
    Entries can either be unallocated (their value is 0xFFFFFFFF) or they can be allocated, in
    which case their value is an offset. This offset is an indirection that "maps" blocks.
    
    The mapping is a mapping from the Drive's POV into the XVD file in disk (improve this theory)

    This function FindDynamicOccupancy() is very simple. It finds and reads the BAT, and then
    iterates through all the entries, counting how many of them are allocated. Each entry is
    the representation of a block. If there are N valid entries, this is equivalent to saying
    the XVD file has at least N blocks of data that are in use. Or in other words, at least
    N * sizeof(block) bytes are used.

    Remark 1: For some reason, the BAT covers not only the Drive, but also UserData, XVC, and
              the BAT itself. UserData and XVC shouldn't grow so this is weird, and makes the
              explanation a little bit more difficult to understand. It is not clear for me atm
              how an hypervisor treats a mounted XVD's UserData and XVC regions. Those should in
              theory not be visible for a guestOS. Only the Drive should be "seen" by a guestOS.
              UserData is often the VBI bootloader for an Xbox VM, which iirc gets loaded into
              RAM memory. That somewhat matches my expectations, that UserData is not really ever
              accessed trough the guestOS filesystem (since it's loaded in RAM) so it being in the
              BAT feels out of place.

    Remark 2: Block size is always 0xAA000, although the XVD header has a field to override
              this value, but no other values have yet been found in the wild AFAIK.

    \*******************************************************************************************/

    // 1. Find the BAT offset. This is now possible since we know the sizes of all previous regions (especially the HashTree)
    //     HashTree comes always after MDU and then we have user data, XVC, and finally we'd reach the BAT start offset.
    auto bat_start = FindHashTreePosition() + FindHashTreeSize() + 
                    + mHeader.user_data_length 
                    + mHeader.xvc_data_length;

    auto bat_size  = mHeader.dynamic_header_length;

    //printf("bat_start: 0x%16x\n", bat_start);
    //printf("bat_size:  0x%16x\n", bat_size);

    // 2. Iterate through the BAT entries and find how many entries are valid
    // Allocate a buffer to read the BAT
    char* bat_data = (char *)calloc(1, bat_size);
    fseek(mFD, bat_start, SEEK_SET);
    fread(bat_data, 1, bat_size, mFD);

    // Each block is mapped by one entry. There are as many entries as blocks
    uint32_t max_entry = 0;
    size_t   unallocated_entries = 0;
    size_t   allocated_entries   = 0;
    int      curr_entry = 0;
    auto     bat_entries = bat_size / 4;
    for(curr_entry; curr_entry < bat_entries; curr_entry++)
    {
        // Invalid entry found. Count it and ignore it
        auto entry = ((uint32_t*)bat_data)[curr_entry];
        if(entry == (uint32_t)XVD_INVALID_BLOCK)
        {
            unallocated_entries++;
            continue;
        }
        
        // Save the latest found valid entry that is a bigger offset than what previously found
        allocated_entries++;
        if(max_entry < entry)
            max_entry = entry;        
    }
    /*
    printf("latest valid bat_index: %d / max entry value: 0x%x\n", curr_entry, max_entry);
    printf("ALLOCATED ENTRIES | ENTRIES\n");
    printf("%d | %d", allocated_entries, bat_entries);
    */

    free(bat_data);
    
    // I have not figured exactly why I have to add +1 to the number of BAT entries unfortunately. 
    // The count of allocated entries is correct, so the +1 shouldn't be needed, but it is.
    return (allocated_entries+1) * XVD_BLOCK_SIZE;
}

//////////////////////////////////////////
// PUBLIC / USER FACING METHODS         //
//////////////////////////////////////////
int XanaduXVD::InfoDump()
{
    // Internally keep track of all the relevant values in a serialized form
    // Could probably be a vector of pairs of strs for faster access but idc
    std::unordered_map<std::string, std::string> values;

    // Parse relevant header info in a user-friendly format
    // TODO: Compiler warning is legit, suppress it eventually
    values["CONTENT_ID"] = MsGUIDToString(*(MS_GUID*)mHeader.content_id_guid); // Incredibly illegal casts FTW
    values["USER_ID"] = MsGUIDToString(*(MS_GUID*)mHeader.user_id);
    values["VERSION"] = std::to_string(mHeader.format_version);
    values["SANDBOX"] = std::string((char*)mHeader.sandbox_id);
    values["ProductID"] = MsGUIDToString(*(MS_GUID*)mHeader.ProductId);
    values["PDUID"] = MsGUIDToString(*(MS_GUID*)mHeader.PDUID);
    values["PackageVersion"] = MsVersionToString(mHeader.PackageVersionNumber, false);
    values["MinSysVersion"] = MsVersionToString(mHeader.min_sp_ver, false);
    values["CreationTime"] = FiletimeToString(mHeader.creation_time);
    values["ODK_Keyslot_ID"] = mHeader.odk_id == 0 ? "0 (Defined by ContentType)" : std::to_string(mHeader.odk_id);

    char zeros[RSA_SINGATURE_SIZE] = {0x00};
    bool no_header_signature = memcmp(mHeader.rsa_signature, zeros, RSA_SINGATURE_SIZE) == 0;
    values["SIGNED"] = no_header_signature ? "false" : "true";

    // Print all relevant information for the user
    printf("///////////////////////////// XVD INFORMATION /////////////////////////////\n\n");
    for(auto value : values)
    {
        printf("%-18s : %s\n", value.first.c_str(), value.second.c_str());
    }
    printf("\n///////////////////////////// XVD INFORMATION /////////////////////////////\n");

    return 0;    
}

int XanaduXVD::ExtractEmbeddedXVD(const char* output_filename)
{
    // Get the eXVD region size
    auto exvd_size = FindEmbeddedXVDSize();
    auto exvd_pos  = FindEmbeddedXVDPosition();
    if(exvd_size == 0)
    {
        fprintf(stderr, "ERR: XVD does not contain an eXVD.\n");
        return 1;
    }

    printf("Extracting Embedded XVD...");

    // Allocate a buffer to read the eXVD into memory
    char* buffer = (char *)calloc(1, exvd_size);
    fseek(mFD, exvd_pos, SEEK_SET);
    fread(buffer, 1, exvd_size, mFD);

    // Future optimization: eXVD potentially a large file
    // to read chunk by chunk

    // Write to output file
    FILE* f = fopen(output_filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERR: Failed to open output file '%s'!\n", output_filename);
        return 2;
    }
    fwrite(buffer, 1, exvd_size, f);
    fclose(f);
    free(buffer);

    // IMPROVEMENT: Open the extracted eXVD and check its ID matches
    // against the parent XVD header

    printf(" [DONE]\n");
    return 0;
}

int XanaduXVD::ExtractUserData(const char* output_filename)
{
    // Get the eXVD region size
    auto userdata_size = FindUserDataSize();
    auto userdata_pos  = FindUserDataPosition();
    if(userdata_size == 0)
    {
        fprintf(stderr, "ERR: XVD does not contain UserData.\n");
        return 1;
    }

    printf("Extracting UserData...");

    // Allocate a buffer to read the eXVD into memory
    char* buffer = (char*)calloc(1, userdata_size);
    fseek(mFD, userdata_pos, SEEK_SET);
    fread(buffer, 1, userdata_size, mFD);

    // Future optimization: UserData potentially a large file
    // to read chunk by chunk

    // Write to output file
    FILE* f = fopen(output_filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERR: Failed to open output file '%s'!\n", output_filename);
        return 2;
    }
    fwrite(buffer, 1, userdata_size, f);
    fclose(f);
    free(buffer);

    printf(" [DONE]\n");
    return 0;
}

int XanaduXVD::VerifyHashTree()
{
    return 0;
}

int XanaduXVD::RebuildHashTree()
{
    return 0;
}