# XanaduXVD
XanaduXVD is an experimental work-in-progress C++ tool/playground for parsing and manipulating XVD (Xbox Virtual Disk) files, used by Xbox One, Xbox Series, and Windows PC. It aims to serve as a source of technical documentation for the fileformat, since it lacks a public standard.

XanaduXVD is not yet fully featured, you might want to use [xvdtool](https://github.com/emoose/xvdtool) or [XvdTool.Streaming](https://github.com/LukeFZ/XvdTool.Streaming)

# Currently Available Features

- [x] Dumping embedded XVD (Which is usually the game's era.xvd or gameos.xvd)
- [x] Dumping UserData (Usually the [VBI](https://xboxoneresearch.github.io/wiki/boot/vbi/))
- [ ] Package decryption
- [ ] Drive extraction
- [ ] XVC support
- [ ] MSIXVC support
- [ ] UWA/UWP/UWX support
- [ ] Header editor & signature manipulation
- [ ] Hash tree verification
- [ ] Hash tree rebuilding
- [ ] Triming and removal of sections

# Project Structure
- XanaduXVD
  - src
    - XanaduXVD.h : header definition 
    - XanaduXVD.cpp : implementation containing most of the logic for parsing and manipulating XVD files
    - XVDTypes.h : file containing definitions about the format
   
- XanaduCLI
  - A command line utility that uses XanaduXVD
 
- XanaduGUI
  - A graphical user interface that uses XanaduXVD
 
# Build
g++ XanaduXVD.cpp
