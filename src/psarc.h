/*
 * Open PSARC PS3 extractor
 * Copyright (C) 2011-2018 Matthieu Milan
 */

#ifndef PSARC_H__
#define PSARC_H__

#include "file.h"

#define kPSARCMagicNumber 0x50534152

#define BUFFER_SIZE 600*1024				//added defines for a few literals
#define TEMP_FILE "psarc.temp"				//that I had to modify
#ifndef __linux__							//getting windows temp path requires
	#define TEMP_DIR  "."					//filesystem.h which isn't an option
	#define TEMP_PATH "./psarc.temp"		//using working directory for temp file
	#define PRId64 "lld"
#else
	#define TEMP_DIR  "/tmp"
	#define TEMP_PATH "/tmp/psarc.temp"
#endif


struct Pack {
	int32_t _id;
	uint64_t _length;
	char *_name;
	uint32_t _zIndex;
	uint64_t _zOffset;
};

struct PSARC {
	File _f;
	uint8_t *_buffer;

	Pack *_entries;
	uint32_t _numEntries;
	uint32_t _maxLen;

	PSARC();
	~PSARC();

	void inflateEntry(uint32_t entry, uint32_t *zBlocks, uint32_t cBlockSize, char *basename, char *dirname);
	void read(const char *arcName, uint32_t start, uint32_t end, bool header, bool console = false);
	void read(const char *arcName, uint32_t start, uint32_t end);
	void readHeader(const char *arcName, bool console = false);
};

#ifndef __linux__
	char * strndup (const char *s, size_t n); //not implemented in mingw
#endif

#endif // PSARC_H__
