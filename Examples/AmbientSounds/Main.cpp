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

#include <errno.h>
#include <stdlib.h>

/** A simple application that plays audio samples.
    
    
*/

int main(int argc, char ** argv)
{
  const char * directory = 0;
  float gain = 0.2f;

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--dir") == 0 && (i + 1) < argc)
      directory = argv[++i];
    else if(strcmp(argv[i], "--gain") == 0 && (i + 1) < argc)
      gain = atof(argv[++i]);
    else if(strcmp(argv[i], "--verbose") == 0)
      Radiant::enableVerboseOutput(true);
    else {
      printf("%s # Unknown argument \"%s\"\n", argv[0], argv[i]);
      return EINVAL;
    }
  }


  Resonant::DSPNetwork dsp;

  dsp.start();

  Resonant::ControlData control, control2;

  Resonant::DSPNetwork::Item item;
  Resonant::ModuleSamplePlayer * player = new Resonant::ModuleSamplePlayer(0);
  item.m_module = player;
  item.m_module->setId("sampleplayer");
  
  control.writeInt32(2);
  control.rewind();

  item.m_module->control("channels", & control);

  player->createAmbientBackground(& dsp, directory, gain);
  
  dsp.addModule(item);
  
  Radiant::Sleep::sleepS(1000);

  dsp.stop();

  return 0;
}

