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
  public:

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

    CPUMipmaps();
    virtual ~CPUMipmaps();

    void update(float dt, float purgeTime);

    int getOptimal(Nimble::Vector2f size);
    int getClosest(Nimble::Vector2f size);

    Image * getImage(int i);

    void markImage(int i);

    bool startLoading(const char * filename, bool immediate);

    const Nimble::Vector2i & nativeSize() const { return m_nativeSize;}
  
    GPUMipmaps * getGpuMipmaps(GLResources *);
    
    int maxLevel() const { return m_maxLevel; }
    int lowestLevel() const { return 5; }

    bool isActive();

    inline float aspect() const { return (float)m_nativeSize.x / (float)m_nativeSize.y; }
  private:

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
