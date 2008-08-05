#include <Luminous/VertexBuffer.hpp>

namespace Luminous
{

  template<GLenum type>
  BufferObject<type>::BufferObject()
  {
    glGenBuffers(1, &m_bufferId);
  }

  template<GLenum type>
  BufferObject<type>::~BufferObject()
  {
    glDeleteBuffers(1, &m_bufferId);
  }

  template<GLenum type>
  void BufferObject<type>::allocate(size_t bytes, Usage usage)
  {
    fill(0, bytes, usage); 
  }

  template<GLenum type>
  void BufferObject<type>::bind() const
  {
    glBindBuffer(type, m_bufferId);
  }

  template<GLenum type>
  void BufferObject<type>::unbind() const
  {
    glBindBuffer(type, 0);
  }

  template<GLenum type>
  void BufferObject<type>::fill(void * data, size_t bytes, Usage usage)
  {
    bind();
    glBufferData(type, bytes, data, usage);
  }

  template<GLenum type>
  void BufferObject<type>::partialFill(size_t offsetInBytes, void * data, size_t bytes)
  {
    bind();
    glBufferSubData(type, offsetInBytes, bytes, data);
  }

  template<GLenum type>
  void * BufferObject<type>::map(AccessMode mode)
  {
    bind();
    return glMapBuffer(type, mode);
  }

  template<GLenum type>
  void BufferObject<type>::unmap()
  {
    bind();
    glUnmapBuffer(type);
  }

}
