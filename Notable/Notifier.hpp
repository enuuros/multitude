/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Notifier.hpp  Notifier class definition.
                This is the base class for all notifiers. See example
                programs and "Simplifying C++" article for how to create
                a notifier subclass.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

namespace Notable
{

  class Dispatcher;

  /**
    * @class Notifier
    * The 'message' class. For each type of data to be passed between
    * objects, an application should derive a class from Notifier.
    */
  class Notifier
  {

    public:

      enum NOTIFIER_PRIORITY
      {
        NOTIFIER_PRIORITY_HIGH,
        NOTIFIER_PRIORITY_NORMAL,
        NOTIFIER_PRIORITY_LOW
      };

    public:

      /// Constructors and destructors.

      Notifier(int classID = 0);
      Notifier(const Notifier & src);
      virtual ~Notifier();
      Notifier & operator=(const Notifier & event);

      /** Operations.
        * @note Notifiers to be posted or sent must be dynamically
        * allocated heap objects as by default they are automatically
        * deleted after dispatch.
        */

      /// Queue this notifier for subsequent dispatch.
      virtual void post(NOTIFIER_PRIORITY priority = NOTIFIER_PRIORITY_NORMAL);

      /// Dispatch this notifier immediately.
      /// By default, delete the notifier after sending.
      virtual void send(bool autoDelete = true);

      /// Accessors.

      /// Return notifier sub-class.
      int getClassID() { return m_classID; }

      /// Set notifier sub-class.
      void setClassID(const int classID) { m_classID = classID; }

      /// Return priority that determines order of dispatching.
      NOTIFIER_PRIORITY getPriority() { return m_priority; }

      /// Change the sort order of a notifier (before it's queued).
      void setPriority(const NOTIFIER_PRIORITY priority) { m_priority = priority; }

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

      /// Subclass determines which On function is called in CSubscriber.
      int   m_classID;

      /// Determines dispatch order of posted notifiers.
      NOTIFIER_PRIORITY     m_priority;

    friend class Dispatcher;

  };

}

#endif  // NOTIFIER_HPP
