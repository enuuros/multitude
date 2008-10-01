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

#include <Radiant/Sleep.hpp>
#include <Radiant/Trace.hpp>

#include <Resonant/DSPNetwork.hpp>
#include <Resonant/ModuleSamplePlayer.hpp>

#include <conio.h>

int main(int argc, char ** argv)
{
  const char * file = "../test.wav";

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--sample") == 0 && (i + 1) < argc)
      file = argv[++i];
    else
      printf("%s # Unknown argument \"%s\"\n", argv[0], argv[i]);
  }

  Resonant::DSPNetwork dsp;

  dsp.start();

  Resonant::DSPNetwork::Item item;
  item.m_module = new Resonant::ModuleSamplePlayer(0);
  item.m_module->setId("sampleplayer");
  
  dsp.addModule(item);

  Resonant::ControlData control;

  // Id of the receiving module
  control.writeString("sampleplayer/playsample");
  // File name
  control.writeString(file);
  // Gain
  control.writeFloat32(0.745f);
  // Relative pitch
  control.writeFloat32(0.25);

  Radiant::Sleep::sleepMs(500);

  for(int i = 0; i < 5; i++) {
    Radiant::trace("Playing sample %s", file);
    dsp.send(control);
    Radiant::Sleep::sleepMs(900);
    // Radiant::Sleep::sleepS(3);
  }

  Radiant::Sleep::sleepS(3);

  dsp.stop();

  _getch();

  return 0;
}

