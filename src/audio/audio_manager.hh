#ifndef GM_AUDIO_MANAGER_HH
#define GM_AUDIO_MANAGER_HH


#include <list>

#include "miniaudio/miniaudio.h"


namespace tgm
{


    


class AudioDecoder
{
    public:
        AudioDecoder(char const* filename);
        ~AudioDecoder();
        
        auto ptr() { return &m_decoder; }

    private:
        bool m_is_init = false;
        ma_decoder m_decoder;
};


class AudioDevice
{
    public:
        AudioDevice(ma_device_config const& device_config);
        ~AudioDevice();

        auto ptr() { return &m_device; }

        void start();

    private:
        bool m_is_init = false;
        ma_device m_device;
};



class AudioManager
{
    public:
        AudioManager();
        AudioManager(AudioManager const&) = delete;
        AudioManager& operator=(AudioManager const&) = delete;
        ~AudioManager();

        void reproduce_sound(char const* filename);
        void reproduce_sound_loop(char const* filename);

    private:
        ma_engine m_engine;
        bool m_is_engine_init = false;

        std::list<AudioDecoder> m_decoders;	// Using a list in order to have stable pointers
        std::list<AudioDevice> m_devices;
};



} //namespace tgm


#endif //GM_AUDIO_MANAGER_HH