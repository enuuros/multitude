/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DIVA_CONFIG_H
#define DIVA_CONFIG_H

#include <Radiant/Endian.hpp>

// Version number:
#define DIVA_VERSION_MAJOR 0
#define DIVA_VERSION_MINOR 2
#define DIVA_VERSION_SUBMINOR 10

// Is our DIVA sample float or double:
#define DIVA_SAMPLE_IS_FLOAT 1

#if DIVA_SAMPLE_IS_FLOAT == 1
typedef float DIVA_Sample;
#define DIVA_ASF_SAMPLE DIVA_ASF_FLOAT32

#else 
typedef double DIVA_Sample;
#define DIVA_ASF_SAMPLE DIVA_ASF_FLOAT64

#endif

#define DIVA_USE_ALSA_AUDIO 1
#define DIVA_USE_OSS_AUDIO  1
#define DIVA_USE_SGI_AUDIO  

#define DIVA_USE_OSS_MIDI  1
#define DIVA_USE_SGI_MIDI  

#define DIVA_HAVE_PTHREAD 1
// #define DIVA_HAVE_PWCIOCTL_H @HAVE_PWCIOCTL_H@

#endif
