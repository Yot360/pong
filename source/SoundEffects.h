//
// Created by yot on 08/01/2022.
//

#ifndef PONG___SOUNDEFFECTS_H
#define PONG___SOUNDEFFECTS_H

#include "SDL_mixer.h"
#include <vector>
#include "SDL.h"

class SoundEffects{
public:
    SoundEffects();
    ~SoundEffects();

    void addSoundEffect(const char* path);

    void playSoundEffect(const int which) const;

private:
    std::vector<Mix_Chunk*> mSoundEffectBank;

};

#endif //PONG___SOUNDEFFECTS_H
