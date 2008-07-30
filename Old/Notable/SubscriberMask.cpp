/*--------------------------------------------------------------------------

  This file is part of Notable.

-----------------------------------------------------------------------------
         Copyright 1997    On Target Software    All Rights Reserved

  Dave Pomerantz, Nov 1997.  Updates available at http://www.targetsoft.com
-----------------------------------------------------------------------------
  SubscriberMask.cpp  SubscriberMask implementation
-----------------------------------------------------------------------------

  Adapted Apr 2008, George Whale, MultiTouch Oy.

---------------------------------------------------------------------------*/

#include "SubscriberMask.hpp"

#include <assert.h>

namespace Notable
{

  using namespace Radiant;

  /// this is used if the caller passes an invalid bit index.
  unsigned long   g_zeroMask = 0;

  /*---------------------------------------------------------------------------
  clear: set the mask to zero.
  ---------------------------------------------------------------------------*/
  void SubscriberMask::clear()
  {
    memset(m_mask, 0, LEN_SUBSCRIBER_MASK * sizeof(unsigned long));
  }

  /*---------------------------------------------------------------------------
  isClear: return true if all bits are off.
  ---------------------------------------------------------------------------*/
  bool SubscriberMask::isClear() const
  {
    for(int i = 0; i < LEN_SUBSCRIBER_MASK; ++i)
    {
      if(m_mask[i] != 0)
      {
        return false;
      }
    }

    return true;
  }

  /*---------------------------------------------------------------------------
  subscribe: given a notifier type, set the corresponding mask bit.
  ---------------------------------------------------------------------------*/
  void SubscriberMask::subscribe(unsigned int bitIndex)
  {
    unsigned long    bitMask;
    unsigned long &  maskWord = const_cast<unsigned long &> (getMaskWord(bitIndex, bitMask));

    maskWord |= bitMask;
  }

  /*---------------------------------------------------------------------------
  unsubscribe: given a notifier type, clear the corresponding mask bit.
  ---------------------------------------------------------------------------*/
  void SubscriberMask::unsubscribe(unsigned int bitIndex)
  {
    unsigned long    bitMask;
    unsigned long &  maskWord = const_cast<unsigned long &> (getMaskWord(bitIndex, bitMask));

    maskWord &= ~bitMask;
  }

  /*---------------------------------------------------------------------------
  isSubscribed: given a notifier type, is the corresponding mask bit on?
  ---------------------------------------------------------------------------*/
  bool SubscriberMask::isSubscribed(unsigned int bitIndex) const
  {
    unsigned long    bitMask;
    const unsigned long &  maskWord = getMaskWord(bitIndex, bitMask);
    bool   isSubscribed = (maskWord & bitMask) != 0;

    return isSubscribed;
  }

  /*---------------------------------------------------------------------------
  == comparison operator.
  ---------------------------------------------------------------------------*/
  bool operator==(const SubscriberMask & a,  const SubscriberMask & b)
  {
    for(int i = 0; i < SubscriberMask::LEN_SUBSCRIBER_MASK; ++i)
    {
      if(a.m_mask[i] != b.m_mask[i])
      {
        return false;
      }
   }

    return true;
  }

  /*---------------------------------------------------------------------------
  != comparison operator.
  ---------------------------------------------------------------------------*/
  bool operator!=(const SubscriberMask & a,  const SubscriberMask & b)
  {
    return !(a == b);
  }

  /*---------------------------------------------------------------------------
  getMaskWord: given a notifier type, get the mask and the mask word.

  NOTE: THIS FUNCTION ASSUMES THE MASK ARRAY ELEMENTS ARE 4-BYTES LONG.
  ---------------------------------------------------------------------------*/
  const unsigned long & SubscriberMask::getMaskWord(
    unsigned int bitIndex,       // bit within multi-word mask
    unsigned long & mask) const  // output parameter
  {
    int  elementIndex = bitIndex >> 5;    // divide by # bits in unsigned long
    int  bitInWord = bitIndex - (elementIndex << 5);
    mask = 1 << bitInWord;

    // Validate element index

    assert(elementIndex < LEN_SUBSCRIBER_MASK);
    if(elementIndex >= LEN_SUBSCRIBER_MASK)
    {
      g_zeroMask = 0;
      return g_zeroMask;
    }

    return m_mask[elementIndex];
  }

}
