/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Cache View Methods.
*/
#ifndef _MAGICK_CACHE_VIEW_H
#define _MAGICK_CACHE_VIEW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _ViewInfo
{
  Image
    *image;

  unsigned long
    id;

  unsigned long
    signature;
} ViewInfo;

/*
  MagickExport cache view interfaces.
*/
extern MagickExport const PixelPacket
  *AcquireCacheView(const ViewInfo *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

extern MagickExport IndexPacket
  *GetCacheViewIndexes(const ViewInfo *);

extern MagickExport PixelPacket
  *GetCacheViewPixels(const ViewInfo *),
  *GetCacheView(ViewInfo *,const long,const long,const unsigned long,
    const unsigned long),
  *SetCacheView(ViewInfo *,const long,const long,const unsigned long,
    const unsigned long);

extern MagickExport unsigned int
  SyncCacheView(ViewInfo *);

extern MagickExport ViewInfo
  *OpenCacheView(Image *);

extern MagickExport void
  CloseCacheView(ViewInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
