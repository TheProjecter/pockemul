#ifndef SOUND_H
#define SOUND_H

#define __PC_RESONATOR_768K__
#include <__sound.h>

beep(byte beep_freq,byte beep_duration) {


    __beep(beep_freq,beep_duration);
}

#endif // SOUND_H
