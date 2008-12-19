#ifndef CODEC_REGISTERY_HPP
#define CODEC_REGISTERY_HPP

#include <vector>
#include <map>
#include <string>

namespace Luminous
{
  class ImageCodec;

  class CodecRegistry
  {
  public:
    CodecRegistry();
    ~CodecRegistry();

    ImageCodec * getCodec(const std::string & filename, FILE * file = 0);
    void registerCodec(ImageCodec * codec);

  private:
    typedef std::vector<ImageCodec *> Codecs;
    typedef std::multimap<std::string, ImageCodec *> Aliases;

    Codecs m_codecs;
    Aliases m_aliases;
  };

}

#endif
