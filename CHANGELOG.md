# Changelog
All notable changes to this project will be documented in this file.

## [0.1.3.1] - 2019-10-06
### Added
- Added *-h* command to print header information to std out (id:size:full/file/path).

### Changed
- Complete rewrite of Makefile to support Windows and add *debug* target.
- Changed *mkdir* to a macro to support cross platform directory manipulation.
- Windows: Added include for *direct.h* and replacement define for *fseeko* to *fseek*.
- Tweaked formatting of usage message.
- Moved *_buffer* size and temp file path to defines.
- Windows: Added basic implementation of strndup and PRId64 which are missing from MinGW.
- Changed *inflateEntry*'s file open (write) to binary mode to prevent fseek from choking on carriage returns.
- Optimized how file paths are read into entires.
