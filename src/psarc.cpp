/*
 * Open PSARC PS3 extractor
 * Copyright (C) 2011-2018 Matthieu Milan
 */

#include <inttypes.h>
#include "psarc.h"

PSARC::PSARC() {
	_buffer = (uint8_t *)malloc(BUFFER_SIZE);  //moved literal to define fo use in loop logic
	_entries = NULL;
}

PSARC::~PSARC() {
	_f.close();
	free(_buffer);

	if (_entries != NULL)
		delete _entries;
}

void PSARC::inflateEntry(uint32_t entry, uint32_t *zBlocks, uint32_t cBlockSize, char *basename, char *dirname) {
	uint64_t length = 0;

	if (entry == 0) {
		dirname = (char *)TEMP_DIR;
		basename = (char *)TEMP_FILE;
	}

	File stream;
	if (stream.open(basename, dirname, "wb+")) { //changed to binary mode to avoid issues with carriage returns
		if (_entries[entry]._length != 0) {
			_f.seek(_entries[entry]._zOffset);
			uint32_t zIndex = _entries[entry]._zIndex;
			do {
				if (zBlocks[zIndex] == 0) {
					_f.read(_buffer, cBlockSize);
					stream.write(_buffer, cBlockSize);
				} else {
					uint16_t isGzipped = _f.readUint16BE(_buffer);
					_f.shift(-2);
					_f.read(_buffer, zBlocks[zIndex]);
					if (isGzipped == 0x78da) {
						uint8_t *uncompressData;
						uLongf uncompressSize;
						uint32_t val = _entries[entry]._length - (zIndex - _entries[entry]._zIndex) * cBlockSize;
						if (val < cBlockSize) {
							uncompressData = (uint8_t *)malloc(val);
							uncompressSize = (uLongf)val;
						} else {
							uncompressData = (uint8_t *)malloc(cBlockSize);
							uncompressSize = (uLongf)cBlockSize;
						}
						uncompress(uncompressData, &uncompressSize, _buffer, zBlocks[zIndex]);
						stream.write(uncompressData, uncompressSize);
						free(uncompressData);
					} else
						stream.write(_buffer, zBlocks[zIndex]);
				}
				zIndex++;
			} while (stream.offset() < _entries[entry]._length);
		}

		if (entry == 0) {
			length = stream.offset();
			stream.close();

			File reader;
			reader.open(basename, dirname, "r");

			_entries[0]._name = (char *)"NamesBlock.bin";
			for (uint32_t i = 1; i < _numEntries; i++) {	//This read loop was reworked to not use fseeko
				uint8_t count = 0;							//fseeko is not imnplemented on minggw and fseek
															//chokes on text files.  This method is faster anyway.
				_buffer[count] = reader.readByte();
				while ((_buffer[count] != 10) && (reader.offset() < length) && count < (BUFFER_SIZE-1)) {
					count++;
					_buffer[count] = reader.readByte();	
				}
				uint8_t plength = (_buffer[count] == 10) ? count : (count + 1); //EOL = count, EOF = count +1
				_entries[i]._name = strndup((char *)_buffer, plength);
			}

			reader.close();
			remove(TEMP_PATH);
		} else {
			if (stream.offset() != _entries[entry]._length)
				printf("File size : %" PRId64 " bytes. Expected size: %" PRId64 " bytes\n", stream.offset(), _entries[entry]._length);

			stream.close();
		}
	}
}

void PSARC::read(const char *arcName, uint32_t start, uint32_t end, bool header, bool console) {
	char *dirNamec = strdup(arcName);
	char *fileNamec = strdup(arcName);

	char *dirName = dirname(dirNamec);
	char *fileName = basename(fileNamec);

	if (_f.open(fileName, dirName)) {
		if (_f.readUint32BE(_buffer) == kPSARCMagicNumber) {
			_f.seek(8);
			if (_f.readUint32BE(_buffer) == 0x7a6c6962) {
				uint32_t zSize = _f.readUint32BE(_buffer);
				_f.seek(20);
				_numEntries = _f.readUint32BE(_buffer);
				_f.seek(24);
				uint32_t cBlockSize = _f.readUint32BE(_buffer);

				uint8_t zType = 1;
				uint32_t i = 256;
				do {
					i *= 256;
					zType = (uint8_t)(zType + 1);
				} while (i < cBlockSize);

				_f.seek(32);
				_entries = new Pack[_numEntries];
				for (uint32_t i = 0; i < _numEntries; i++) {
					_f.shift(16);

					_entries[i]._id = i;
					_entries[i]._zIndex = _f.readUint32BE(_buffer);
					_entries[i]._length = _f.readInt40BE(_buffer);
					_entries[i]._zOffset = _f.readInt40BE(_buffer);
					_maxLen = _maxLen >= _entries[i]._length ? _maxLen : _entries[i]._length;
				}

				uint32_t numBlocks = (zSize - (uint32_t)_f.offset()) / zType;
				uint32_t *zBlocks = new uint32_t[numBlocks];
				for (uint32_t i = 0; i < numBlocks; i++) {
					switch (zType) {
						case 2:
							zBlocks[i] = _f.readUint16BE(_buffer);
							break;

						case 3:
							zBlocks[i] = _f.readInt24BE(_buffer);
							break;

						case 4:
							zBlocks[i] = _f.readUint32BE(_buffer);
							break;
					}
				}

				char *baseDir = NULL;
				char ext[] = ".psarc";
				if (strlen(fileName) >= 6 && strncmp(fileName + strlen(fileName) - strlen(ext), ext, strlen(ext)) == 0) {
					baseDir = (char *)malloc(strlen(fileName) - strlen(ext) + 1);
					snprintf(baseDir, strlen(fileName) - strlen(ext) + 1, "%s", fileName);
				} else {
					char data[] = "_data";

					baseDir = (char *)malloc(strlen(fileName) + strlen(data) + 1);
					snprintf(baseDir, strlen(fileName) + strlen(data) + 1, "%s%s", fileName, data);
				}

				inflateEntry(0, zBlocks, cBlockSize, NULL, NULL);

				if (header) {
					char ext[] = ".txt";
					char *outList = (char *)malloc(strlen(fileName) + strlen(ext) + 1);
					snprintf(outList, strlen(fileName) + strlen(ext) + 1, "%s%s", fileName, ext);

					if (console) {
						for (uint32_t i = 1; i < _numEntries; i++)
							printf("%d:%" PRId64 ":%s\n", i, _entries[i]._length, _entries[i]._name);
					} else {
						File list;
						if (list.open(outList, ".", "w")) {
							char * msg = (char *) malloc(_maxLen +1);
							for (uint32_t i = 1; i < _numEntries; i++) {
								if (_entries[i]._length < 0x400L)
									sprintf(msg, "%d %" PRId64 " b %s\n", i, _entries[i]._length, _entries[i]._name);
								else if (_entries[i]._length < 0x100000L)
									sprintf(msg, "%d %1.2f Kb %s\n", i, _entries[i]._length / 1024.0, _entries[i]._name);
								else if (_entries[i]._length < 0x40000000L)
									sprintf(msg, "%d %1.2f Mb %s\n", i, _entries[i]._length / 1048576.0, _entries[i]._name);
								else
									sprintf(msg, "%d %1.2f Gb %s\n", i, _entries[i]._length / 1073741824.0, _entries[i]._name);

								list.write(msg, strlen(msg));
							}
							free(msg);
							list.close();
						}
					}
					free(outList);
				} else {
					bool flag = true;
					if (start == 0) {
						start = 1;
						end = _numEntries;
					} else if ((start > (_numEntries - 1)) || (end > (_numEntries - 1))) {
						flag = false;
					} else {
						end++;
					}

					if (flag) {
						__mkdir__(baseDir, 0777);
						
						for (uint32_t i = start; i < end; i++) {
							printf("%i %s\n", _entries[i]._id, _entries[i]._name);

							char *subOutDirc = strdup(_entries[i]._name);
							char *outFilec = strdup(_entries[i]._name);

							char *subOutDir = dirname(subOutDirc);
							char *outFile = basename(outFilec);
							char *outDir;
							if (strncmp("/", _entries[i]._name, 1) == 0) {
								outDir = (char *)malloc(strlen(baseDir) + strlen(subOutDir) + 1);
								snprintf(outDir, strlen(baseDir) + strlen(subOutDir) + 1, "%s%s", baseDir, subOutDir);
							} else {
								outDir = (char *)malloc(strlen(baseDir) + strlen(subOutDir) + 2);
								snprintf(outDir, strlen(baseDir) + strlen(subOutDir) + 2, "%s/%s", baseDir, subOutDir);
							}

							mkpath(outDir, 0777);

							inflateEntry(i, zBlocks, cBlockSize, outFile, outDir);

							free(outDir);
							free(subOutDirc);
							free(outFilec);
							free(_entries[i]._name);
						}
					}
				}

				free(baseDir);
				delete zBlocks;
			}
		} else
			printf("Compression type is not zlib... Aborting.");
	}

	free(dirNamec);
	free(fileNamec);
}

void PSARC::read(const char *arcName, uint32_t start, uint32_t end) {
	read(arcName, start, end, false);
}

void PSARC::readHeader(const char *arcName, bool console) {
	read(arcName, 0, 0, true, console);
}

#ifndef __linux__
	char * strndup (const char *s, size_t n)
	{
		char *cpy = (char *)malloc(n + 1);
		memcpy(cpy, s, n);
		cpy[n] = '\0';
		return cpy;
	}
#endif
