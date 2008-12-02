#include "CodecRegistry.hpp"
#include "ImageCodec.hpp"

#include <Radiant/StringUtils.hpp>

namespace Luminous
{

  CodecRegistry::CodecRegistry()
  {}

  CodecRegistry::~CodecRegistry()
  {}

  ImageCodec * CodecRegistry::getCodec(const std::string & filename, FILE * file)
  {
    ImageCodec * codec = 0;

    // Try a codec that matches the extension first
    const std::string ext = filename.substr(filename.rfind(".") + 1);
    Aliases::iterator alias = m_aliases.find(ext);

    if(alias != m_aliases.end())
      codec = alias->second;

    if(file) {
     
      // Verify our choice
      if(codec && codec->canRead(file)) 
        return codec;
     
      // No codec matched the extension, go through all registered codecs and
      // see if they match
      for(Codecs::iterator it = m_codecs.begin(); it != m_codecs.end(); it++) {
        ImageCodec * candidate = *it;

        // We already tried this
        if(candidate == codec) continue;

        if(candidate->canRead(file)) 
          return candidate;
      }
    }

    return codec;
  }

  void CodecRegistry::registerCodec(ImageCodec * codec)
  {
    namespace su = Radiant::StringUtils;

    m_codecs.push_back(codec);

    // Associate extensions with this codec
    su::StringList exts;
    su::split(codec->extensions(), " ", exts);

    for(su::StringList::iterator it = exts.begin(); it != exts.end(); it++) {
      m_aliases.insert(std::make_pair(*it, codec));
    }    
  }

  
}
