/**********************************************************/
/*                       XanaduCLI                        */
/* Terminal tool to manipulate XVD files using XanaduXVD  */
/*                  2024 (c) TorusHyperV                  */
/**********************************************************/

///////////////////////////////////////
// Project includes
///////////////////////////////////////
#include "..\src\XanaduXVD.h"
#include <getopt.h>

void PrintHelp()
{
    char help[] = "XanaduXVD CLI Usage:\n" 
                  "  Usage: %s [xvd filepath]\n"\
                  "Options:\n"\
                  " --file:                           Specifies the input XVD file\n"\
                  " --info:                           Displays information about the XVD\n"\
                  " --extract_exvd [output_filename]: Extract Embedded XVD\n"\
                  " --extract_udat [output_filename]: Extract UserData\n"\
                  " --verify_htree:                   Verify HashTree\n"\
                  " --rebuild_htree:                  Rebuild HashTree\n"\
                  " --help:  Show help\n";

    printf("%s", help);
}

// soon to be: XanaduXVDCli
int main(int argc, char *argv[])
{
    int ret = 0;

    // man getopt_long for info
    const option long_opts[] = 
    {
        {"file",          required_argument,    nullptr, 'f'},
        {"info",          no_argument,          nullptr, 'i'},
        {"extract_exvd",  required_argument,    nullptr, 'e'},
        {"extract_udat",  required_argument,    nullptr, 'u'},
        {"verify_htree",  no_argument,          nullptr, 'v'},
        {"rebuild_htree", no_argument,          nullptr, 'r'},
        {"help",          no_argument,          nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };
    int long_index = 0;
    int opt = 0;

    // Keep track of requested options
    bool infodump     = false;
    bool extract_exvd = false;
    bool extract_udat = false;
    bool verify_hasht = false;
    bool rebuild_hash = false;
    char* filename    = nullptr;

    const char* const short_opts = "f:euvrh";
    while( (opt = getopt_long(argc, argv, short_opts, long_opts, &long_index)) != -1 )
    {
        switch(opt)
        {
            case 'i':
                infodump = true;
                break;
            case 'e':
                extract_exvd = true;
                break;
            case 'u':
                extract_udat = true;
                break;
            case 'v':
                verify_hasht = true;
                break;
            case 'r':
                rebuild_hash = true;
                break;
            case 'h':
                PrintHelp();
                exit(0);
                break;
            case 'f':
                filename = optarg;
                break;
            default: 
                PrintHelp();
                exit(0);
        }
    }

    if(filename == nullptr)
    {
        printf("No XVD file passed. Please use --file or -f\n");
        PrintHelp();
        exit(0);
    }

    
    // Create XanaduXVD object
    XanaduXVD xvd(filename);

    // Start XanaduXVD
    if(auto ret = xvd.Start(false, true); ret)
    {
        fprintf(stderr, "Failed to manipulate XVD: %s - reason: %d\n", argv[0], ret);
        return 1;
    }

    if(infodump)
        xvd.InfoDump();

    // go laugh have fun
    //xvd.ExtractEmbeddedXVD("exvd.xvd");

    // TODO Create enum of errors in XanaduXVD.h
    return 0;
}