#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <thread>

#define TAU 6.2831853

//#if __cplusplus > 199711L
//#define C11 
//#include <thread>
//#endif



void initTable(float table[], int size){
    for(int i = 0; i < size; i++){
        table[i] = sin(TAU*(i/(float)size));
    }
}

class Osc{
public:
float freq;
float phase = 0; 
float p1 = 0, p2 = 0, p3 = 0, p4 = 0;
float step = 5;
float* table;
int tsize;
int srate;
float s1, s2, s3, s4;
float a = 1, a1 = 0.8, a2 = 0.7, a3 = 0.6, a4 = 0.5;

Osc(float* table, int tsize, float freq, int srate){
    this->freq = freq;
    this->table = table;
    this->tsize = tsize;
    this->srate = srate;   
    this->step = freq*(float)tsize/(float)srate;
    this->s1 = freq*2*(float)tsize/(float)srate;
    s2 = freq*3*(float)tsize/(float)srate;
    s3 = freq*5*(float)tsize/(float)srate;
    s4 = freq*7*(float)tsize/(float)srate;

}    
float inc(){
//    phase += step;
//    if(phase >= tsize){ phase -= tsize; }
//    return table[(int)phase];
    float out =  incp(phase,step, a)+incp(p1, s1, a1)+incp(p2, s2, a2)+incp(p3, s3, a3)+incp(p4, s4, a4);
    return out/4.0;
}

float incp(float &p, float &s, float &amp){
    p += s;
    if(p >= tsize){ p -= tsize; }
    return table[(int)p]*amp;
}
      
};



static void miniFunc(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,void* userdata )
{
    float *out = (float*)outputBuffer;
     unsigned long i;

     (void) inputBuffer;
     
    Osc *o = (Osc*)userdata;
    
    for( i=0; i< framesPerBuffer; i++ ){
         *out = 0;
         
         *out = o->inc();
        
         *out++;
     }
}

void floatFunc(const float* in, float* out, unsigned long frames, void* data){
    
           
    Osc *o = (Osc*)data;
    
    for(unsigned long i=0; i< frames; i++ ){
         
         *out = o->inc();       
          out++;
     }
    
    
}

static int patestCallback( 
                            const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userdata )
{
     float *out = (float*)outputBuffer;
     unsigned long i;

     (void) timeInfo; /* Prevent unused variable warnings. */
     (void) statusFlags;
     (void) inputBuffer;
     
    Osc *o = (Osc*)userdata;
    
    for( i=0; i< framesPerBuffer; i++ ){
         *out = 0;
         
         *out = o->inc();
        
         *out++;
     }
    
    //    miniFunc(inputBuffer, outputBuffer, framesPerBuffer, userdata );
    
     return paContinue;
}


class Pa{

public:    
    
//enum RunMode {wait, waitInThread, dontTerminate, sleep, waitForKey };
enum RunMode {dontTerminate, wait, sleep, waitForKey};
    
private: 
    
 RunMode mode = RunMode::dontTerminate;   

 typedef int(*mainCallBack)(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*); 
 mainCallBack PaCallBack;

 typedef void (*miniCallBack)(const void*, void*, unsigned long,void*);
 static miniCallBack cbPtr;
 
 typedef void (*floatCallBack)(const float*, float*, unsigned long,void*);
 static floatCallBack floatCb;
 
 void (*streamFinished)(void*) = NULL;
 
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
 
//#ifdef C11
// std::thread t;
//#endif 
 
public:
 
Pa(mainCallBack func, void* data)
{
      PaCallBack = func;
      userDataType = data;
}

Pa(mainCallBack func, unsigned int samp, unsigned int frames, unsigned int inch, unsigned int outch, void *data)
{
      PaCallBack = func;
      userDataType = data;
      samplerate = samp;
      framesperbuffer = frames;
      inchannels = inch;
      outchannels = outch;
}

Pa(floatCallBack func, void* data)
{
    floatCb = func; 
    userDataType = data;
}

Pa(floatCallBack func, unsigned int samp, unsigned int frames, unsigned int inch, unsigned int outch, void *data)
{
      floatCb = func;
      userDataType = data;
      samplerate = samp;
      framesperbuffer = frames;
      inchannels = inch;
      outchannels = outch; 
}

Pa(miniCallBack func, void* data)
{
      cbPtr = func;
      userDataType = data;   
}

Pa(miniCallBack func, unsigned int samp, unsigned int frames, unsigned int inch, unsigned int outch, void *data)
{
      cbPtr = func;
      userDataType = data;
      samplerate = samp;
      framesperbuffer = frames;
      inchannels = inch;
      outchannels = outch; 
}


private:

void restart(Pa::RunMode mode){
    
if(!Pa_IsStreamStopped(stream)){
    return;
}
    PaError err = 0;
    
    err = Pa_StartStream( stream );
    
    if( err != paNoError ) goto error;
    
         if(mode == RunMode::wait){ 
            runloop = true;
            while(runloop){
            Pa_Sleep(100);
            } 
       }
       
       if(mode == RunMode::sleep){
           Pa_Sleep(sleepTime);
       }
       
       if(mode == RunMode::waitForKey){
           printf("\nPress any key stop stream:\n");
           fflush(stdout);
           getchar();
       }
    

       if(mode != RunMode::dontTerminate){
        err = Pa_StopStream( stream );  
        if( err != paNoError ) goto error;
       }
    
    return;  
    
    error:               
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    fflush(stdout);
}    

void startStream(Pa::RunMode mode){
    
    if(init){
        restart(mode);
        return;
    }
    
    PaError err;
    PaStreamParameters outputParameters;   
    PaStreamParameters inputParameters;
    
    
     err = Pa_Initialize();
     if( err != paNoError ) goto error;

    if(outchannels > 0){
        outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
       if (outputParameters.device == paNoDevice) {
         fprintf(stderr,"Error: No default output device.\n");
         goto error;
       }
        outputParameters.channelCount = outchannels;      
        outputParameters.sampleFormat = sampleFormat; 
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;   
    }
     
    if(inchannels > 0){ 
        inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
        if (inputParameters.device == paNoDevice) {
          fprintf(stderr,"Error: No default input device.\n");
          goto error;
        }
        inputParameters.channelCount = inchannels;  
        inputParameters.sampleFormat = sampleFormat; 
        inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;
    }
     
     
      err = Pa_OpenStream(
               &stream,
               (inchannels > 0 ? &inputParameters : NULL), 
               (outchannels > 0 ? &outputParameters : NULL),
               samplerate,
               framesperbuffer,
               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
           //    (cbPtr == NULL ? PaCallBack : &Pa::paCb),
              &Pa::paCb,
               userDataType );
     
       if( err != paNoError ) goto error;
      
       if(streamFinished != NULL){   
        err = Pa_SetStreamFinishedCallback( stream, streamFinished );
        if( err != paNoError ) goto error;
       }
   
       init = true;
     
       err = Pa_StartStream( stream );
       if( err != paNoError ) goto error;
       
//       printf("Run Mode: %i\n", mode);
//       fflush(stdout);
       
   //    if(mode == RunMode::wait || mode == RunMode::waitInThread){ 
         if(mode == RunMode::wait){ 
            runloop = true;
            while(runloop){
            Pa_Sleep(100);
            } 
       }
       
       if(mode == RunMode::sleep){
           Pa_Sleep(sleepTime);
       }
       
       if(mode == RunMode::waitForKey){
           printf("\nPress any key stop stream:\n");
           fflush(stdout);
           getchar();
       }
    

       if(mode != RunMode::dontTerminate){
        err = Pa_StopStream( stream );  
        if( err != paNoError ) goto error;
       }
       
//       printf("\nstream: %i\n", Pa_IsStreamStopped(stream));
//       fflush(stdout);
       
     return;
       
     error:               
     Pa_Terminate();
     fprintf( stderr, "An error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
     fflush(stdout);
     
 }
 
static PaError paCb(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* udata){
     
 
    if(cbPtr == NULL){
        floatCb((const float*)inputBuffer, (float*)outputBuffer, framesPerBuffer, udata);
    }else{
        cbPtr(inputBuffer, outputBuffer, framesPerBuffer, udata);   
    }
    
     return paContinue;
     
 }

public:
    

 void start(){
//    if(mode == RunMode::waitInThread){
//        #ifdef C11
//         t = std::thread(&Pa::startStream, this, mode);
//        #else
//         startStream(mode);
//        #endif    
//    }else{
//        startStream(mode); 
//    } 
     startStream(mode); 
 }

 void start(RunMode mode){
     this->mode = mode;
     startStream(this->mode);
 }
 
 void start(unsigned long sleeptime){
    this->sleepTime = sleeptime;
    mode = RunMode::sleep;
    startStream(mode);   
 }
 
// void start(bool wait){ //ambiguous w long parameter needs template..
//  mode = wait? RunMode::wait : RunMode::dontTerminate;
//  startStream(mode);
// }
  
 
// void setRunMode(RunMode in){ 
//     mode = in;
// }
      
 void setSleepTime(unsigned long time){
     sleepTime = time;
 }
 
 void setSampleFormat(PaSampleFormat format){
     sampleFormat = format;
 }
 
 ~Pa(){
     Pa_Terminate();
 }

 void stop(){
     runloop = false;
//    #ifdef C11
//     t.join();
//    #endif 
     PaError err = 0;
     
     if(!Pa_IsStreamStopped(stream))
     err = Pa_StopStream( stream );
     
     if( err !=  paNoError)  
         goto error;
     
     return;
     
     error:               
     Pa_Terminate();
     fprintf( stderr, "An errooor occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );    
     fflush(stdout);
     
 }
 
void stop(bool in){
     runloop = false;
//    #ifdef C11
//     t.join();
//    #endif 
     PaError err = 0;
     
     if(!Pa_IsStreamStopped(stream))
     err = Pa_StopStream( stream );
     
       if(in){
       err = Pa_CloseStream( stream );
       init = false;
       if( err != paNoError ) goto error;
       }
     
     if( err !=  paNoError)  
         goto error;
     
     return;
     
     error:               
     Pa_Terminate();
     fprintf( stderr, "An errooor occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );    
     fflush(stdout);
     
 }

void setFinishedCallBack(void(&func)(void* a)){
    streamFinished = func;
}

  
};
Pa::miniCallBack Pa::cbPtr = NULL;
Pa::floatCallBack Pa::floatCb = NULL;

void streamFinished(void* in){
    printf("\nstream finished\n");
    fflush(stdout);
}

int main(void)
{
    int aa = 0;
    float table[2048];
    initTable(table,2048);
    
    Osc osc(table, 2048, 78, 44100);

    //  Pa a(patestCallback, &osc);
    //    Pa a(floatFunc, &osc);
    Pa a(miniFunc, 44100, 0, 0, 1, &osc);
    
      //  a.setFinishedCallBack(streamFinished);

   // Pa a(patestCallback, 44100, 0, 0, 1, &osc);

        
  //  a.start(5000);
//    a.start(Pa::dontTerminate);
//    getchar();
//    
//    a.stop(true);
//    
//    getchar();
//    
//    a.start(2000);
    
     a.start(Pa::wait);
    
//    printf("restart char1:...\n");
//    fflush(stdout);
//    getchar();
    
   // getchar();
    
    
   // a.stop(true);
   // a.start(3000);
    
//    a.stop();
//    
//    printf("restart char2:...\n");
//    fflush(stdout);
//    getchar();
//    
//    a.start(Pa::waitForKey);
    
    
    


}