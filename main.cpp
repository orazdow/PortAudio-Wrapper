#include "Pa.h"
#include <math.h>

#define tau 6.2831853
#define sample_rate 44100


class Oscs{

public:    
    
float  p1, p2, p3, p4, // phase
       s1, s2, s3, s4, // step
       a1, a2, a3, a4; // amplitude 

float lfophase = 0;

//float mod = 0;  // <<<<<<<<<<<<< uncommenting this breaks it....?







Oscs(float f1, float a1, float f2, float a2, float f3, float a3, float f4, float a4){
     s1 = tau*f1/sample_rate;  this->a1 = a1;
     s2 = tau*f2/sample_rate;  this->a2 = a2;
     s3 = tau*f3/sample_rate;  this->a3 = a3;
     s4 = tau*f4/sample_rate;  this->a4 = a4; 
 }  
 
    
float inc(float &phase, float step, float amp){
    phase += step;
    if(phase > tau){ phase -= tau; }
    return sinf(phase)*amp;
} 

float sum(){
    float mod = inc(lfophase, 0.00004, 0.006);
    return (inc(p1, s1, a1)+inc(p2, s2+mod, a2)+inc(p3, s3, a3)+inc(p4, s4+mod, a4))*0.3;
}
   
};

//struct Bank{
//    
//Osc o1, o2;
//
//Bank(Osc a, Osc b) : o1(a), o2(b){}
//    
//};

void minifunc(const float* in, float* out, unsigned long frames, void* data){
    
           
    Oscs *o = (Oscs*)data;
    
    for(unsigned long i=0; i< frames; i++ ){
         
         *out = o->sum();
          out++;
     }
    
    
}



int main(){


    Oscs o(340, 1, 400, 0.8, 230, 0.5, 380, 0.6);
    
    //constructor parapms: input channels, output channels, sampling rate, buffer size (0 = auto), user data reference
    Pa a(minifunc, 0, 1, sample_rate, 0, &o);

    a.start(Pa::waitForKey);
       
    return 0;
}