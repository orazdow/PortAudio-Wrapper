#include "pa.h"
#include <math.h>

const float tau = 6.2831853;
const float sample_rate = 44100;

/* FM Example */

// oscillator
struct Osc{
    float freq;
    float phase;
    float out;
};

// increment phase
void increment(Osc* osc, float freq){
    osc->phase += freq*tau/sample_rate;

    if(osc->phase >= tau)
        osc->phase -= tau;

    osc->out = sinf(osc->phase);
}

// fm patch
void fm(Osc* mod, Osc* car, float amp){
    increment(mod, mod->freq);
    increment(car, mod->out*amp + car->freq);
}

// fm data
struct FmData{
    Osc* mod;
    Osc* car;
    Osc* lfo;
};


void minifunc(const float* in, float* out, long frames, void* data){    
           
    FmData* d = (FmData*)data;
    
    for(int i = 0; i < frames; i++){

        increment(d->lfo, d->lfo->freq);
        fm(d->mod, d->car, (1 + d->lfo->out) * 1000);
        *out++ = d->car->out * 0.7;
         
     }
       
}


int main(){
    
    Osc car{290};
    Osc mod{400};
    Osc lfo{0.7};
    FmData data{&mod, &car, &lfo};
    
    Pa a(minifunc, &data);
    a.start(Pa::waitForKey);
    
    return 0;
}

