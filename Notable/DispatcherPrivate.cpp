/*--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  DispatcherPrivate.cpp  DispatcherPrivate class dispatches Notifiers to
                         Subscribers. It manages lists of subscribers
                         and notifiers, and handles requests to add or
                         remove subscriptions, and post notifiers.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#include "Notifier.hpp"
#include "Subscriber.hpp"
#include "Dispatcher.hpp"
#include "SubscriberElement.hpp"
#include "DispatcherPrivate.hpp"

#include <Radiant/Trace.hpp>

#include <algorithm>
#include <assert.h>

namespace Notable
{

  using namespace Radiant;

  /**************************************
  *** Construction and Initialization ***
  **************************************/

  /*---------------------------------------------------------------------------
  Constructor.
  ---------------------------------------------------------------------------*/
  DispatcherPrivate::DispatcherPrivate(Dispatcher * dispatcher)
  : m_dispatcher(dispatcher)
  , m_subscriberMain(new SubscriberList)
  , m_subscriberDispatch(new SubscriberList)
  , m_listsInSync(true)
  , m_dispatching(false)
  , m_notifierQueue(new NotifierQueue)
  {
    trace("DispatcherPrivate::DispatcherPrivate");
  }

  /*---------------------------------------------------------------------------
  Destructor: delete any undispatched notifiers, delete lists.
  ---------------------------------------------------------------------------*/
  DispatcherPrivate::~DispatcherPrivate()
  {
    trace("DispatcherPrivate::~DispatcherPrivate");

    // Delete main subscriber list

    delete m_subscriberMain;

    // Delete subscriber list used during dispatch

    delete m_subscriberDispatch;

    // Delete unposted notifiers

    for(NotifierQueueIterator it = m_notifierQueue->begin(); it != m_notifierQueue->end(); it++)
    {
      delete * it;
    }
    delete m_notifierQueue;
  }

  /*****************************
  *** NOTIFIER POST AND SEND ***
  *****************************/

  /*---------------------------------------------------------------------------
  post: queue a notifier for subsequent dispatch.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::post(Notifier * notifier)
  {
    trace("DispatcherPrivate::post");

    insertNotifier(notifier);
  }

  /*---------------------------------------------------------------------------
  send: send a notifier to all subscribers while caller waits.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::send(Notifier * notifier, bool autoDelete)
  {
    trace("DispatcherPrivate::send");

    dispatchToAll(notifier);

    // Delete notifier after sending it, if instructed to

    if(autoDelete)
    {
      delete notifier;
    }
  }

  /*---------------------------------------------------------------------------
  insertNotifier  Insert a notifier in priority order.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::insertNotifier(Notifier * newNotifier)
  {
    trace("DispatcherPrivate::insertNotifier");

    // Mutual excusion lock ensures thread-safety

    mutexNotifier.lock();

    // The list is ordered by notifier priority. Within priority, later
    // notifiers are posted at the back.

    NotifierQueueIterator   it = m_notifierQueue->begin();
    while(it != m_notifierQueue->end() && newNotifier->getPriority() >= (* it)->getPriority())
    {
      it++;
    }

    m_notifierQueue->insert(it, newNotifier);

    mutexNotifier.unlock();
  }

  /********************************
  *** SUBSCRIBE AND UNSUBSCRIBE ***
  ********************************/

  /*---------------------------------------------------------------------------
  isSubscribed: return true if the subscriber holds a given subscription.
  ---------------------------------------------------------------------------*/
  bool DispatcherPrivate::isSubscribed(Subscriber * subscriber, unsigned int subscription)
  {
    trace("DispatcherPrivate::isSubscribed");

    bool  found = false;

    mutexSubscriber.lock();

    // Look for the subscriber in the main list

    SubscriberListIterator    it = find(subscriber, m_subscriberMain);

    if(it != m_subscriberMain->end())
    {
      found = it->m_mask.isSubscribed(subscription);
    }

    mutexSubscriber.unlock();

    return found;
  }

  /*---------------------------------------------------------------------------
  subscribe: if a subscriber is already subscribed (to another subscription)
             then it will be in the list and only its mask has to be changed.
             If it's not in the list, it has to be added.

             If we're in the middle of a dispatch, we can't add a subscription
             to the dispatcher's private list.  Instead, we modify the main
             list and set a flag indicating that the two lists differ. We'll
             update the dispatcher's list after the dispatch completes.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::subscribe(Subscriber * subscriber, unsigned int subscription)
  {
    trace("DispatcherPrivate::subscribe");

    mutexSubscriber.lock();

    // Look for the subscriber in the main list

    SubscriberListIterator    it = find(subscriber, m_subscriberMain);

    if(it != m_subscriberMain->end())
    // The subscriber is already in the list, only need to update its mask
    {
      // UPDATE IN MAIN LIST
      it->m_mask.subscribe(subscription);

      // UPDATE IN DISPATCHER LIST
      // If the two lists are currently in sync and we're not dispatching,
      // keep them in sync
      if(m_listsInSync && !m_dispatching)
      {
        it = find(subscriber, m_subscriberDispatch);
        // lists are in sync, should be able find it
        assert(it != m_subscriberDispatch->end());
        it->m_mask.subscribe(subscription);
      }
      // if we were dispatching, we couldn't keep the lists in sync
      else
      {
         m_listsInSync = false;
      }
    }
    else
    // The subscriber is new, add it to the subscriber list
    {
      // ADD TO MAIN LIST
      SubscriberElement    newElement(subscriber);
      newElement.m_mask.subscribe(subscription);
      newElement.m_priority = subscriber->getPriority();
      insertSubscriber(newElement, m_subscriberMain);

      // ADD TO DISPATCHER LIST
      // If the two lists are currently in sync and we're not dispatching,
      // keep them in sync
      if(m_listsInSync && !m_dispatching)
      {
        insertSubscriber(newElement, m_subscriberDispatch);
      }
      // if we were dispatching, we couldn't keep the lists in sync
      else
      {
        m_listsInSync = false;
      }
    }

    assertInSync();

    mutexSubscriber.unlock();
  }

  /*---------------------------------------------------------------------------
  unsubscribe: update the subscription mask for the subscriber. If the mask
               becomes zero, i.e. we removed the last subscription, remove the
               subscriber record completely.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::unsubscribe(Subscriber * subscriber, unsigned int subscription)
  {
    trace("DispatcherPrivate::unsubscribe");

    mutexSubscriber.lock();

    // Find the subscriber, clear the bit in its mask.
    // If the mask is now zero, remove the subscriber from the list.

    SubscriberListIterator    it = find(subscriber, m_subscriberMain);

    if(it != m_subscriberMain->end())
    {
      // clear the subscription bit
      it->m_mask.unsubscribe(subscription);
      // if the subscriber has no more subscriptions, remove it
      if(it->isMaskClear())
      {
         m_subscriberMain->erase(it);
      }
    }

    // We keep the mask in the dispatch subscriber list up-to-date
    // This is important because the moment a subscriber unsubscribes they
    // must no longer receive notifiers. For one thing, they may be
    // unsubscribing because they're being deleted!
    // But we can't remove subscriber records if a dispatch is active because
    // that can foul up the iteration through the list.

    it = find(subscriber, m_subscriberDispatch);

    if(it != m_subscriberDispatch->end())
    {
      // found the subscriber, clear the bit in its subscription mask
      it->m_mask.unsubscribe(subscription);
      // if the mask is now zero, remove the subscriber
      if(it->isMaskClear())
      {
        // if not dispatching, remove from the dispatcher list
        if(!m_dispatching && m_listsInSync)
        {
          m_subscriberDispatch->erase(it);
        }
        else
        {
          m_listsInSync = false;
        }
      }
    }

    assertInSync();

    mutexSubscriber.unlock();
  }

  /*---------------------------------------------------------------------------
  unsubscribeAll: remove a subscriber from the subscriber list. This removes
                  all its subscriptions.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::unsubscribeAll(Subscriber * subscriber)
  {
    trace("DispatcherPrivate::unsubscribeAll");

    mutexSubscriber.lock();

    // Find the subscriber and remove it from the list
    SubscriberListIterator    it = find(subscriber, m_subscriberMain);

    if(it != m_subscriberDispatch->end())
    {
      m_subscriberMain->erase(it);
    }

    // We clear the mask in the dispatch subscriber list.
    // This is important because the moment a subscriber unsubscribes they
    // must no longer receive notifiers. For one thing, they be unsubscribing
    // because they're being deleted!
    // But we can't remove subscribers if a dispatch is active because
    // that can foul up the iteration through the list.

    it = find(subscriber, m_subscriberDispatch);

    if(it != m_subscriberDispatch->end())
    {
      // found the subscriber, clear its subscription mask
      it->m_mask.clear();
      if(!m_dispatching && m_listsInSync)
      {
        // if not dispatching, remove from the dispatcher list
        m_subscriberDispatch->erase(it);
      }
      else
      {
        m_listsInSync = false;
      }
    }

    assertInSync();

    mutexSubscriber.unlock();
  }

  /*---------------------------------------------------------------------------
  setSubscriberPriority: change subscriber priority by re-inserting in list.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::setSubscriberPriority(Subscriber * subscriber,
    const Subscriber::SUBSCRIBER_PRIORITY priority)
  {
    trace("DispatcherPrivate::setSubscriberPriority");

    mutexSubscriber.lock();

    SubscriberListIterator    it = find(subscriber, m_subscriberMain);

    if(it != m_subscriberMain->end())
    {
      // make a copy of the element we found
      SubscriberElement   element = * it;
      if(element.m_priority != priority)
      {
        // SET PRIORITY IN MAIN LIST
        // remove and re-insert the element
        m_subscriberMain->erase(it);
        insertSubscriber(element, m_subscriberMain);

        // SET PRIORITY IN DISPATCHER LIST
        // If the two lists are currently in sync and we're not dispatching,
        // keep them in sync
        if(m_listsInSync && !m_dispatching)
        {
          // lists are in sync, we had better find this subscriber!!
          it = find(subscriber, m_subscriberDispatch);
          assert(it != m_subscriberDispatch->end());
          if(it != m_subscriberDispatch->end())
          {
            // remove and re-insert in the dispatch list
            m_subscriberDispatch->erase(it);
            insertSubscriber(element, m_subscriberDispatch);
          }
        }
      }
    }

    assertInSync();

    mutexSubscriber.unlock();
  }

  /*---------------------------------------------------------------------------
  find: find a subscriber in the subscriber list.
  ---------------------------------------------------------------------------*/

  SubscriberListIterator DispatcherPrivate::find(Subscriber * subscriber,
    SubscriberList * list)
  {
    trace("DispatcherPrivate::find");

    SubscriberListIterator  it = list->begin();
    while(it != list->end())
    {
      if(it->m_subscriber == subscriber)
      {
         break;
      }
      it++;
    }

    return it;
  }

  /*---------------------------------------------------------------------------
  insertSubscriber: insert a subscriber in priority order
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::insertSubscriber(SubscriberElement & newElement,
    SubscriberList * list)
  {
    trace("DispatcherPrivate::insertSubscriber");

    // The list is ordered by subscriber priority. Within priority, later
    // subscribers are posted at the back.

    SubscriberListIterator    it = list->begin();
    // if the subscriber mask is zero, the subscriber pointer isn't valid
    while(it != list->end() && ((newElement.m_priority >=
      it->m_subscriber->getPriority()) || it->isMaskClear()))
    {
      it++;
    }

    list->insert(it, newElement);
  }

  /******************
  *** DISPATCHING ***
  ******************/

  /*---------------------------------------------------------------------------
  onTimer: dispatch all notifiers to their subscribers.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::onTimer()
  {
    mutexNotifier.lock();
    trace("DispatcherPrivate::onTimer: m_notifierQueue->size() = %d", m_notifierQueue->size());
    mutexNotifier.unlock();

    // Dispatch all pending notifiers

    while(!m_notifierQueue->empty())
    {
      // lock notifiers, remove top notifier, dispatch to subscribers, then delete

      mutexNotifier.lock();

      Notifier *    notifier = m_notifierQueue->front();
      m_notifierQueue->pop_front();

      mutexNotifier.unlock();

      dispatchToAll(notifier);

      delete notifier;
    }
  }

  /*---------------------------------------------------------------------------
  dispatchToAll: dispatch a notifier to all subscribers.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::dispatchToAll(Notifier * notifier)
  {
    trace("DispatcherPrivate::dispatchToAll");

    // Prevent other threads from updating the main subscriber list or
    // deleting a subscriber during a dispatch.
    // This thread may still update the main list, but may not insert
    // or delete records from the dispatcher's list.

    mutexSubscriber.lock();

    bool    wasDispatching = m_dispatching;
    m_dispatching = true;  // stops this thread from updating dispatcher list

    // The dispatcher has its own copy of the subscriber list. Before a
    // dispatch, check if the main list changed and if so, update the
    // dispatcher's list.

    updateDispatcherList(); 

    // Dispatch the notifier to each subscriber

    trace("DispatcherPrivate::dispatchToAll: m_subscriberDispatch->size() = %d", m_subscriberDispatch->size());

    for(SubscriberListIterator it = m_subscriberDispatch->begin(); it != m_subscriberDispatch->end(); it++)
    {
trace("iteration");
      if(it->isSubscribed(notifier->getClassID()))
      {
        m_dispatcher->dispatchToOne(notifier, it->m_subscriber);
      }
trace("A");
    }

    m_dispatching = wasDispatching;

    mutexSubscriber.unlock();
  }

  /*---------------------------------------------------------------------------
  updateDispatcherList: if the subscriber queue has changed since the last 
                        update, copy it to the dispatcher's private list.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::updateDispatcherList()
  {
    trace("DispatcherPrivate::updateDispatcherList");

    // If the list is out of sync, erase it and copy the main list

    if(!m_listsInSync)
    {
      m_subscriberDispatch->clear();

      for(SubscriberListIterator it = m_subscriberMain->begin(); it != m_subscriberMain->end(); it++)
      {
        m_subscriberDispatch->push_back(* it);
      }

      m_listsInSync = true;
    }

    assertInSync();
  }

  /*---------------------------------------------------------------------------
  AssertInSync: validate that the two lists are in sync.
  ---------------------------------------------------------------------------*/
  void DispatcherPrivate::assertInSync()
  {
    trace("DispatcherPrivate::assertInSync");

    // Only assert if they're supposed to be in sync

    if(m_listsInSync)
    {
      assert(m_subscriberMain->size() == m_subscriberDispatch->size());

      SubscriberListIterator  itMain = m_subscriberMain->begin();
      SubscriberListIterator  itDisp = m_subscriberDispatch->begin();

      while(itMain != m_subscriberMain->end())
      {
        if(itMain->m_subscriber != itDisp->m_subscriber)
        {
          assert(false);
        }
        if(itMain->m_mask != itDisp->m_mask)
        {
          assert(false);
        }
        itMain++;
        itDisp++;
      }
    }
  }

}
