# PortAudio Wrapper

Helper class for PortAudio

---

This wrapper provides a simple interface to PortAudio's callback API for reading and writing to an audio stream. 

### Basic usage:

    #include "Pa.h"
    #include "math.h"
    #define TWO_PI 6.2831853

    double phase = 0;
    double step = TWO_PI*440/44100.0;

    void paFunc(const float* in, float* out, unsigned long frames, void* data){    
        // play a 440Hz sine tone
        for(unsigned long i=0; i< frames; i++ ){
             phase += step;
             *out++ = sin(phase)*0.5;
         }

    }

    int main() {
    	// use default parameters, not passing any data
        Pa a(paFunc, NULL);
        // start stream
        a.start(Pa::waitForKey);
        return 0;
    }



