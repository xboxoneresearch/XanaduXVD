/**********************************************************/
/* XVDLith: Monolithic XVD Parser / playground tool       */
/*                    2024 (c) TorusHyperV                */
/*                                                        */
/*  XVDTypes.h - Header defining types and constants      */
/*               intrinsic to the XVD Format.             */
/*                                                        */
/**********************************************************/

#pragma once

///////////////////////////////////////
// C includes
///////////////////////////////////////
#include <stdio.h>
#include <stdint.h>

///////////////////////////////////////
// C++ includes
///////////////////////////////////////
#include <ctime>
#include <string>

///////////////////////////////////////
// Constants
///////////////////////////////////////

// TODO: Add a namespace?

// Magic bytes in the header
const char MAGIC[] = "msft-xvd";

// Region sizes defines
#define RSA_SINGATURE_SIZE         0x200
#define XVD_PAGE_SIZE              0x1000
#define XVD_HEADER_INCL_SIGNATURE  0x3000

// BAT Defines
#define XVD_INVALID_BLOCK          0xFFFFFFFF
#define XVD_BLOCK_SIZE             0xAA000      // 680 Kbytes
#define BAT_ENTRY_SIZE             0x4          // BAT Entry is just uint32_t, like in VHD

// HashTree Defines
#define HASH_LENGTH                24
#define ROOT_HASH_LENGTH           32

// Drive Defines
#define SECTOR_SIZE_LEGACY         0x0200
#define SECTOR_SIZE_MODERN         0x1000

// Crypto Defines
#define XVD_KEY_LEN                32
#define XVD_MAX_PE_CATALOGS        4
#define XVD_MAX_PE_CATALOG_CAPS    4
#define XVD_SANDBOX_ID_LENGTH      16
#define XvdHeader_RESERVED        (4096 - (2716 + RSA_SINGATURE_SIZE))

// Other sizes
#define ROAMING_DATA_SIZE          2024

///////////////////////////////////////
// Aux types
///////////////////////////////////////
struct XvdExtEntry
{
    uint32_t code;
    uint32_t length;
    uint64_t offset;
    uint32_t data_length;
    uint32_t reserved;
};

struct XvdFlags
{
    uint32_t ReadOnly              : 1;
    uint32_t EncryptionDisabled    : 1;
    uint32_t DataIntegrityDisabled : 1;
    uint32_t LegacySectorSize      : 1;
    uint32_t ResiliencyEnabled     : 1;
    uint32_t SraReadOnly           : 1;
    uint32_t RegionIdInXts         : 1;
    uint32_t TitleSpecific         : 1;
    uint32_t PointerXvd            : 1;
    uint32_t StreamingRoamable     : 1;
    uint32_t DiffusiveDisabled     : 1;
    uint32_t SpoofedDuid           : 1;
    uint32_t Reserved0             : 1; // Previously TrimSupported. Do not reuse.
    uint32_t TrimSupported         : 1;
    uint32_t RoamingEnabled        : 1;
    uint32_t Reserved              : 17;
};

enum XvdContentType : uint32_t
{
    Data                = 0,
    Title               = 1,
    SystemOS            = 2,
    EraOS               = 3,
    Scratch             = 4,
    ResetData           = 5,
    App                 = 6,
    HostOS              = 7,
    X360STFS            = 8,
    X360FATX            = 9,
    X360GDFX            = 10,
    Updater             = 11,
    OfflineUpdater      = 12,
    Template            = 13,
    MteHost             = 14,
    MteApp              = 15,
    MteTitle            = 16,
    MteEraOS            = 17,
    EraTools            = 18,
    SystemTools         = 19,
    SystemAuxiliary     = 20,
    AcousticModel       = 21,
    Codec               = 22,
    Qaslt               = 23,
    AppDlc              = 24,
    TitleDlc            = 25,
    UniversalDlc        = 26,
    SystemData          = 27,
    Test                = 28,
    HwTest              = 29,
    KioskData           = 30,
    DevPackage          = 31,
    HostProfiler        = 32,
    Roamable            = 33,
    ThinProvisioned     = 34,
    StreamingOnlySra    = 35,
    StreamingOnlyEra    = 36,
    StreamingOnlyHost   = 37,
    QuickResume         = 38,
    HostData            = 39,
};

static const char* ContentTypeStr(XvdContentType type)
{
    switch(type)
    {
        case Data:              return "Data";
        case Title:             return "Title";
        case SystemOS:          return "SystemOS";
        case EraOS:             return "EraOS";
        case Scratch:           return "Scratch";
        case ResetData:         return "ResetData";
        case App:               return "App";
        case HostOS:            return "HostOS";
        case X360STFS:          return "X360STFS";
        case X360FATX:          return "X360FATX";
        case X360GDFX:          return "X360GDFX";
        case Updater:           return "Updater";
        case OfflineUpdater:    return "OfflineUpdater";
        case Template:          return "Template";
        case MteHost:           return "MteHost";
        case MteApp:            return "MteApp";
        case MteEraOS:          return "MteEraOS";
        case EraTools:          return "EraTools";
        case SystemTools:       return "SystemTools";
        case SystemAuxiliary:   return "SystemAuxiliary";
        case AcousticModel:     return "AcousticModel";
        case Codec:             return "Codec";
        case Qaslt:             return "Qaslt";
        case AppDlc:            return "AppDlc";
        case TitleDlc:          return "TitleDlc";
        case UniversalDlc:      return "UniversalDlc";
        case SystemData:        return "SystemData";
        case Test:              return "Test";
        case HwTest:            return "HwTest";
        case KioskData:         return "KioskData";
        case DevPackage:        return "DevPackage";
        case HostProfiler:      return "HostProfiler";
        case Roamable:          return "Roamable";
        case ThinProvisioned:   return "ThinProvisioned";
        case StreamingOnlySra:  return "StreamingOnlySra";
        case StreamingOnlyEra:  return "StreamingOnlyEra";
        case StreamingOnlyHost: return "StreamingOnlyHost";
        case QuickResume:       return "QuickResume";
        case HostData:          return "HostData";
        
        //case 
        default: return "UNKNOWN";
    }
}

struct XvdPlatformMask
{
    uint8_t pc_supported      : 1;
    uint8_t gen8_supported    : 1;
    uint8_t gen9_supported    : 1;
    uint8_t unused            : 5;
};

// Gotten from symbols
enum XvdTrimPhase 
{
    none    = 0,
    shuffle = 1,
    bat     = 2,
};

// Gotten from symbols
struct XvdTrimState
{
    XvdTrimPhase phase;
    int64_t      timestamp;
    uint64_t     blob_size;
};

enum XvdType 
{
    FIXED   = 0,
    DYNAMIC = 1
};

///////////////////////////////////////
// Type definitions
///////////////////////////////////////
struct XvdHeader
{
    uint8_t         rsa_signature[RSA_SINGATURE_SIZE];   // 0x000
    uint8_t         magic[8];                            // 0x200
    XvdFlags        flags;                               // 0x208
    uint32_t        format_version;                      // 0x20C
    uint64_t        creation_time;                       // 0x210
    uint64_t        drive_size;                          // 0x218
    uint8_t         content_id_guid[0x10];               // 0x220 DRIVE id
    uint8_t         user_id[0x10];                       // 0x230 User  id
    uint8_t         root_hash[0x20];                     // 0x240
    uint8_t         xvc_hash[0x20];                      // 0x260
    uint32_t        xvd_type;                            // 0x280
    XvdContentType  content_type;                        // 0x284
    uint32_t        embedded_xvd_length;                 // 0x288
    uint32_t        user_data_length;                    // 0x28C
    uint32_t        xvc_data_length;                     // 0x290
    uint32_t        dynamic_header_length;               // 0x294
    uint32_t        block_size;                          // 0x298

    XvdExtEntry   ExtEntry[4];
    uint16_t        Capabilities[8]; // Capabilities seem unused / unknown

    uint8_t         PECatalogHash[ROOT_HASH_LENGTH];
    // future: check wtf is this (seems to be related to hashTree?)
    uint8_t         exvd_PDUID[16];
    uint8_t         reserved_0[16];

    // This field is the key to decrypt the drive. However, this field itself
    // is encrypted with the respective ODK. This is also called the 
    // encrypted or sealed CIK
    uint8_t         key_material[XVD_KEY_LEN];

    // Seems somewhat unused?
    uint8_t         user_data_hash[ROOT_HASH_LENGTH];
    uint8_t         sandbox_id[XVD_SANDBOX_ID_LENGTH];

    // Data that usually identifies a store package
    // Sort of defined by game developer studio
    uint8_t         ProductId[16];
    // Sort of random ID generated by server-side CDN / store
    uint8_t         PDUID[16];
    // versionID
    uint64_t        PackageVersionNumber;

    uint16_t        PECatalogCaps[XVD_MAX_PE_CATALOGS][XVD_MAX_PE_CATALOG_CAPS];
    uint8_t         PECatalogs[XVD_MAX_PE_CATALOGS][ROOT_HASH_LENGTH];

    //  These two fields are only used for writeable encrypted XVDs. They define data
    //  that is required to regenerate the key.
    uint32_t        writeable_expiration_date;
    uint32_t        writeable_policy_flags;

    // Persistent Local Storage
    uint32_t        pls_size;         
    //  Number of pages of mutable metadata for XVCs.    
    uint8_t         mutable_page_num; 

    //  Target the XVD was built for (PC, Xb1, Xbsx/ss)
    XvdPlatformMask platforms_supported;

    //  PLS: Persistent Local Storage. How much this XVD has? IDK yet
    uint32_t        max_pls_size; 

    // For XVDs created by a xCloud remote machine, this seems to indicate
    // what type of server created it. Useless but the symbols gave us an
    // enum to identify it.
    uint8_t         server_console_mode;
    uint8_t         unused[13]; 

    // There seems to be an OS internal struct which is a variation of XVD, called PointerXVD
    // (seems to be related with cloud streaming in PC and Xbox), and these "XVDs" don't
    // contain an eXVD but rather fetch some sort of remote blob (from where?). 
    // I guess it's some sort of "virtual" or "remote" XVD. This field indicates the size of that blob. 
    // Regardless, it's unlikely that we'll ever need to parse this field.
    uint64_t        remote_blob_size;

    // Seems to be related with HashTree code but absolutely unused.
    int64_t         sequence_number;

    // Minimum version the SP (or hostOS?) must run to work with / mount this XVD. 0 means it's ignored / always accepted
    uint64_t        min_sp_ver;

    // If XVD contains crypto material, said material is encrypted with an ODK.
    // If this field is zero, ODK must be inferred from content type. Otherwise
    // the value identifies which ODK must be used.
    // Some "theory of operation" about this would be appreciated later when I understand it better
    uint32_t        odk_id;

    // Roaming information (format unknown)
    uint8_t         roaming_header[ROAMING_DATA_SIZE];

    // Flag that seems to be used by the hypervisor when operating with Dynamic XVDs
    // Depending on the flag type, the BAT will be updated differently.
    // On an XVD file in disk, I guess this flag could be used to identify if the
    // XVD has been somewhat used by a real Xbox or rather was just generated artificially.
    XvdTrimState    trim_state;

    // Note: seems like some bits are used in modern XVDs, but for unknown purposes
    uint8_t         reserved[XvdHeader_RESERVED];

} __attribute__ ((__packed__));

struct GUID 
{          
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
}; // size is 16 bytes

//////////////////////////////////////////
// AUXILIARY METHODS                    //
//////////////////////////////////////////

// XVD Layout math methods
uint32_t BytesToPages(uint64_t size_bytes);
uint64_t PagesToBytes(uint64_t num_pages);
uint64_t BytesToBlocks(uint64_t size_bytes);
uint64_t BlocksToBytes(uint64_t num_blocks);
uint64_t PagesToBlocks(uint32_t num_pages);
uint64_t AlignSizeToPageBoundary(uint64_t size_bytes);

// Microsoft formats conversion methods
std::string MsGUIDToString(GUID guid);
std::string MsVersionToString(uint64_t version, bool extended);
std::string FiletimeToString(uint64_t filetime);