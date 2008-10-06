/*
  Copyright (C) 2007 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Interfaces specific to Unix.
 
*/
#ifndef _UNIX_PORT_H
#define _UNIX_PORT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport int
  MagickSpawnVP(const unsigned int verbose, const char *file, char *const argv[]);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _UNIX_PORT_H */
