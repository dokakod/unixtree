/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-1996 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* $Id: zutil.c,v 1.1.1.1 1997/03/19 15:06:41 kivinen Exp $ */

#include <stdio.h>
#include "zutil.h"
#include "libsys.h"

struct internal_state      { int dummy; }; /* for buggy compilers */

const char *z_errmsg[] =
{
	"need dictionary",     /* Z_NEED_DICT       2  */
	"stream end",          /* Z_STREAM_END      1  */
	"",                    /* Z_OK              0  */
	"file error",          /* Z_ERRNO         (-1) */
	"stream error",        /* Z_STREAM_ERROR  (-2) */
	"data error",          /* Z_DATA_ERROR    (-3) */
	"insufficient memory", /* Z_MEM_ERROR     (-4) */
	"buffer error",        /* Z_BUF_ERROR     (-5) */
	"incompatible version",/* Z_VERSION_ERROR (-6) */
	""
};


const char *zlibVersion()
{
    return ZLIB_VERSION;
}

#ifdef DEBUG
void z_error (char *m)
{
    fprintf(stderr, "%s\n", m);
    exit(1);
}
#endif

void * zcalloc (
    void * opaque,
    unsigned items,
    unsigned size)
{
    return CALLOC(items, size);
}

void  zcfree (
    void * opaque,
    void * ptr)
{
    FREE(ptr);
}
