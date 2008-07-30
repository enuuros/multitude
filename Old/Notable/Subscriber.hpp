/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Subscriber.h  Subscriber Mixin Class
                Base class for objects that receive Notifiers.
                This class is intended for multiple inheritance, so the
                names are chosen to remain unique.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include "Notifier.hpp"

namespace Notable
{

  class Dispatcher;
  class Notifier;

  /**
    * @class Subscriber
    * A Subscriber object is a receiver of notifiers. Every application
    * should have at least one class derived from Subscriber.
    */
  class Subscriber
  {

    public:

      /// Subscriber priority.

      enum SUBSCRIBER_PRIORITY
      {
        SUBSCRIBER_PRIORITY_HIGH,
        SUBSCRIBER_PRIORITY_NORMAL,
        SUBSCRIBER_PRIORITY_LOW
      };

    public:

      /// Constructors and destructors.

      Subscriber(int classID = 0, SUBSCRIBER_PRIORITY priority = SUBSCRIBER_PRIORITY_NORMAL);
      Subscriber(const Subscriber & src);
      virtual ~Subscriber();
      Subscriber & operator=(const Subscriber & src);

      /// Accessors.

      /// Return subscriber subclass.
      int getClassID() { return m_classID; }

      /// Set subscriber subclass.
      void setClassID(const int classID) { m_classID = classID; }

      /// Return relative priority of subscriber.
      SUBSCRIBER_PRIORITY getPriority();

      /// Move this subscriber ahead or behind other subscribers in the list.
      void setPriority(const SUBSCRIBER_PRIORITY priority);

      /// Return true if a subscriber will be sent this notifier 
      bool isSubscribed(unsigned int subscription);

      /// Add subscriber to the list that receives this notifier.
      void subscribe(unsigned int subscription);

      /// No longer send this notifier type to the subscriber.
      void unsubscribe(unsigned int subscription);

    protected:

      /// Implementation.

      /// Called by Dispatcher to set the static pointer to itself.
      static void setDispatcher(Dispatcher * dispatcher);

      /// Get pointer to dispatcher.
      static Dispatcher * getDispatcher();

    private:

      /// Data members.

      /// See SetDispatcher.
      static Dispatcher *   theDispatcher;

      /// Subclassing enables different objects to subscribe to different notifiers.
      int   m_classID;

      /// Determines dispatch order of posted notifiers.
      SUBSCRIBER_PRIORITY   m_priority;

    friend class Dispatcher;
  };

}

#endif //SUBSCRIBER_HPP
