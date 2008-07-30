/**--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  Subscriber_mask.hpp  SubscriberMask class definition

  A subscription to a notifier means that a bit is set in a subscriber's
  bit mask, at the position reserved for that notifier. The NotifierMask
  class encapsulates the manipulation of the subscriber mask.

  This is important since there can be more than 32 notifier types, meaning
  that a mask may extend beyond a simple unsigned long.

  ---------------
  IMPORTANT NOTE:
  ---------------
  MAX_NOTIFIER_TYPES is constant. To increase the capacity of notifier types,
  this constant must be changed in the source code and then rebuilt.
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#ifndef SUBSCRIBER_MASK_HPP
#define SUBSCRIBER_MASK_HPP

#include <Radiant/Trace.hpp>

#include <memory>

namespace Notable
{

  class SubscriberMask
  {
    public:

      // Number of bits required for the bit mask.
      enum { MAX_NOTIFIER_TYPES = 64 };   // this is an arbitrary value for testing

      // Number of 4-byte unsigned longs required to contain the mask.
      enum { LEN_SUBSCRIBER_MASK = ((((MAX_NOTIFIER_TYPES + 7) / 8) + 3) / 4) };

      // Constructors and destructors.

      SubscriberMask();
      SubscriberMask(const SubscriberMask & src);
      ~SubscriberMask();
      SubscriberMask & operator=(const SubscriberMask & src);

      // Operations.

      // Zero the bit mask.
      void clear();

      // Return true if the bit mask is zero.
      bool isClear() const;

      // Set a bit in the bit mask, e.g. bit 3 is 0x00001000.
      void subscribe(unsigned int bitIndex);

      // Clear a bit in the bit mask.
      void unsubscribe(unsigned int bitIndex);

      // Return true if the corresponding bit is set.
      bool isSubscribed(unsigned int bitIndex) const;

      // Operators.

      friend bool operator==(const SubscriberMask & a, const SubscriberMask & b);

      friend bool operator!=(const SubscriberMask & a, const SubscriberMask & b);

    private:

      const unsigned long & getMaskWord(unsigned int bitIndex, unsigned long & mask) const;

      /// 32-bits of mask per array entry.
      unsigned long   m_mask[LEN_SUBSCRIBER_MASK];

  };

  /***********************
  *** Inline Functions ***
  ***********************/

  /*---------------------------------------------------------------------------
  Default contructor.
  ---------------------------------------------------------------------------*/
  inline SubscriberMask::SubscriberMask()
  {
    clear();
  }

  /*---------------------------------------------------------------------------
  Copy constructor.
  ---------------------------------------------------------------------------*/
  inline SubscriberMask::SubscriberMask(const SubscriberMask & src)
  {
    * this = src;
  }

  /*---------------------------------------------------------------------------
  Destructor.
  ---------------------------------------------------------------------------*/
  inline SubscriberMask::~SubscriberMask()
  {
  }

  /*---------------------------------------------------------------------------
  Assignment operator.
  ---------------------------------------------------------------------------*/
  inline SubscriberMask & SubscriberMask::operator=(const SubscriberMask & src)
  {
    memcpy(m_mask, src.m_mask, LEN_SUBSCRIBER_MASK * sizeof(unsigned long));

    return * this;
  }

}

#endif  //SUBSCRIBER_MASK_HPP
