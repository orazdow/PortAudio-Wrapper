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
    
}
Osc;

class Oscs{

public:  
    
    Osc oscs[55];

    Oscs(){

        for (int i = 0; i < 55; i++){
            oscs[i].set( 10+30*pow(2, ((40+i)/16.0))  , 1);
        }

    }

    float sum(){
        double sum = 0;
        for (int i = 0; i < 55; i++) {
            sum += oscs[i].inc();
            }
        return sum/55.0;   
    }
    
    ~Oscs(){
      //  printf("I'm ded");
    }
     
};



void minifunc(const float* in, float* out, unsigned long frames, void* data){    
           
     Oscs *o = (Oscs*)data;
    
    for(unsigned long i=0; i< frames; i++ ){

         *out++ = o->sum();
         
     }
       
}


int main(){
    Oscs o;
    
    Pa a(minifunc, &o);
    a.start(Pa::waitForKey);
    
    return 0;
}

