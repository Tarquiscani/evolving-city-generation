#include "audio_manager.hh"


#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "debug/logger/log_streams.hh"


namespace tgm
{

    
AudioDecoder::AudioDecoder(char const* filename)
{
    auto result = ma_decoder_init_file(filename, NULL, &m_decoder);
    if (result != MA_SUCCESS) 
    {
        g_log << "ma_decoder_init_file failed." << std::endl;
    }
    else
    {
        m_is_init = true;
    }
}

AudioDecoder::~AudioDecoder()
{
    if (m_is_init)
    {
        ma_decoder_uninit(&m_decoder);
    }
}

AudioDevice::AudioDevice(ma_device_config const& device_config)
{
    if (ma_device_init(NULL, &device_config, &m_device) != MA_SUCCESS) 
    {
        g_log << "Failed to open playback device." << std::endl;
    }
    else
    {
        m_is_init = true;
    }
}

AudioDevice::~AudioDevice()
{
    if (m_is_init)
    {
        ma_device_uninit(&m_device);
    }
}

void AudioDevice::start()
{
    if (ma_device_start(&m_device) != MA_SUCCESS) 
    {
        g_log << "Failed to start playback device." << std::endl;
    }
}

AudioManager::AudioManager()
{
    auto result = ma_engine_init(NULL, &m_engine);
    if (result != MA_SUCCESS) 
    {
        g_log << "Failed to initialize audio engine." << std::endl;
    }
    else
    {
        m_is_engine_init = true;
    }
}

AudioManager::~AudioManager()
{
    if (m_is_engine_init)
    { 
        ma_engine_uninit(&m_engine);
    }
}

void AudioManager::reproduce_sound(char const* filename)
{
    if (ma_engine_play_sound(&m_engine, filename, NULL) != MA_SUCCESS)
    {
        std::cout << "Failed to reproduce a sound - File address: " << filename << std::endl;
    }
}


static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    auto pDecoder = ((AudioDecoder*)pDevice->pUserData)->ptr();
    if (pDecoder == NULL) {
        return;
    }

    /* Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping(). */
    ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

void AudioManager::reproduce_sound_loop(char const* filename)
{
    m_decoders.emplace_back(filename);
    auto & decoder = m_decoders.back();
    
    /*
    A decoder is a data source which means we just use ma_data_source_set_looping() to set the
    looping state. We will read data using ma_data_source_read_pcm_frames() in the data callback.
    */
    ma_data_source_set_looping(decoder.ptr(), MA_TRUE);
    
    auto deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.ptr()->outputFormat;
    deviceConfig.playback.channels = decoder.ptr()->outputChannels;
    deviceConfig.sampleRate        = decoder.ptr()->outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    m_devices.emplace_back(deviceConfig);
    auto & device = m_devices.back();

    device.start();
}



} //namespace tgm