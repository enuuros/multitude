/* COPYRIGHT
 *
 * This file is part of TimeLine.
 *
 * Copyright: MultiTouch Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2007-2008
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from MultiTouch Oy.
 *
 * See file "TimeLine.hpp" for authors and more details.
 *
 */

#ifndef LUMINOUS_CPU_MIPMAPS_HPP
#define LUMINOUS_CPU_MIPMAPS_HPP

#include <Luminous/BGThread.hpp>
#include <Luminous/Image.hpp>
#include <Luminous/Task.hpp>

#include <Nimble/Vector2.hpp>

#include <Radiant/RefPtr.hpp>

namespace Luminous {

  class GLResources;
  class GPUMipmaps;
  
  class CPUMipmaps
  {
  private:
  public:
    
    friend class GPUMipmaps;

    CPUMipmaps();
    virtual ~CPUMipmaps();

    /** Drop old CPU mipmaps from memory.

	@param dt delta-time

	@param purgeTime The time-limit for keeping CPUMipmaps in
	memory. If purgeTime is less than zero, the mipmap idle times
	are updated, but they are <B>not</B> deleted from memory.
     */
    void update(float dt, float purgeTime);

    /** Returns the index of the mipmap level that would best match
	the actual output pixel resolution. */
    int getOptimal(Nimble::Vector2f size);
    /** Returns the index of the closest available mipmap-level. */
    int getClosest(Nimble::Vector2f size);
    /** Gets the mipmap image on level i. If the level does not
	contain a valid mipmap, then 0 is returned. */
    Image * getImage(int i);
    /** Mark an image used. This method resets the idle-counter of the
	level, preventing it from being dropped from the memory in the
	near future. */
    void markImage(int i);

    /** Starts to load given file, and build the mipmaps. */
    bool startLoading(const char * filename, bool immediate);

    /** Returns the native size of the image, in pixels. */
    const Nimble::Vector2i & nativeSize() const { return m_nativeSize;}
  
    /** Fetch corresponding GPU mipmaps from a resource set. If the
	GPUMipmaps object does not exist yet, it is created and
	returned. */
    GPUMipmaps * getGpuMipmaps(GLResources *);
    
    /** Returns the highest possible mipmap-level. This is basically
	the level of the mipmap with native resolution. */
    int maxLevel() const { return m_maxLevel; }
    /** Returns the lowest mipmap level that is ever going to be
	created. */
    int lowestLevel() const { return 5; }
    /** Returns true if the mipmaps are still being loaded. */
    bool isActive();
    /** Returns the aspect ratio of the image. */
    inline float aspect() const
    { return (float)m_nativeSize.x / (float)m_nativeSize.y; }
  private:

    class CPUItem;
    
    class Loader : public Task
    {
    public:
      friend class CPUItem;
      Loader(Luminous::Priority prio,
	     CPUItem * dest, const std::string file);
      virtual ~Loader();

      virtual void doTask();

    private:

      CPUItem   * m_dest;
      std::string m_file;
    };

    class Scaler : public Task
    {
    public:
      friend class CPUMipmaps;
      friend class CPUItem;

      Scaler(Luminous::Priority prio,
	     CPUMipmaps *, CPUItem * dest, CPUItem * source, int level);

      virtual ~Scaler();

      virtual void doTask();

    private:
      
      CPUMipmaps * m_master;
      CPUItem * m_source;
      CPUItem * m_dest;
      int       m_level;
      std::string m_file;
      bool      m_quartering;
    };

    enum {
      DEFAULT_MAP1 = 6,
      DEFAULT_MAP2 = 9,
      MAX_MAPS = 13
    };

    enum ItemState {
      WAITING,
      WORKING,
      FINISHED,
      FAILED
    };

    class CPUItem
    {
    public:
      friend class CPUMipmaps;
      friend class Loader;
      friend class Scaler;

      CPUItem();

      ~CPUItem();
      inline bool needsLoader() const
      { return (m_state != FINISHED) && (m_scaler == 0) && (m_loader == 0); }

      inline bool working() const 
      {
	return (m_scaler != 0) || (m_loader != 0) || (m_state != FINISHED);
      }

    private:
      ItemState m_state;
      Scaler  * m_scalerOut;
      Scaler  * m_scaler;
      Loader  * m_loader;
      Radiant::RefPtr<Image> m_image;
      float     m_unUsed;
    };


    Luminous::Priority levelPriority(int level)
    {
      if(level <= DEFAULT_MAP1)
	return Luminous::Task::PRIORITY_NORMAL +
	  Luminous::Task::PRIORITY_OFFSET_BIT_HIGHER;
      
      return Luminous::Task::PRIORITY_NORMAL;
    }

    bool savebleMipmap(int i)
    { return i == DEFAULT_MAP1 || i == DEFAULT_MAP2; }

    void createLevelScalers(int level);

    void cacheFileName(std::string &, int level);

    BGThread * bgt() { return BGThread::instance(); }

    bool needsLoader(int i)
    {
      CPUItem * ci = m_stack[i].ptr();
      if(!ci)
        return true;
      return ci->needsLoader();
    }

    std::string m_filename;

    Radiant::RefPtr<CPUItem> m_stack[MAX_MAPS];
    Nimble::Vector2i m_nativeSize;
    int              m_maxLevel;
    // What level files are available as mip-maps.
    uint32_t         m_fileMask;
  };


}

#endif
