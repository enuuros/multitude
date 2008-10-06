/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick types typedefs
*/

#ifndef _MAGICK_TYPES_H
#define _MAGICK_TYPES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Assign ANSI C stdint.h-like typedefs based on the sizes of native types
  magick_int8_t   --                       -128 to 127
  magick_uint8_t  --                          0 to 255
  magick_int16_t  --                    -32,768 to 32,767
  magick_uint16_t --                          0 to 65,535
  magick_int32_t  --             -2,147,483,648 to 2,147,483,647
  magick_uint32_t --                          0 to 4,294,967,295
  magick_int64_t  -- -9,223,372,036,854,775,807 to 9,223,372,036,854,775,807
  magick_uint64_t --                          0 to 18,446,744,073,709,551,615
*/

#if defined(WIN32) && !defined(__MINGW32__)

  /* The following typedefs are used for WIN32 */
  typedef signed char   magick_int8_t;
  typedef unsigned char  magick_uint8_t;

  typedef signed short  magick_int16_t;
  typedef unsigned short magick_uint16_t;

  typedef signed int  magick_int32_t;
  typedef unsigned int magick_uint32_t;

  /* Maybe we need to do something different for __BORLANDC__ here? */
  typedef signed __int64  magick_int64_t;
  typedef unsigned __int64 magick_uint64_t;

#else

  /* The following typedefs are subtituted when using Unixish configure */
  typedef @INT8_T@   magick_int8_t;
  typedef @UINT8_T@  magick_uint8_t;

  typedef @INT16_T@  magick_int16_t;
  typedef @UINT16_T@ magick_uint16_t;

  typedef @INT32_T@  magick_int32_t;
  typedef @UINT32_T@ magick_uint32_t;

  typedef @INT64_T@  magick_int64_t;
  typedef @UINT64_T@ magick_uint64_t;

#endif

  /* 64-bit file and blob offset type */
  typedef magick_int64_t magick_off_t;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_TYPES_H */
