//
// Created by yot on 09/01/2022.
//

#include "SoundEffects.h"
#include <iostream>

SoundEffects::SoundEffects() {
    //init sound
    SDL_Init(SDL_INIT_AUDIO);

    int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0){
        std::cout << "Could not init audio" << std::endl;
    }
}

SoundEffects::~SoundEffects() {

}

void SoundEffects::addSoundEffect(const char *path) {
    Mix_Chunk *tmpChunk = Mix_LoadWAV(path);

    if (tmpChunk != nullptr){
        mSoundEffectBank.push_back(tmpChunk);
        std::cout << "Added sound: " << path << std::endl;
    }else{
        std::cout << "Could not init sound" << std::endl;
    }
}

void SoundEffects::playSoundEffect(const int which) const {
    if (which > mSoundEffectBank.size() -1){
        std::cout << "Sound out of range" << std::endl;
    }

    Mix_PlayChannel(-1, mSoundEffectBank[which], 0);
    //std::cout << "Played sound at: " << which << std::endl;
}

