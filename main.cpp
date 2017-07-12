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
float step;
float* table;
int tsize;
int srate;

Osc(float* table, int tsize, float freq, int srate){
    this->freq = freq;
    this->table = table;
    this->tsize = tsize;
    this->srate = srate;   
    this->step = freq*(float)tsize/(float)srate;
}    
float inc(){
    phase += step;
    if(phase >= tsize){ phase -= tsize; }
    return table[(int)phase];
}
      
};

struct Bank{
    
Osc o1, o2;

Bank(Osc a, Osc b) : o1(a), o2(b){}
    
};

void minifunc(const float* in, float* out, unsigned long frames, void* data){
    
           
    Bank *b = (Bank*)data;
    
    for(unsigned long i=0; i< frames; i++ ){
         
        *out = b->o1.inc();
          out++;
        *out = b->o2.inc();
          out++;
     }
    
    
}



int main(){
        
    float table[2048];
    initTable(table,2048);
    
    Osc osc1(table, 2048, 158, 44100);
    Osc osc2(table, 2048, 288, 44100);
    
    Bank OscBank(osc1, osc2);
    
    Pa a(minifunc, 0, 2, 44100, 0, &OscBank);

    a.start(Pa::waitForKey);
       
    return 0;
}