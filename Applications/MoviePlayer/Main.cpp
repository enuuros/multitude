/* COPYRIGHT
 *
 * This file is part of Applications/MoviePlayer.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Applications/MoviePlayer.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "VideoWindow.hpp"

#include <Luminous/Luminous.hpp>

#include <Radiant/Trace.hpp>

#include <QtGui/QApplication>

int main(int argc, char ** argv)
{
  QApplication qa(argc, argv);

  if(argc < 2) {
    puts("No filename given");
    return 0;
  }

  VideoWindow * vw = new VideoWindow();

  vw->makeCurrent();

  Luminous::initLuminous();

  vw->resize(800, 600);

  const char * filename = 0;

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--verbose") == 0)
      Radiant::enableVerboseOutput(true);
    else if(strcmp(argv[i], "--stress") == 0)
      vw->stressTest();
    else {
      filename = argv[i];

      if(!vw->open(filename, 0)) {
        delete vw;
        return -1;
      }
    }
  }

  vw->show();
  vw->raise();

  QObject::connect( & qa, SIGNAL(lastWindowClosed()), & qa, SLOT(quit()));
  
  int res = qa.exec();

  delete vw;

  return res;
}
