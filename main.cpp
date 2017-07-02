#include <stdio.h>
#include <math.h>
#include "portaudio.h"

#define TAU 6.2831853


void initTable(float table[], int size){
    for(int i = 0; i < size; i++){
        table[i] = sin(TAU*(1/(float)i));
    }
}

class Osc{
public:
float freq;
float phase = 0;
float step;
float* table;
int tsize;
int srate;

Osc(float* table, int tsize, float freq, int srate){
    this->freq = freq;
    this->table = table;
    this->tsize = tsize;
    this->srate = srate;   
    step = freq*tsize/srate;
}    
float inc(){
    phase += step;
    if(phase >= tsize){ phase -= tsize; }
    return table[(int)phase];
}    
      
};

void callback(int a, void* userdata){
    
    Osc *o = (Osc*)userdata;
    
    printf("hi %i\n", a);
    printf("hi %f\n", o->inc());
    printf("hi %f\n", o->inc());


    
}

static int patestCallback( const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData )
{
     float *out = (float*)outputBuffer;
     unsigned long i;
 
     (void) timeInfo; /* Prevent unused variable warnings. */
     (void) statusFlags;
     (void) inputBuffer;
     
     for( i=0; i<framesPerBuffer; i++ ){
         *out = 0;
         
         //////////
         
         *out++;
     }
     
     /////////////////test...
    Osc *o = (Osc*)userData;
    
    printf("hi %f\n", o->inc());
    printf("hi %f\n", o->inc());
    ////////////////////////////
     
     return paContinue;
}


class Pa{
int(*PaCallBack)(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData );   
 void* userDataType;
 PaStream *stream;
 unsigned int inchannels = 0;
 unsigned int outchannels = 1;
 unsigned int framesperbuffer = 0;
 unsigned int samplerate = 44100;
 void* streamFinished = NULL;
 
 
public:    

//     outputParameters.channelCount = 1;       /* mono output */
//     outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
//     outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
//     outputParameters.hostApiSpecificStreamInfo = NULL;
// 
//     err = Pa_OpenStream(
//               &stream,
//               NULL, /* no input */
//               &outputParameters,
//               samplerate,
//               framesperbuffer,
//               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
//               portAudioCallback,
//               &userdataType );

//PA p(patestCallback, StreamFinished, SAMPLE_RATE, FRAMES_PER_BUFFER, &oscs);
    
/*
 callback, sample rate, framesperbuffer, inchannels, outchannels, *userdata
 */    
    
Pa(int(&func)(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData ),
 unsigned int samplerate, unsigned int framesperbuffer, unsigned int inchannels, unsigned int outchannels, void *dataType )
{
    this->PaCallBack = func;
    this->userDataType = dataType;
    this->samplerate = samplerate;
    this->framesperbuffer = framesperbuffer;
    this->inchannels = inchannels;
    this->outchannels = outchannels;
}
    
 void start(int a){
     PaCallBack(NULL, NULL,0, NULL, NULL, userDataType);
 }

 
 void setFinishedCallBack(void(&func)(void* a)){
 
}
 
};

int main(void)
{
    float table[2048];
    initTable(table,2048);
    
    Osc osc(table, 2048, 220, 4410);
    
    Pa a(patestCallback, 44100, 0, 0, 1, &osc);   
    
    a.start(5);
 
}




