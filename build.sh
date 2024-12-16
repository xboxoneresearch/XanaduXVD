#!/usr/bin/bash
# Builds the XanaduCLI app. -I./src specifies that headers are in the /src folder (that's where XanaduXVD lives)
g++ -I./src .\XanaduCLI\XanaduCLI.cpp .\src\XanaduXVD.cpp .\src\XVDTypes.cpp