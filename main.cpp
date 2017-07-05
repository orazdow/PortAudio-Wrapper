#include <stdio.h>
#include <math.h>
#include "portaudio.h"

#define TAU 6.2831853


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

//void testcallback(void* userdata){
//    
//    Osc *o = (Osc*)userdata;
//    
////    printf("hi %i\n", a);
////    printf("hi %f\n", o->inc());
////    printf("hi %f\n", o->inc());
//
//    for(int i = 0; i < 1000; i++){
//      //  printf("%f\n", o->inc());
//        o->inc();
//    }
//    
//}

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

    
     return paContinue;
}


class Pa{

int(*PaCallBack)( const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void* data);   
 void* userDataType;
 PaStream *stream;
 unsigned int inchannels = 0;
 unsigned int outchannels = 1;
 unsigned int framesperbuffer = 0;
 unsigned int samplerate = 44100;
 void* streamFinished = NULL;
 /////////
 //void(*testcb)(void*);
 
 
public:    

/*
 callback, sample rate, framesperbuffer, inchannels, outchannels, *userdata
 */    
    
//Pa( void(*func)(void*), void* data )
    Pa(int(&func)( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void*),
            void* data)
 //unsigned int samplerate, unsigned int framesperbuffer, unsigned int inchannels, unsigned int outchannels, void *dataType )
{
    //this->PaCallBack = func;
//    this->userDataType = dataType;
//    this->samplerate = samplerate;
//    this->framesperbuffer = framesperbuffer;
//    this->inchannels = inchannels;
//    this->outchannels = outchannels;
        this->PaCallBack = func;
   // this->testcb = func;
    this->userDataType = data;
}
    
 void start(){

    PaError err;
    PaStreamParameters outputParameters;   
     err = Pa_Initialize();
     if( err != paNoError ) goto error;

        outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
        if (outputParameters.device == paNoDevice) {
         fprintf(stderr,"Error: No default output device.\n");
        // printf("haaaa");
         goto error;
       }

       outputParameters.channelCount = outchannels;      
       outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
       outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
       outputParameters.hostApiSpecificStreamInfo = NULL;  
       
         err = Pa_OpenStream(
               &stream,
               NULL, /* no input */
               &outputParameters,
               samplerate,
               framesperbuffer,
               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
               PaCallBack, //
               userDataType );
     
     if( err != paNoError ) goto error;
       err = Pa_StartStream( stream );
     if( err != paNoError ) goto error;
     
     while(true){}
       
     error:
     Pa_Terminate();
     fprintf( stderr, "An error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
     
 }
 
 void test(){
    // testcb(userDataType);
     PaCallBack(NULL, NULL, 60, NULL, NULL, userDataType );

 }

 
 void setFinishedCallBack(void(&func)(void* a)){
 
}
 
// void setTestCallback(void(&f)(int, void*)){
//     testcb = f;
// }
 
};

int main(void)
{
    float table[2048];
    initTable(table,2048);
    
    Osc osc(table, 2048, 78, 44100);

    Pa a(patestCallback, &osc);

    a.start();

//   a.setTestCallback(testcallback);
  // a.test();
        

}