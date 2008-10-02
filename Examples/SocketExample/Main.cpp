/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Radiant/TCPServerSocket.hpp>
#include <Radiant/TCPSocket.hpp>

#ifdef WIN32
#include <WinPort.h>
#endif

#ifndef WIN32

const char * strerror_s(const char *, int, int err)
{
  return strerror(err);
}

#endif


using namespace Radiant;

const char * appname = 0;

void runServer(const char * host, int port, bool withBlocking)
{
  printf("Setting up a server socket to %s:%d\n", host, port);

  char buf[1024];

  TCPServerSocket server;
  int err = server.open(host, port, 5);
// err = server.open(0, port, 5)
  if(err) {
    const int  msgSize = 128;
    char  msgBuf[msgSize] = "";
    printf("%s cannot open server socket to %s:%d -> %s\n", 
	   appname, host, port, strerror_s(msgBuf, msgSize, err));
    return;
  }
  
  for(int i = 0; i < 10; i++) {
    puts("Waiting for a connection");

    while(withBlocking && !server.isPendingConnection(1000000)) {
      printf(".");
      fflush(0);
    }

    TCPSocket * socket = server.accept();
    printf("Got a new socket %p\n", socket);
    fflush(0);

    int32_t len = 0;
    buf[0] = '\0';
    socket->read( & len, 4);
    socket->read(buf, len);

    printf("Received \"%s\"\n", buf);
    fflush(0);

    delete socket;
  }
}

void runClient(const char * host, int port, const char * message)
{
  printf("Setting up a client socket to %s:%d\n", host, port);

  TCPSocket socket;
  int err = socket.open(host, port);
  if(err) {
    const int  msgSize = 128;
    char  msgBuf[msgSize] = "";
    printf("%s cannot open client socket to %s:%d -> %s\n", 
	   appname, host, port, strerror_s(msgBuf, msgSize, err));
    return;
  }

  puts("Sending message");
  
  int32_t len = strlen(message) + 1;
  socket.write(&len, 4);
  socket.write(message, len);

  puts("Closing");

  socket.close();
}

void runListener(const char * host, int port, const char *)
{
  printf("Setting up a listener socket to %s:%d\n", host, port);

  TCPSocket socket;
  int err;
  if((err = socket.open(host, port)) != 0) {
    printf("%s cannot open client socket to %s:%d -> %s\n", 
	   appname, host, port, strerror(err));
    return;
  }

  while(true) {
    char buf[64];
    bzero(buf, sizeof(buf));

    int n = socket.read(buf, sizeof(buf) - 1);
    
    buf[n] = 0;

    printf(buf);
    fflush(0);
  }

  socket.close();
}

int main(int argc, char ** argv)
{
  const char * host = "localhost";
  const char * message = "Here we have a message";
  int port = 3456;
  bool islistener = false;
  bool isclient = true;
  bool withBlocking = false;

  appname = argv[0];

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--server") == 0)
      isclient = false;
    else if(strcmp(argv[i], "--listen") == 0)
      islistener = true;
    else if(strcmp(argv[i], "--host") == 0 && (i + 1) < argc)
      host = argv[++i];
    else if(strcmp(argv[i], "--port") == 0 && (i + 1) < argc)
      port = atoi(argv[++i]);
    else if(strcmp(argv[i], "--message") == 0 && (i + 1) < argc)
      message = argv[++i];
    else if(strcmp(argv[i], "--withblocking") == 0)
      withBlocking = true;
    else
      printf("%s # Unknown argument \"%s\"\n", appname, argv[i]);
  }

#ifdef WIN32
  WinPort::initSockets();
#endif

  if(islistener)
    runListener(host, port, message);
  else if(isclient)
    runClient(host, port, message);
  else
    runServer(host, port, withBlocking);

#ifdef WIN32
  WinPort::exitSockets();
#endif

  return 0;
}

