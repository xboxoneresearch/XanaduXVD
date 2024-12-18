@echo off
REM Builds the XanaduCLI app. -I./src specifies that headers are in the /src folder (that's where XanaduXVD lives)
REM -I/usr/include is added for the Cygwin environment to find the openssl headers at /usr/include/openssl/evp.h (libssl-devel)

@echo on
g++ -std=c++20 -I./src -L/usr/lib .\XanaduCLI\XanaduCLI.cpp .\src\XanaduXVD.cpp .\src\XVDTypes.cpp -lssl -lcrypto