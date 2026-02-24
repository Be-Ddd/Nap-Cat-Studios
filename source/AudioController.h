//
//  AudioController.h
//
//  Created by Felicia Chen on 2/23/26.
//

#ifndef __AUDIO_CONTROLLER_H__
#define __AUDIO_CONTROLLER_H__
#include <cugl/cugl.h>

class AudioController {
    private:
        static std::shared_ptr<cugl::AssetManager> _assets;
        static std::shared_ptr<cugl::audio::AudioQueue> _queue;
    
    public:
        static void init(const std::shared_ptr<cugl::AssetManager>& assets);
        static void shutdown();
    
        static void playBGM(const std::string& key, bool loop);
        static void pauseBGM();
        static void resumeBGM();
    
        static void playSFX(const std::string& key);
    
};
#endif /*__AUDIO_CONTROLLER_H__*/
