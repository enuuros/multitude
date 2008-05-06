include(../multitude.pri)

HEADERS += Dispatcher.hpp
HEADERS += DispatcherPrivate.hpp
HEADERS += Notable.hpp
HEADERS += Notifier.hpp
HEADERS += Subscriber.hpp
HEADERS += SubscriberElement.hpp
HEADERS += SubscriberMask.hpp

SOURCES += Dispatcher.cpp
SOURCES += DispatcherPrivate.cpp
SOURCES += Notifier.cpp
SOURCES += Subscriber.cpp
SOURCES += SubscriberMask.cpp

TARGET = Notable

LIBS += $$LIB_RADIANT

include(../libs.pri)
