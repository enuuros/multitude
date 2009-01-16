#ifndef CODEC_REGISTERY_HPP
#define CODEC_REGISTERY_HPP

#include <vector>
#include <map>
#include <string>

namespace Luminous
{
  class ImageCodec;

  /** CodecRegistry keeps track of different registered ImageCodecs that can be
   * used to load images. **/
  class CodecRegistry
  {
  public:
    CodecRegistry();
    ~CodecRegistry();

    /// Try to get a codec that could load a given file.
    /// @param filename name of the file to load
    /// @param file the file to query
    /// @return returns a pointer to a codec that reports it can load the given
    /// file or NULL if no codec is found.
    ImageCodec * getCodec(const std::string & filename, FILE * file = 0);
    /// Register a new codec that can be used to load images
    /// @param codec the new codec
    void registerCodec(ImageCodec * codec);

  private:
    typedef std::vector<ImageCodec *> Codecs;
    typedef std::multimap<std::string, ImageCodec *> Aliases;

    Codecs m_codecs;
    Aliases m_aliases;
  };

}

#endif
