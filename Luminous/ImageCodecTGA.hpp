#ifndef CODEC_TGA_HPP
#define CODEC_TGA_HPP

#include <Luminous/ImageCodec.hpp>

namespace Luminous
{

  class ImageCodecTGA : public ImageCodec
  {
    public:
      virtual ~ImageCodecTGA();
      virtual bool canRead(FILE * file);

      virtual std::string extensions() const;
      virtual std::string name() const;
      virtual bool ping(ImageInfo & info, FILE * file);
      virtual bool read(Image & image, FILE * file);
      virtual bool write(const Image & image, FILE * file);
  };

}

#endif
