/*--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Notifier.cpp  Notifier class implementation.
                This is the base class for all notifiers. See example
                programs and "Simplifying C++" article for how to create
                a notifier subclass.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#include "Notifier.hpp"
#include "Dispatcher.hpp"

#include <Radiant/Trace.hpp>

#include <assert.h>

namespace Notable
{

  using namespace Radiant;

  /******************
  *** Static Data ***
  ******************/
  Dispatcher *   Notifier::theDispatcher = 0;

  /**********************************
  *** Construction and Assignment ***
  **********************************/

  /*---------------------------------------------------------------------------
  Default constructor
  ---------------------------------------------------------------------------*/
  Notifier::Notifier(int classID)
  {
    trace("Notifier::Notifier[default]");

    if(!theDispatcher)
    {
      fatal("The Dispatcher must be created before a Notifier can be created");
    }

    m_classID = classID;
    m_priority = NOTIFIER_PRIORITY_NORMAL;
  }

  /*---------------------------------------------------------------------------
  Copy constructor
  ---------------------------------------------------------------------------*/
  Notifier::Notifier(const Notifier & src)
  {
    trace("Notifier::Notifier[copy]");

    * this = src;
  }

  /*---------------------------------------------------------------------------
  Destructor
  ---------------------------------------------------------------------------*/
  Notifier::~Notifier()
  {
    trace("Notifier::~Notifier");
  }

  /*---------------------------------------------------------------------------
  Assignment operator
  ---------------------------------------------------------------------------*/
  Notifier & Notifier::operator=(const Notifier & src)
  {
    m_classID = src.m_classID;
    m_priority = src.m_priority;

    return * this;
  }

  /*****************
  *** Operations ***
  *****************/

  /*---------------------------------------------------------------------------
  post: add a notifier to the queue for subsequent dispatching.
  ---------------------------------------------------------------------------*/
  void Notifier::post(NOTIFIER_PRIORITY priority)
  {
    trace("CNotifier::post");

    assert(theDispatcher);

    setPriority(priority);
    theDispatcher->post(this);
  }

  /*---------------------------------------------------------------------------
  send: send a notifier immediately to all subscribers.
  ---------------------------------------------------------------------------*/
  void Notifier::send(bool autoDelete)
  {
    trace("CNotifier::send");

    assert(theDispatcher);

    theDispatcher->send(this, autoDelete);
  }

  /*---------------------------------------------------------------------------
  setDispatcher
  ---------------------------------------------------------------------------*/
  void Notifier::setDispatcher(Dispatcher * dispatcher)
  {
    trace("Notifier::setDispatcher");

    theDispatcher = dispatcher; 
  }

  /*---------------------------------------------------------------------------
  getDispatcher
  ---------------------------------------------------------------------------*/
  Dispatcher * Notifier::getDispatcher()
  {
    trace("Notifier::getDispatcher");

    return theDispatcher; 
  }

}
