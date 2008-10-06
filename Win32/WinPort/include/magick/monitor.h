/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Progress Monitor Methods.
*/
#ifndef _MAGICK_MONITOR_H
#define _MAGICK_MONITOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Monitor typedef declarations.
*/
typedef unsigned int
  (*MonitorHandler)(const char *text,const magick_int64_t quantum,
    const magick_uint64_t span,ExceptionInfo *exception);

/*
  Monitor declarations.
*/
extern MagickExport MonitorHandler
  SetMonitorHandler(MonitorHandler handler);

extern MagickExport unsigned int
  MagickMonitor(const char *text,
  const magick_int64_t quantum,const magick_uint64_t span,
  ExceptionInfo *exception);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
