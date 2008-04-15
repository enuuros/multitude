
/** This file is part of the Radiant library.*/

#ifndef CONFIG_READER_EMPTY_HPP
#define CONFIG_READER_EMPTY_HPP

/** @file 
    Empty declarations of the classes. Use this header to avoid loading the more
    complex stuff into all of your files.
*/

namespace Radiant {

  class Variant;
  template <class T> class ChunkT;

  typedef ChunkT<Variant> Chunk;
  typedef ChunkT<Chunk>   Config;

}

#endif
