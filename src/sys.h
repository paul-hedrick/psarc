/*
 * Open PSARC PS3 extractor
 * Copyright (C) 2011-2018 Matthieu Milan
 */

#ifndef SYS_H__
#define SYS_H__

#define _FILE_OFFSET_BITS 64

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#ifndef __linux__
	#include <direct.h>  //needed for mkdir in windows
	#define fseeko fseek //no fseeko in mingw, fseek is not usable with text files
	#define __mkdir__(p,m) mkdir(p) //windows doesn't have perms
#else
	#define __mkdir__(p,m) mkdir(p,m) //and linux does
#endif

inline uint16_t READ_BE_UINT16(const uint8_t *ptr) {
	return (ptr[0] << 8) | ptr[1];
}

inline uint32_t READ_BE_INT24(const uint8_t *ptr) {
	return (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];
}

inline uint32_t READ_BE_UINT32(const uint8_t *ptr) {
	return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

inline uint64_t READ_BE_INT40(const uint8_t *ptr) {
	uint64_t res = ptr[4] | (0x100L * ptr[3]) | (ptr[2] * 0x10000L) | (ptr[1] * 0x1000000L) | (ptr[0] * 0x100000000LLU);
	if (ptr[0] == 0)
		res %= 0xffffffff00000000LLU;
	else
		res %= 0xfffffffe00000000LLU;
	return res;
}

inline int mkpath(const char *s, mode_t mode){
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int rv;

	rv = -1;
	if (strcmp(s, ".") == 0 || strcmp(s, "/") == 0)
		return (0);

	if ((path = strdup(s)) == NULL)
		exit(1);

	if ((q = strdup(s)) == NULL)
		exit(1);

	if ((r = dirname(q)) == NULL) {
		free(q);
		free(path);
		return (rv);
	}

	if ((up = strdup(r)) == NULL)
		exit(1);

	if ((mkpath(up, mode) == -1) && (errno != EEXIST)) {
		free(up);
		free(q);
		free(path);
		return (rv);
	}

	if ((__mkdir__(path, mode) == -1) && (errno != EEXIST))
		rv = -1;
	else
		rv = 0;

	free(up);
	free(q);
	free(path);
	return (rv);
}

#endif // SYS_H__
