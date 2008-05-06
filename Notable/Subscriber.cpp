/*--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Subscribe.cpp  Subscriber Mixin Class
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#include "Subscriber.hpp"
#include "Dispatcher.hpp"

#include <Radiant/Trace.hpp>

#include <assert.h>

namespace Notable
{

  using namespace Radiant;

  /******************
  *** Static Data ***
  ******************/
  Dispatcher *   Subscriber::theDispatcher = 0;

  /*---------------------------------------------------------------------------
  Default contructor.
  ---------------------------------------------------------------------------*/
  Subscriber::Subscriber(int classID, SUBSCRIBER_PRIORITY priority)
  {
    trace("Subscriber::Subscriber[default]");

    if(!theDispatcher)
    {
      fatal("The Dispatcher must be created before a Subscriber can be created");
    }

    m_classID = classID;
    m_priority = priority;
  }

  /*---------------------------------------------------------------------------
  Copy constructor.
  ---------------------------------------------------------------------------*/
  Subscriber::Subscriber(const Subscriber & src)
  {
    trace("Subscriber::Subscriber[copy]");
 
    * this = src;
  }

  /*---------------------------------------------------------------------------
  Destructor.
  ---------------------------------------------------------------------------*/
  Subscriber::~Subscriber()
  {
    trace("Subscriber::~Subscriber");

    if(theDispatcher != 0)
    {
      theDispatcher->unsubscribeAll(this);
    }
  }

  /*---------------------------------------------------------------------------
  Assignment operator.
  ---------------------------------------------------------------------------*/
  Subscriber & Subscriber::operator=(const Subscriber & src)
  {
    m_classID = src.m_classID;
    m_priority = src.m_priority;

    return * this;
  }

  /*---------------------------------------------------------------------------
  setDispatcher.
  ---------------------------------------------------------------------------*/
  void Subscriber::setDispatcher(Dispatcher * dispatcher)
  {
    trace("Subscriber::setDispatcher");

    theDispatcher = dispatcher;
  }

  /*---------------------------------------------------------------------------
  getDispatcher.
  ---------------------------------------------------------------------------*/
  Dispatcher * Subscriber::getDispatcher()
  {
    trace("Subscriber::getDispatcher");

    return theDispatcher;
  }

  /*------------------------------- --------------------------------------------
  getPriority.
  ---------------------------------------------------------------------------*/
  Subscriber::SUBSCRIBER_PRIORITY Subscriber::getPriority()
  {
    trace("Subscriber::getPriority");

    return m_priority;
  }

  /*---------------------------------------------------------------------------
  setPriority.
  ---------------------------------------------------------------------------*/
  void Subscriber::setPriority(const SUBSCRIBER_PRIORITY priority)
  {
    trace("Subscriber::setPriority");

    theDispatcher->setSubscriberPriority(this, priority);
    m_priority = priority;
  }

  /*---------------------------------------------------------------------------
  isSubscribed.
  ---------------------------------------------------------------------------*/
  bool Subscriber::isSubscribed(unsigned int subscription)
  {
    trace("Subscriber::isSubscribed" );

    return theDispatcher->isSubscribed(this, subscription);
  }

  /*---------------------------------------------------------------------------
  subscribe: add a subscription for this subscriber.
  ---------------------------------------------------------------------------*/
  void Subscriber::subscribe(unsigned int subscription)
  {
    trace("Subscriber::subscribe");

    theDispatcher->subscribe(this, subscription);
  }

  /*---------------------------------------------------------------------------
  unsubscribe: remove a subscription for this subscriber.
  ---------------------------------------------------------------------------*/
  void Subscriber::unsubscribe(unsigned int subscription)
  {
    trace("Subscriber::unsubscribe");

    theDispatcher->unsubscribe(this, subscription);
  }

}
