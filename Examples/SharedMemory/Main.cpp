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

#include <Radiant/SHMPipe.hpp>
#include <Radiant/Trace.hpp>

#ifdef WIN32
#include <WinPort.h>
#endif

static const char * appname = 0;

int main(int argc, char ** argv)
{
  key_t key = 100;
  int bytes = 1000000;

  appname = argv[0];
  
  for(int i = 0; i < argc; i++) {
    if(strcmp(argv[i], "--key") == 0 && (i + 1) < argc)
      key = atoi(argv[++i]);
    else if(strcmp(argv[i], "--verbose") == 0)
      Radiant::enableVerboseOutput(true);
    else
      printf("%s # Unknown argument \"%s\"\n", appname, argv[i]);
  }
  
  Radiant::info("Setting up shared memory buffer");

  Radiant::SHMPipe * shm = new Radiant::SHMPipe(key, bytes);

  Radiant::info("Deleting shared memory buffer");
  
  delete shm;

  Radiant::info("Done");

  return 0;
}
