/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Subscriber_element.hpp  A SubscriberElement is the object stored in the
                          dispatcher's list of subscribers.
                          This class is used like a data structure, with a
                          few convenient functions. It's only used by the
                          DispatcherPrivate class, so we're not hiding
                          the data.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef SUBSCRIBER_ELEMENT_HPP
#define SUBSCRIBER_ELEMENT_HPP

#include "Subscriber.hpp"
#include "SubscriberMask.hpp"

#include <Radiant/Trace.hpp>

namespace Notable
{

  class Notifier;

  class SubscriberElement
  {
    public:

      // Member data.

      Subscriber *    m_subscriber;
      SubscriberMask  m_mask;
      Subscriber::SUBSCRIBER_PRIORITY   m_priority;

    public:

      // Member functions.

      // Constructor and destructors.

      SubscriberElement(Subscriber * subscriber = 0);
      virtual ~SubscriberElement();
      SubscriberElement(const SubscriberElement & src);
      SubscriberElement & operator=(const SubscriberElement & src);

      // Operations.

      // Return true if a given bit is set in the mask.
      // Note that bitIndex 3 is 0x00001000.
      bool isSubscribed(unsigned int bitIndex) const;

      // Return true if all mask bits are zero.
      bool isMaskClear() const;

  };

  /***********************
  *** Inline Functions ***
  ***********************/

  /*---------------------------------------------------------------------------
  Default constructor: element is not valid if Subscriber is zero.
  ---------------------------------------------------------------------------*/
  inline SubscriberElement::SubscriberElement(Subscriber * subscriber)
  {
    m_subscriber = subscriber;
    m_mask.clear();
    m_priority = Subscriber::SUBSCRIBER_PRIORITY_NORMAL;
  }

  /*---------------------------------------------------------------------------
  Copy constructor.
  ---------------------------------------------------------------------------*/
  inline SubscriberElement::SubscriberElement(const SubscriberElement & src)
  {
    * this = src;
  }

  /*---------------------------------------------------------------------------
  Destructor.
  ---------------------------------------------------------------------------*/
  inline SubscriberElement::~SubscriberElement()
  {
  }

  /*---------------------------------------------------------------------------
  Assignment operator.
  ---------------------------------------------------------------------------*/
  inline SubscriberElement & SubscriberElement::operator=(const SubscriberElement & src)
  {
    m_subscriber = src.m_subscriber;
    m_mask = src.m_mask;

    return * this;
  }

  /*---------------------------------------------------------------------------
  isSubscribed: return true if a given bit is set in the mask.
  Note that bitIndex 3 is 0x00001000.
  ---------------------------------------------------------------------------*/
  inline bool SubscriberElement::isSubscribed(unsigned int bitIndex) const
  {
    return m_mask.isSubscribed(bitIndex);
  }

  /*---------------------------------------------------------------------------
  isMaskClear: return true if all mask bits are zero
  ---------------------------------------------------------------------------*/
  inline bool SubscriberElement::isMaskClear() const
  {
    return m_mask.isClear();
  }

}

#endif  // SUBSCRIBER_ELEMENT_HPP
