/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QATOMIC_POWERPC_H
#define QATOMIC_POWERPC_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return false; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return false; }

QT_BEGIN_NAMESPACE

#if defined(Q_CC_GNU)

inline bool QBasicAtomicInt::ref()
{
    register int originalValue;
    register int newValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "addi   %[newValue], %[originalValue], %[one]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&b" (originalValue),
                   "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [_q_value] "r" (&_q_value),
                   [one] "i" (1)
                 : "cc", "memory");
    return newValue != 0;
}

inline bool QBasicAtomicInt::deref()
{
    register int originalValue;
    register int newValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "addi   %[newValue], %[originalValue], %[minusOne]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&b" (originalValue),
                   "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [_q_value] "r" (&_q_value),
                   [minusOne] "i" (-1)
                 : "cc", "memory");
    return newValue != 0;
}

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    register int result;
    asm volatile("lwarx  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+12\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-16\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    register int result;
    asm volatile("lwarx  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+16\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-16\n"
                 "isync\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    register int result;
    asm volatile("eieio\n"
                 "lwarx  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+12\n"
                 "stwcx. %[newValue],0,%[_q_value]\n"
                 "bne-   $-16\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    register int originalValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    register int originalValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 "isync\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    register int originalValue;
    asm volatile("eieio\n"
                 "lwarx  %[originalValue], 0, %[_q_value]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    register int originalValue;
    register int newValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd)
                 : "cc", "memory");
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    register int originalValue;
    register int newValue;
    asm volatile("lwarx  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 "isync\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd)
                 : "cc", "memory");
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    register int originalValue;
    register int newValue;
    asm volatile("eieio\n"
                 "lwarx  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 "stwcx. %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd)
                 : "cc", "memory");
    return originalValue;
}

#if defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
#  define LPARX "ldarx"
#  define STPCX "stdcx."
#else
#  define LPARX "lwarx"
#  define STPCX "stwcx."
#endif

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    register void *result;
    asm volatile(LPARX"  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+12\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-16\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    register void *result;
    asm volatile(LPARX"  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+16\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-16\n"
                 "isync\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    register void *result;
    asm volatile("eieio\n"
                 LPARX"  %[result], 0, %[_q_value]\n"
                 "xor.   %[result], %[result], %[expectedValue]\n"
                 "bne    $+12\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-16\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    register T *originalValue;
    asm volatile(LPARX"  %[originalValue], 0, %[_q_value]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    register T *originalValue;
    asm volatile(LPARX"  %[originalValue], 0, %[_q_value]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 "isync\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    register T *originalValue;
    asm volatile("eieio\n"
                 LPARX"  %[originalValue], 0, %[_q_value]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-8\n"
                 : [originalValue] "=&r" (originalValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    register T *originalValue;
    register T *newValue;
    asm volatile(LPARX"  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd * sizeof(T))
                 : "cc", "memory");
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    register T *originalValue;
    register T *newValue;
    asm volatile(LPARX"  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 "isync\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd * sizeof(T))
                 : "cc", "memory");
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    register T *originalValue;
    register T *newValue;
    asm volatile("eieio\n"
                 LPARX"  %[originalValue], 0, %[_q_value]\n"
                 "add    %[newValue], %[originalValue], %[valueToAdd]\n"
                 STPCX"  %[newValue], 0, %[_q_value]\n"
                 "bne-   $-12\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value),
                   [valueToAdd] "r" (valueToAdd * sizeof(T))
                 : "cc", "memory");
    return originalValue;
}

#undef LPARX
#undef STPCX

#else

extern "C" {
    int q_atomic_test_and_set_int(volatile int *ptr, int expectedValue, int newValue);
    int q_atomic_test_and_set_acquire_int(volatile int *ptr, int expectedValue, int newValue);
    int q_atomic_test_and_set_release_int(volatile int *ptr, int expectedValue, int newValue);
    int q_atomic_test_and_set_ptr(volatile void *ptr, void *expectedValue, void *newValue);
    int q_atomic_test_and_set_acquire_ptr(volatile void *ptr, void *expectedValue, void *newValue);
    int q_atomic_test_and_set_release_ptr(volatile void *ptr, void *expectedValue, void *newValue);
    int q_atomic_increment(volatile int *);
    int q_atomic_decrement(volatile int *);
    int q_atomic_set_int(volatile int *, int);
    int q_atomic_fetch_and_store_acquire_int(volatile int *ptr, int newValue);
    int q_atomic_fetch_and_store_release_int(volatile int *ptr, int newValue);
    void *q_atomic_set_ptr(volatile void *, void *);
    int q_atomic_fetch_and_store_acquire_ptr(volatile void *ptr, void *newValue);
    int q_atomic_fetch_and_store_release_ptr(volatile void *ptr, void *newValue);
    int q_atomic_fetch_and_add_int(volatile int *ptr, int valueToAdd);
    int q_atomic_fetch_and_add_acquire_int(volatile int *ptr, int valueToAdd);
    int q_atomic_fetch_and_add_release_int(volatile int *ptr, int valueToAdd);
    void *q_atomic_fetch_and_add_ptr(volatile void *ptr, qptrdiff valueToAdd);
    void *q_atomic_fetch_and_add_acquire_ptr(volatile void *ptr, qptrdiff valueToAdd);
    void *q_atomic_fetch_and_add_release_ptr(volatile void *ptr, qptrdiff valueToAdd);
} // extern "C"


inline bool QBasicAtomicInt::ref()
{
    return q_atomic_increment(&_q_value) != 0;
}

inline bool QBasicAtomicInt::deref()
{
    return q_atomic_decrement(&_q_value) != 0;
}

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return q_atomic_test_and_set_int(&_q_value, expectedValue, newValue) != 0;
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    return q_atomic_test_and_set_acquire_int(&_q_value, expectedValue, newValue) != 0;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    return q_atomic_test_and_set_release_int(&_q_value, expectedValue, newValue) != 0;
}

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    return q_atomic_set_int(&_q_value, newValue);
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    return q_atomic_fetch_and_store_acquire_int(&_q_value, newValue);
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    return q_atomic_fetch_and_store_release_int(&_q_value, newValue);
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    return q_atomic_fetch_and_add_int(&_q_value, valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    return q_atomic_fetch_and_add_acquire_int(&_q_value, valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    return q_atomic_fetch_and_add_release_int(&_q_value, valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return q_atomic_test_and_set_ptr(&_q_value, expectedValue, newValue) != 0;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    return q_atomic_test_and_set_acquire_ptr(&_q_value, expectedValue, newValue) != 0;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    return q_atomic_test_and_set_release_ptr(&_q_value, expectedValue, newValue) != 0;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    return reinterpret_cast<T *>(q_atomic_set_ptr(&_q_value, newValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    return reinterpret_cast<T *>(q_atomic_fetch_and_store_acquire_ptr(&_q_value, newValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    return reinterpret_cast<T *>(q_atomic_fetch_and_store_release_ptr(&_q_value, newValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    return reinterpret_cast<T *>(q_atomic_fetch_and_add_ptr(&_q_value, valueToAdd * sizeof(T)));
}
template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    return reinterpret_cast<T *>(q_atomic_fetch_and_add_acquire_ptr(&_q_value, valueToAdd * sizeof(T)));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    return reinterpret_cast<T *>(q_atomic_fetch_and_add_release_ptr(&_q_value, valueToAdd * sizeof(T)));
}

#endif

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    return fetchAndStoreAcquire(newValue);
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    return fetchAndAddAcquire(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    return fetchAndStoreAcquire(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    return fetchAndAddAcquire(valueToAdd);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_POWERPC_H
