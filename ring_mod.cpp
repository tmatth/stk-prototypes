/* ring_mod.cpp
 *
 * Copyright (C) 2010 Tristan Matthews <le.businessman@gmail.com>
 * A simple ring modulator example using STK that modulates the amplitude
 * of an incoming capture signal (i.e. microphone input) in real-time.
 */

#include <stk/SineWave.h>
#include <stk/RtAudio.h>

struct Effects {
    Effects() : sine() {}
    stk::SineWave sine;
};

int cleanup(RtAudio &adac)
{
    if (adac.isStreamOpen())
        adac.closeStream();
    return 0;
}

// dsp callback
int process(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double /*streamTime*/, RtAudioStreamStatus status, void *data)
{
    using namespace stk;
    register StkFloat *out = static_cast<StkFloat*>(outputBuffer);
    register StkFloat *in = static_cast<StkFloat*>(inputBuffer);
    // the number of input and output channels is equal 
    if (status)
        std::cout << "Stream over/underflow detected." << std::endl;
    SineWave &sine = static_cast<Effects*>(data)->sine;

    // stereo, interleaved channels
    while (nBufferFrames--)
    {
        StkFloat sineVal = sine.tick();
        *out++ = sineVal * *in++;
        *out++ = sineVal * *in++;
    }

    return 0;
}

int main()
{
    using namespace stk;
    RtAudio adac;  
    Effects effects;

    if (adac.getDeviceCount() < 1) {
        std::cout << "\nNo audio devices found!\n";
        return cleanup(adac);
    }

    // set the same number of channels for both input and output
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : 
        RTAUDIO_FLOAT32;
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = 0; // first available device
    iParams.nChannels = 2;
    oParams.deviceId = 0; // first available device
    oParams.nChannels = 2;

    try {
        adac.openStream(&oParams, &iParams, format, Stk::sampleRate(), 
                &bufferFrames, &process, static_cast<void *>(&effects));
    }
    catch (RtError& e) {
        e.printMessage();
        return cleanup(adac);
    }

    // set the frequency of our modulator
    effects.sine.setFrequency(440);

    try { 
        adac.startStream();
        char input;
        std::cout << "Running...press <enter> to quit.\n";
        std::cin.get(input);
        // stop the stream
        adac.stopStream();
    }
    catch (RtError &e)
    {
        e.printMessage();
        return cleanup(adac);
    }
    
    return 0;
}

