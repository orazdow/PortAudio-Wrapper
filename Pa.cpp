#include <iostream>

#include "Pa.h"

Pa::miniCallBack Pa::miniCb = NULL;

Pa::Pa(mainCallBack func, void* data){    
    PaCallBack = func;
    userDataType = data;
    intialize();
}
Pa::Pa(mainCallBack func,unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames, void *data){    
    PaCallBack = func;
    userDataType = data;
    samplerate = samp;
    framesperbuffer = frames;
    inchannels = inch;
    outchannels = outch;
    intialize();
}
Pa::Pa(miniCallBack func, void* data){
    miniCb = func; 
    userDataType = data;
    intialize();
}
Pa::Pa(miniCallBack func,  unsigned int inch, unsigned int outch, unsigned int samp, unsigned int frames, void *data){
    miniCb = func;
    userDataType = data;
    samplerate = samp;
    framesperbuffer = frames;
    inchannels = inch;
    outchannels = outch; 
    intialize();
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

void Pa::startStream(Pa::RunMode mode){
    
    if(init){
        restart(mode);
        return;
    }
    
    PaError err;
    PaStreamParameters outputParameters;   
    PaStreamParameters inputParameters;
    
    if(outchannels > 0){
        if( outputdevice < 0){
            outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
        }else{
            outputParameters.device = outputdevice;    
        }
       if (outputParameters.device == paNoDevice) {
         fprintf(stderr,"\nError: No default output device.\n");
         goto error;
       }
        outputParameters.channelCount = outchannels;      
        outputParameters.sampleFormat = sampleFormat; 
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;   
    }
     
    if(inchannels > 0){ 
        if( inputdevice < 0){
            inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
        }else{
            inputParameters.device = inputdevice;
        }
        if (inputParameters.device == paNoDevice) {
          fprintf(stderr,"\nError: No default input device.\n");
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
     fprintf( stderr, "\nAn error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
     fflush(stdout); 
}

void Pa::intialize(){
     PaError err;
     err = Pa_Initialize();
     if( err != paNoError ){  
        fprintf( stderr, "\nAn error occured initializing portaudio\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );    
        fflush(stdout); 
        Pa_Terminate();
     }
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
    fprintf( stderr, "\nAn error occured while using the portaudio stream\n" );

     err = Pa_Initialize();
     if( err != paNoError ) goto error;  
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
     fprintf( stderr, "\nAn error occured while using the portaudio stream\n" );
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
     fprintf( stderr, "An error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );    
     fflush(stdout);  
}

void Pa::listDevices(){
  const PaDeviceInfo *info;
  int numdevices = 0;
  numdevices = Pa_GetDeviceCount();
  int defaultin = -1, defaultout = -1;
  
  if(numdevices <= 0){
      printf("no devices found\n");
      return;
  }
  defaultin = Pa_GetDefaultInputDevice();
  defaultout = Pa_GetDefaultOutputDevice();
  if(defaultin == paNoDevice){ defaultin = -1;}
  if(defaultout == paNoDevice){ defaultout = -1;}
  printf("Devices:\n");
  for (int i = 0; i < numdevices; i++) {
      
      info = Pa_GetDeviceInfo(i);
      
          const char* inout = "";
          if(info->maxInputChannels > 0 && info->maxOutputChannels == 0){
              inout = "input";
          }else if(info->maxInputChannels == 0 && info->maxOutputChannels > 0){
              inout = "output";
          }
          
      if(i == defaultin || i == defaultout){

          printf("%i: (%s) %s (%s) --default--\n", i, inout, info->name, apiName(info->hostApi));
      
      }    
      else{
         printf("%i: (%s) %s (%s)\n", i, inout, info->name, apiName(info->hostApi));
       
      }
    }

}

void Pa::getDeviceInfo(unsigned int index){
  const PaDeviceInfo *info;
  int numdevices = 0;
  numdevices = Pa_GetDeviceCount();
  int defaultin = -1, defaultout = -1;
  
  if(numdevices <= 0){
      printf("no devices found\n");
      return;
  }
  if(index >= numdevices ){
      printf("invalid index\n");
      return;     
  }
  
  info = Pa_GetDeviceInfo(index);
  printf("\nDevice: (%u)\n", index);
  printf("%s\n", info->name);
  printf("API: %s\n", apiName(info->hostApi));
  printf("Input channels: %i\n", info->maxInputChannels);
  printf("Output channels: %i\n", info->maxOutputChannels);
  printf("Default high input latency: %f\n", info->defaultHighInputLatency);
  printf("Default high output latency: %f\n", info->defaultHighOutputLatency);
  printf("Default low input latency: %f\n", info->defaultLowInputLatency);
  printf("Default low input latency: %f\n", info->defaultLowOutputLatency);
  printf("Default sampling rate: %f\n", info->defaultSampleRate);

}


const char* Pa::apiName(unsigned int index){
    
    const PaHostApiInfo* info;
    int hostindex = -1;
    int apicount = 0;
    int defaultapi = -1;
    
        apicount =  Pa_GetHostApiCount();
        hostindex = Pa_GetDefaultHostApi();
        defaultapi = Pa_GetDefaultHostApi();
        
        if(apicount <= 0){
            return "";
        }
        if(index > apicount-1){
            return "";
        }
    
        info =  Pa_GetHostApiInfo(index);
        
        return info->name;
    
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

void Pa::setInputDevice(unsigned int index){
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount(); 
    if(numdevices == 0){
        printf("\nNo devices found\n"); 
        return;
    }
    if(index >= numdevices){
        printf("\nInvalid index\n"); 
        return;        
    }
    const PaDeviceInfo* info = Pa_GetDeviceInfo(index);
    if(info == NULL){
         printf("\nNInvalid index\n"); 
        return;       
    }
    if(info->maxInputChannels == 0){
         printf("\nInvalid index\n"); 
        return;       
    }
    inputdevice = index;
    printf("\nInput device set to: %u: %s\n", index, info->name);
}

void Pa::setOutputDevice(unsigned int index){
    int numdevices = 0;
    numdevices = Pa_GetDeviceCount(); 
    if(numdevices == 0){
        printf("\nNo devices found\n"); 
        return;
    }
    if(index >= numdevices){
        printf("\nInvalid index\n"); 
        return;        
    }
    const PaDeviceInfo* info = Pa_GetDeviceInfo(index);
    if(info == NULL){
         printf("\nNInvalid index\n"); 
        return;       
    }
    if(info->maxOutputChannels == 0){
         printf("\nInvalid index\n"); 
        return;       
    }
    outputdevice = index; 
    printf("\nOutput device set to: %u: %s\n", index, info->name);
}

PaError Pa::paCb(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* udata){

    miniCb((const float*)inputBuffer, (float*)outputBuffer, framesPerBuffer, udata);
    
    return paContinue;
}
    
       

