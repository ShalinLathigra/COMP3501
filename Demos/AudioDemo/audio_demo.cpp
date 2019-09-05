#include "audio_manager.h"
#include "bin/path_config.h"

/* Macro for printing exceptions */
#define PrintException(exception_object)\
    std::cerr << exception_object.what() << std::endl

using namespace audio_manager;

int main(void){

    AudioManager am;

    /* Reference index of each file to be played */
    int chord_index, ding_index;

    try {
        /* Initialize audio manager */
        am.Init(NULL);

        /* Set position of listener */
        am.SetListenerPosition(0.0, 0.0, 0.0);

        /* Load first sound to be played */
        std::string filename = std::string(AUDIO_DIRECTORY).append(std::string("/").append(std::string("chord.wav")));
        chord_index = am.AddSound(filename.c_str());
        /* Set sound properties */
        am.SetSoundPosition(chord_index, -10.0, 0.0, 0.0);
        am.SetLoop(chord_index, true);
        /* Play the sound */
        am.PlaySound(chord_index);

        /* Load second sound to be played */
        filename = std::string(AUDIO_DIRECTORY).append(std::string("/").append(std::string("ding.wav")));
        ding_index = am.AddSound(filename.c_str());
        /* Set sound properties */
        am.SetSoundPosition(ding_index, 0.0, 0.0, 0.0);
        /* Play the sound */
        am.PlaySound(ding_index);

        /* Keep program running while something is being played */
        while (am.AnySoundIsPlaying()){
            ;
        }

        /* Shut down the audio manager */
        am.ShutDown();
    }
    catch (std::exception &e){
        PrintException(e);
    }
}
