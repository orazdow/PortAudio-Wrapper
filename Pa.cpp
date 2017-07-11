
#include "Pa.h"


Pa::miniCallBack Pa::miniCb = NULL;

Pa::Pa(mainCallBack func, void* data){    
    PaCallBack = func;
    userDataType = data;
}
Pa::Pa(mainCallBack func, unsigned int samp, unsigned int frames, unsigned int inch, unsigned int outch, void *data){    
    PaCallBack = func;
    userDataType = data;
    samplerate = samp;
    framesperbuffer = frames;
    inchannels = inch;
    outchannels = outch;
}
Pa::Pa(miniCallBack func, void* data){
    miniCb = func; 
    userDataType = data;
}
Pa::Pa(miniCallBack func, unsigned int samp, unsigned int frames, unsigned int inch, unsigned int outch, void *data){
    miniCb = func;
    userDataType = data;
    samplerate = samp;
    framesperbuffer = frames;
    inchannels = inch;
    outchannels = outch; 
}

Pa::~Pa(){
     Pa_Terminate();
}

void Pa::start(){
     startStream(mode); 
}
void Pa::start(RunMode mode){
     this->mode = mode;
     startStream(this->mode);
}
void Pa::start(unsigned long sleeptime){
    this->sleepTime = sleeptime;
    mode = RunMode::sleep;
    startStream(mode);  
}

void Pa::stop(){
     runloop = false;

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

void Pa::stop(bool close){
     runloop = false;

     PaError err = 0;
     
     if(!Pa_IsStreamStopped(stream))
         err = Pa_StopStream( stream );
     
    if(close){
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

void Pa::setSleepTime(unsigned long time){
     sleepTime = time;
}
void Pa::setSampleFormat(PaSampleFormat format){
     sampleFormat = format;
}
void Pa::setFinishedCallBack(void(*func)(void* data)){
    streamFinished = func;
}

void Pa::restart(Pa::RunMode mode){
    
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

 
void Pa::startStream(Pa::RunMode mode){
    
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
               (miniCb == NULL ? PaCallBack : &Pa::paCb),
               userDataType );
     
       if( err != paNoError ) goto error;
      
       if(streamFinished != NULL){   
        err = Pa_SetStreamFinishedCallback( stream, streamFinished );
        if( err != paNoError ) goto error;
       }
   
       init = true;
     
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
 
PaError Pa::paCb(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* udata){

    miniCb((const float*)inputBuffer, (float*)outputBuffer, framesPerBuffer, udata);
    
    return paContinue;
}
    
       

