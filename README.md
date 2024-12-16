# XanaduXVD

![Build Ubuntu](https://github.com/xboxoneresearch/XanaduXVD/actions/workflows/c-cpp.yml/badge.svg)
![Build Windows](https://github.com/xboxoneresearch/XanaduXVD/actions/workflows/cygwin-c-cpp.yml/badge.svg)

XanaduXVD is an experimental C++ tool/playground for parsing and manipulating XVD (Xbox Virtual Disk) files, used by Xbox One, Xbox Series, and Windows PC. It aims to serve as a source of technical documentation for the fileformat, since it lacks a public standard.

XanaduXVD is not yet production-ready, you might want to use [xvdtool](https://github.com/emoose/xvdtool) or [XvdTool.Streaming](https://github.com/LukeFZ/XvdTool.Streaming)

# Currently Available Features

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
   
- XanaduGUI: A graphical user interface that uses XanaduXVD
  - XanaduGUI.cpp (COMING SOON!)
 
# Build
Include src/XanaduXVD in your project. For example, to build XanaduCLI do:
`g++ XVDTypes.cpp XanaduXVD.cpp XanaduCLI.cpp`

At the moment this should build everywhere, on Windows (cygwin works great), Linux and MacOS.

# Contribute
Any contributions and [Pull Requests](https://github.com/xboxoneresearch/XanaduXVD/pulls) are welcome, just remember, any code you want merged into the repo should be extensively commented or have accompanying documentation! You may want to check the [Open Issues]((https://github.com/xboxoneresearch/XanaduXVD/issues) for inspiration

# Acknowledgements
Tremendous thanks to tuxuser and msixvc for the technical help and also assisting during my late-night rants about the most (in)significant details about XVD files.
