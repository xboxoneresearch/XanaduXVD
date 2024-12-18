# XanaduXVD

![Build Ubuntu](https://github.com/xboxoneresearch/XanaduXVD/actions/workflows/c-cpp.yml/badge.svg)
![Build Windows](https://github.com/xboxoneresearch/XanaduXVD/actions/workflows/cygwin-c-cpp.yml/badge.svg)

XanaduXVD is an experimental C++ tool/playground for parsing and manipulating XVD (Xbox Virtual Disk) files, used by Xbox One, Xbox Series, and Windows PC. It aims to become a source of technical documentation for the fileformat in the future, since it lacks a public standard.

XanaduXVD is not yet production-ready, you might want to use [xvdtool](https://github.com/emoose/xvdtool) or [XvdTool.Streaming](https://github.com/LukeFZ/XvdTool.Streaming)

# Currently Available User-Facing Features

- [x] Dumping embedded XVD (Which is usually the game's era.xvd or gameos.xvd)
- [x] Dumping UserData (Usually the [VBI](https://xboxoneresearch.github.io/wiki/boot/vbi/))
- [ ] Package decryption
- [ ] Drive extraction
- [ ] XVC support
- [ ] MSIXVC support
- [ ] UWA/UWP/UW9 support
- [ ] Header editor & signature validation & signature manipulation
- [ ] Hash tree verification
- [ ] Hash tree rebuilding
- [ ] Trimming and removal of sections

# Project Structure
- XanaduXVD
  - src
    - XanaduXVD.h   : header definitions 
    - **XanaduXVD.cpp** : implementation containing most of the logic for parsing and manipulating XVD files
    - **XVDTypes.h**    : file containing definitions about the format
    - XVDTypes.cpp  : file containing auxiliary methods to manipulate XVD fields and data structures

- XanaduCLI: A command line utility that uses XanaduXVD
  - XanaduCLI.cpp (requires XanaduXVD)
   
- XanaduGUI: A graphical user interface using ftxui, that uses XanaduXVD
  - ftxui_proj
    - src
      - main.cpp (this is mostly a demo for now)
 
# Build
At the moment this should build everywhere, on Windows (cygwin works great), Linux and MacOS (although little endian machines are not yet supported, need to finish deserialization code).

## XanaduCLI
Use the build.sh and build.bat scripts included in the project.

## XanaduGUI
`cd into the project folder`
`cmake .`
`make`

# Contribute
Any contributions and [Pull Requests](https://github.com/xboxoneresearch/XanaduXVD/pulls) are welcome, any code that goes into the repo should be extensively commented or have accompanying documentation! You may want to check the [Open Issues]((https://github.com/xboxoneresearch/XanaduXVD/issues) for inspiration

# Acknowledgements
Thanks to tuxuser and msixvc for the technical help and also assisting during my late-night rants about the most (in)significant details about XVD files.

# Licensing
This software is licensed under the terms of the GPLv2, with exemptions for specific projects noted below.
You can find a copy of the license in the LICENSE file.