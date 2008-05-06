/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  DispatcherPrivate.hpp  DispatcherPrivate class definition.
                         This is the private interface for the dispatcher.
                         By using a public class, Dispatcher, which
                         delegates to this private class, it reduces the
                         size and complexity of the header files exposed
                         to client programs.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef DISPATCHER_PRIVATE_HPP
#define DISPATCHER_PRIVATE_HPP

#include "Notifier.hpp"
#include "Subscriber.hpp"
#include "SubscriberElement.hpp"

#include <Radiant/Mutex.hpp>

#include <deque>

namespace Notable
{

  // Forward declarations and typedefs.

  class SubscriberElement;
  class Dispatcher;

  typedef std::deque<SubscriberElement>   SubscriberList;
  typedef std::deque<Notifier *>          NotifierQueue;

  typedef std::deque<SubscriberElement>::iterator   SubscriberListIterator;
  typedef std::deque<Notifier *>::iterator          NotifierQueueIterator;

  class DispatcherPrivate
  {

    public:

      // Constructors and destructors.

      DispatcherPrivate(Dispatcher * dispatcher);
      virtual ~DispatcherPrivate();

      // Notifier post and send.

      // Queue a notifier for subsequent dispatch.
      void post(Notifier * notifier);

      // Immediately dispatche a notifier.
      void send(Notifier * notifier, bool autoDelete);

      // Subscribe and unsubscribe.

      // Return true if a subscriber will be sent this notifier 
      bool isSubscribed(Subscriber * subscriber, unsigned int subscription);

      // Add subscriber to the list that receives this notifier.
      void subscribe(Subscriber * subscriber, unsigned int subscription);

      // No longer send this notifier type to the subscriber.
      void unsubscribe(Subscriber * subscriber, unsigned int subscription);

      // Send no notifiers to this subscriber.
      void unsubscribeAll(Subscriber * subscriber);

      // Move this subscriber ahead or behind other subscribers in the list.
      void setSubscriberPriority(Subscriber * subscriber,
        const Subscriber::SUBSCRIBER_PRIORITY priority);

      // Called by TimerProc to dispatch notifiers.
      void onTimer();

    protected:

      // Implementation.

      // Notifier post and send.

      // Adds notifier to the notifier list.
      void insertNotifier(Notifier * newNotifier);

      // Subscribe and Unsubscribe.

      // Finds a subscriber in a given subscriber list.
      SubscriberListIterator find(Subscriber * subscriber,
        SubscriberList * list);

      // Adds a subscriber to a given subscriber list.
      void insertSubscriber(SubscriberElement & newElement, SubscriberList * list);

      // Dispatching functions.

      // Dispatches one notifier to all appropriate subscribers.
      void dispatchToAll(Notifier * notifier);

      // Match the dispatcher's subscriber list to the main subscriber list.
      void updateDispatcherList();

    private:

      // Data members.

      // Mutual exclusion locks for ensure thread safety.

      Radiant::MutexAuto  mutexSubscriber;
      Radiant::MutexAuto  mutexNotifier;

      // The interface class that delegates to us
      Dispatcher *        m_dispatcher;

      // Sorted list of subscribers that is always accurate.
      SubscriberList *    m_subscriberMain;

      // Sorted list of subscribers just for dispatching.
      SubscriberList *    m_subscriberDispatch;

      // True if main list matches dispatch list.
      bool                m_listsInSync;

      // True if currently dispatching notifiers.
      bool                m_dispatching;

      // Holds pending notifiers sorted by priority and order of posting.
      NotifierQueue *     m_notifierQueue;

      // Verifies that the main and dispatcher lists are in sync, if they're supposed to be.
      void assertInSync();

  };

}

#endif  // DISPATCHER_PRIVATE_HPP
