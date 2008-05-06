/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Dispatcher.hpp  Dispatcher class definition.
                  This is the interface between a client application and the
                  notifier dispatcher.

                  The client subclasses this interface to provide a 
                  DispatchToOne function which is aware of the specific
                  notifier classes used by the client.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "Subscriber.hpp"

#include <Radiant/TimeSignaller.hpp>

namespace Notable
{

  class Notifier;

  void timerProc(int sigNo, siginfo_t * info, void * context);

  /**
    * @class Dispatcher
    * Every application has a single object of a class derived from
    * Dispatcher, which must exist before any other objects can be
    * created. The dispatcher runs in the application thread and
    * dispatches notifiers to subscribers at regular intervals.
    */
  class Dispatcher
  {
    public:

      /// Constructors and destructors.

      /** Constructor.
        * @param timerPeriod Period in seconds between successive activations
        * of dispatcher.
        */
      Dispatcher(float timerPeriod = 0.05f);

      /// Destructor.
      virtual ~Dispatcher();

      /// Notifier post and send.

      /// Queues a notifier for subsequent dispatch.
      void post(Notifier * notifier);

      /// Immediately dispatches a notifier.
      void send(Notifier * notifier, bool autoDelete);

      /// Subscribe and unsubscribe.

      /// Returns true if a subscriber will be sent this notifier.
      bool isSubscribed(Subscriber * subscriber, unsigned int subscription);

      /// Adds subscriber to the list that receives this notifier.
      void subscribe(Subscriber * subscriber, unsigned int subscription);

      /// No longer send this notifier type to the subscriber.
      void unsubscribe(Subscriber * subscriber, unsigned int subscription);

      /// Send no notifiers to this subscriber.
      void unsubscribeAll(Subscriber * subscriber);

      /// Move this subscriber ahead or behind other subscribers in the list.
      void setSubscriberPriority(Subscriber * subscriber, const Subscriber::SUBSCRIBER_PRIORITY priority);

    protected:

      /*
        * The client application customizes this function to dispatch all the client's custom
        * notifiers to the custom subscribers.
        */
      virtual void dispatchToOne(Notifier * notifier, Subscriber * subscriber) = 0;

    private:

      /// Time signaller used to trigger periodic dispatching.
      Radiant::TimeSignaller *    m_timer;

    friend class DispatcherPrivate;

  };

}

#endif  // DISPATCHER_HPP
