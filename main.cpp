#include "Pa.h"
#include <math.h>

#define tau 6.2831853
#define sample_rate 44100


typedef struct{

  double phase = 0;
  double step = 0;
  float amp = 1; 
  
  void set(float f, float a){
      step = tau*f/sample_rate;
      amp = a;
  }
  
  double inc(){
      phase += step;
      if(step >= tau){ phase -= tau; }
      return sin(phase)*amp;
  }
    
}Osc;

class Oscs{

public:  
    
    Osc oscs[40];

    Oscs(){

        for (int i = 0; i < 40; i++){
            oscs[i].set( 50+40*pow(2, ((40+i)/16.0))  , (float)i/20.0);
        }

    }

    float sum(){
        double sum = 0;
        for (int i = 0; i < 40; i++) {
            sum += oscs[i].inc();
            }
        return sum/40.0;   
    }
     
};



void minifunc(const float* in, float* out, unsigned long frames, void* data){
    
           
     Oscs *o = (Oscs*)data;
    
    for(unsigned long i=0; i< frames; i++ ){

         *out++ = o->sum();
         
     }
       
}

void streamFinished(void* data){
    Oscs *o = (Oscs*)data;
    o->~Oscs(); 
}

Oscs o;

int main(){

  //  Oscs oo;
   
    Pa a(minifunc, &o);
   // a.setFinishedCallBack(streamFinished); 

    a.start(Pa::waitForKey);
    printf("done");   
    return 0;
}

