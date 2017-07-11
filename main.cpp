#include "Pa.h"
#include <math.h>

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



void minifunc(const float* in, float* out, unsigned long frames, void* data){
    
           
    Osc *o = (Osc*)data;
    
    for(unsigned long i=0; i< frames; i++ ){
         
         *out = o->inc();       
          out++;
     }
    
    
}


int main(){
        
    float table[2048];
    initTable(table,2048);
    
    Osc osc(table, 2048, 78, 44100);
    
    Pa a(minifunc, &osc);

    a.start(Pa::waitForKey);
    
    return 0;
}