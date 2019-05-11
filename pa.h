#ifndef PA_H
#define PA_H

#if __cplusplus > 199711L
#define CPP11
#endif

#include <stdio.h>
#include "portaudio.h"

#ifdef CPP11
#include <memory>
#endif

class Pa{
    
public:    
    
enum RunMode {dontTerminate, wait, sleep, waitForKey};

private: 
    
RunMode mode = RunMode::dontTerminate;   

typedef int(*mainCallBack)(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*); 
mainCallBack PaCallBack = nullptr;

typedef void (*miniCallBack)(const float*, float*, long, void*);
static miniCallBack miniCb;
 
void (*streamFinished)(void*) = nullptr;
void* userDataType = NULL;
PaStream *stream;
PaSampleFormat sampleFormat = paFloat32;
unsigned long sleepTime = 10000;
unsigned int inchannels = 0;
unsigned int outchannels = 1;
unsigned int framesperbuffer = 0;
unsigned int samplerate = 44100;
int inputdevice = -1;
int outputdevice = -1;
bool runloop = false;
bool init = false; 
#ifdef CPP11
std::shared_ptr<void> s_ptr;
#endif

// private functions:
void startStream(Pa::RunMode mode);
void restart(Pa::RunMode mode);
void intialize();
const char* apiName(unsigned int index);
static PaError paCb(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* udata);
 
public:  
    
// constructors:     
    
// full callback, void* data    
Pa(mainCallBack func, void* data); 
// full callback, inchannels, outchannels, sampling rate, frames per buffer,  void* data 
Pa(mainCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  void *data); 
// mini callback, void* data
Pa(miniCallBack func, void* data); 
// mini callback, inchannels, outchannels, sampling rate, frames per buffer, void* data
Pa(miniCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  void *data); 
#ifdef CPP11
// full callback, shared_ptr data
Pa(mainCallBack func, std::shared_ptr<void> data); 
// full callback, params, shared_ptr data
Pa(mainCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  std::shared_ptr<void> data); 
// mini callback, shared_ptr data
Pa(miniCallBack func, std::shared_ptr<void> data); 
// mini callback, params, shared_ptr data
Pa(miniCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  std::shared_ptr<void> data); 
#endif
~Pa();

// public functions:
void start();
void start(RunMode mode);
void start(unsigned long sleeptime);
void stop();
void stop(bool close);
void terminate();
void setSleepTime(unsigned long time); 
void setSampleFormat(PaSampleFormat format);
void setFinishedCallBack(void(*func)(void* data));   
void listDevices();
void getDeviceInfo(unsigned int index);
void setInputDevice(unsigned int index);
void setOutputDevice(unsigned int index);

};


#endif /* PA_H */

