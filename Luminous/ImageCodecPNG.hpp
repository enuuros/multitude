#ifndef CODEC_PNG_HPP
#define CODEC_PNG_HPP

#include <Luminous/ImageCodec.hpp>

namespace Luminous
{

  /// Image codec to handle PNG files
  class ImageCodecPNG : public ImageCodec
  {
    public:
      virtual ~ImageCodecPNG();
      virtual bool canRead(FILE * file);

      virtual std::string extensions() const;
      virtual std::string name() const;
      virtual bool ping(ImageInfo & info, FILE * file);
      virtual bool read(Image & image, FILE * file);
      virtual bool write(const Image & image, FILE * file);
  };

}

#endif
