
#ifndef PA_H
#define PA_H

#include <stdio.h>
#include "portaudio.h"

class Pa{
    
public:    
    
enum RunMode {dontTerminate, wait, sleep, waitForKey};

private: 
    
RunMode mode = RunMode::dontTerminate;   

typedef int(*mainCallBack)(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*); 
mainCallBack PaCallBack;

typedef void (*miniCallBack)(const float*, float*, unsigned long,void*);
static miniCallBack miniCb;
 
void (*streamFinished)(void*);

void* userDataType;
PaStream *stream;
PaSampleFormat sampleFormat = paFloat32;
unsigned long sleepTime = 10000;
unsigned int inchannels = 0;
unsigned int outchannels = 1;
unsigned int framesperbuffer = 0;
unsigned int samplerate = 44100;
bool runloop = false;
bool init = false; 
 
 void startStream(Pa::RunMode mode);
 void restart(Pa::RunMode mode);

 static PaError paCb(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* udata);
 
public:    
    
Pa(mainCallBack func, void* data);
Pa(mainCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  void *data);
Pa(miniCallBack func, void* data);
Pa(miniCallBack func, unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames,  void *data);

~Pa();

void start();
void start(RunMode mode);
void start(unsigned long sleeptime);
void stop();
void stop(bool close);
void setSleepTime(unsigned long time); 
void setSampleFormat(PaSampleFormat format);
void setFinishedCallBack(void(*func)(void* data));    

};


#endif /* PA_H */

