/*
 *  p3t.h
 *  
 *
 *  Created by Johannes Wilke on 2008/08/16.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef P3T__H__
#define P3T__H__

#include <string>
#include <list>
#include <stdio.h>
#include <list>
#include <map>
#include <string>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include "zlib.h"

typedef unsigned long p3tInt;
typedef float p3tFloat;
typedef std::string p3tString;
typedef std::string p3tId;

typedef struct
{
  unsigned char* file_data;	// 4 byte
  size_t file_size;			// 4 byte
} p3tGIM;					// 8 byte

typedef struct
{
  unsigned char* file_data;
  size_t file_size;
} p3tJPG;

typedef struct
{
  unsigned char* file_data;
  size_t file_size;
} p3tVAG;

struct p3tSoundEffectElement
{
  p3tId id;
  p3tVAG left;
  p3tVAG right;
};
typedef std::list<p3tSoundEffectElement> p3tSoundEffectList;

struct p3tSoundEffectTableElement
{
  p3tSoundEffectList se;  
};

struct p3tColorElement
{
  p3tInt selection;
};

struct p3tFontElement
{
  p3tInt selection;
};

struct p3tBGImageElement
{
  p3tJPG hd;
  p3tJPG sd;
};
typedef std::list<p3tBGImageElement> p3tBGImageList;

struct p3tBGImageTableElement
{
  p3tBGImageList bgimage;
};

struct p3tNotificationElement
{
  p3tGIM src;
  p3tInt size;
};

struct p3tPointerElement
{
  p3tInt base_x;
  p3tGIM src;
  p3tInt size;
  p3tId id;
  p3tInt base_y;
};
typedef std::list<p3tPointerElement> p3tPointerList;

struct p3tPointerTableElement
{
  p3tPointerList pointer;
};

struct p3tIconElement
{
  p3tGIM src;
  p3tId id;
  p3tInt size;
};
typedef std::list<p3tIconElement> p3tIconList;

struct p3tIconTableElement
{
  p3tIconList icon;  
};

struct p3tLocalizedInfoElement
{
  p3tId locale;
  p3tString comment;
  p3tString name;
  p3tString author;
  p3tString url;
  p3tString version;
  p3tString genre;
};
typedef std::list<p3tLocalizedInfoElement> p3tLocalizedInfoList;

struct p3tInfoElement
{
  p3tString comment;
  p3tInt previewsize;
  p3tString name;
  p3tString author;
  p3tString url;
  p3tInt authoriconsize;
  p3tGIM authoricon;
  p3tInt iconsize;
  p3tString version;
  p3tString mtime;
  p3tString genre;
  p3tGIM preview;
  p3tGIM icon;
};

struct p3tInfoTableElement
{
  struct p3tInfoElement info;
  p3tLocalizedInfoList localizedinfo;
};

struct p3tTheme
{
  struct p3tInfoTableElement infotable;
  struct p3tSoundEffectTableElement setable;
  struct p3tIconTableElement icontable;
  struct p3tPointerTableElement pointertable;
  struct p3tNotificationElement notification;
  p3tBGImageTableElement bgimagetable;
  p3tFontElement font;
  p3tColorElement color;
  
  void clear();
  p3tLocalizedInfoList::iterator findLocalizedInfo(p3tId locale);
  p3tSoundEffectList::iterator findSoundEffect(p3tId id);
  p3tIconList::iterator findIcon(p3tId id);
  p3tPointerList::iterator findPointerIcon(p3tId id);  
};

enum 
{
  P3T_OK,
  P3T_CANNOT_OPEN_FILE,
  P3T_OUT_OF_MEMORY,
  P3T_COULD_NOT_READ_HEADER,
  P3T_COULD_NOT_READ_STRING_TABLE,
  P3T_COULD_NOT_READ_ID_TABLE,
  P3T_CORRUPT_P3T_FILE
};
extern int p3tprevRip(const char* p3t_filename,char* destdir, p3tTheme& p3t_theme);
extern int p3tRip(const char* p3t_filename,char* destdir, p3tTheme& p3t_theme);

// GIM

enum {
  GIM_OK = 0,
  GIM_CANNOT_OPEN_READ_FILE,
  GIM_CANNOT_CREATE_WRITE_FILE,
  GIM_OUT_OF_MEMORY,
  GIM_CANNOT_READ_FILE,
  GIM_CANNOT_INITIALIZE,
  GIM_COMPRESSION_FAILURE,
  GIM_CANNOT_WRITE_FILE  
};

typedef enum {
  GIM_COMPRESS,
  GIM_UNCOMPRESS
} gimCompressionDirection;

extern int compressGIMBuffer(void* gim_buffer, size_t gim_buffer_size, void* compressed_buffer, size_t compressed_buffer_size);
extern long uncompressGIMBuffer(void* compressed_buffer, size_t compressed_buffer_size, void** gim_buffer);
extern long gimCompression(char* read_filename, char* write_filename, gimCompressionDirection direction);

// GIM END

extern bool extracthd;
extern bool extractsd;
extern int vag2wav( const char* vagfile, const char* pcmfile);
extern int p3textract(const char* p3t_filename,char* destdir);
extern int p3tprevextract(const char* p3t_filename,char* destdir);
extern int p3tcolor;

#endif
