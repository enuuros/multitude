/*--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Dispatcher.cpp  Interface class that hides the dispatcher implementation
                  from the users of the class.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#include "Notifier.hpp"
#include "Subscriber.hpp"
#include "Dispatcher.hpp"
#include "DispatcherPrivate.hpp"
#include "SubscriberElement.hpp"

#include <Radiant/Trace.hpp>

#include <assert.h>

namespace Notable
{

  using namespace Radiant;

  /********************************
  *** Static Data and Functions ***
  ********************************/

  // The ONE AND ONLY dispatcher object.
  static DispatcherPrivate   * g_dispPrivate = 0;

  /*---------------------------------------------------------------------------
  timerProc: once the dispatcher is initialized, this proc must be called
             periodically (usually every 50 ms).
  ---------------------------------------------------------------------------*/
  void timerProc(int sigNo, siginfo_t * /*info*/, void * /*context*/)
  {
    trace("++++++++ timerProc");

    static bool   dispatching = false;

    if(sigNo == TIME_SIGNAL)
    {
      if(!dispatching)
      // Don't waste time re-entering the onTimer function
      {
        dispatching = true;
        if(g_dispPrivate)
        {
          g_dispPrivate->onTimer();
        }
        dispatching = false;
      }
    }
  }

  /**************************************
  *** Construction and Initialization ***
  **************************************/

  /*---------------------------------------------------------------------------
  Constructor.
  ---------------------------------------------------------------------------*/
  Dispatcher::Dispatcher(float timerPeriod)
  : m_timer(0)
  {
    trace("Dispatcher::Dispatcher");

    // Ensure only one instance

    assert(!g_dispPrivate);

    //  Create private dispatcher

    g_dispPrivate = new DispatcherPrivate(this);

    // Create time signaller to periodically activate the dispatcher

    m_timer = new Radiant::TimeSignaller(timerPeriod, & timerProc);

    // Inform other classes that this is the dispatcher

    Subscriber::setDispatcher(this);
    Notifier::setDispatcher(this);
  }

  /*---------------------------------------------------------------------------
  Destructor: delete any undispatched notifiers, delete lists.
  ---------------------------------------------------------------------------*/
  Dispatcher::~Dispatcher()
  {
    trace("Dispatcher::~Dispatcher");

    Subscriber::setDispatcher(0);
    Notifier::setDispatcher(0);

    if(g_dispPrivate)
    {
      delete g_dispPrivate;
      g_dispPrivate = 0;
    }

    if(m_timer)
    {
      delete m_timer;
    }
  }

  /*****************************
  *** NOTIFIER POST AND SEND ***
  *****************************/

  /*---------------------------------------------------------------------------
  post: queue a notifier for subsequent dispatch.
  ------------------------------------------ ---------------------------------*/
  void Dispatcher::post(Notifier * notifier)
  {
    trace("Dispatcher::post");

    g_dispPrivate->post(notifier);
  }

  /*---------------------------------------------------------------------------
  send: send a notifier to all subscribers while caller waits.
  ---------------------------------------------------------------------------*/
  void Dispatcher::send(Notifier * notifier, bool autoDelete)
  {
    trace("Dispatcher::send");

    g_dispPrivate->send(notifier, autoDelete);
  }

  /********************************
  *** SUBSCRIBE AND UNSUBSCRIBE ***
  ********************************/
 
  /*---------------------------------------------------------------------------
  isSubscribed: return true if the subscriber holds a given subscription.
  ---------------------------------------------------------------------------*/
  bool Dispatcher::isSubscribed(Subscriber * subscriber, unsigned int subscription)
  {
    trace("Dispatcher::isSubscribed");

    return g_dispPrivate->isSubscribed(subscriber, subscription);
  }

  /*---------------------------------------------------------------------------
  subscribe: for a given subscriber, add a subscription to a notifier.
  ---------------------------------------------------------------------------*/
  void Dispatcher::subscribe(Subscriber * subscriber, unsigned int subscription)
  {
    trace("Dispatcher::subscribe");

    g_dispPrivate->subscribe(subscriber, subscription);
  }

  /*---------------------------------------------------------------------------
  unsubscribe: for a given subscriber, remove  a subscription to a notifier.
  ---------------------------------------------------------------------------*/
  void Dispatcher::unsubscribe(Subscriber * subscriber, unsigned int subscription)
  {
    trace("Dispatcher::unsubscribe");

    g_dispPrivate->unsubscribe(subscriber, subscription);
  }

  /*---------------------------------------------------------------------------
  unsubscribeAll: for a given subscriber, remove all subscriptions.
  ---------------------------------------------------------------------------*/
  void Dispatcher::unsubscribeAll(Subscriber * subscriber)
  {
    trace("Dispatcher::unsubscribeAll");

    g_dispPrivate->unsubscribeAll(subscriber);
  }

  /*---------------------------------------------------------------------------
  setSubscriberPriority: change the position of a subscriber within the
                         subscriber list.
  ---------------------------------------------------------------------------*/
  void Dispatcher::setSubscriberPriority(Subscriber * subscriber,
    const Subscriber::SUBSCRIBER_PRIORITY priority)
  {
    trace("Dispatcher::setSubscriberPriority");

    g_dispPrivate->setSubscriberPriority(subscriber, priority);
  }

}
