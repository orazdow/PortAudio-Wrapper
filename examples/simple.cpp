    #include "Pa.h"
    #include "math.h"
    #define TWO_PI 6.2831853

    double phase = 0, step = TWO_PI*440/44100.0;

    void paFunc(const float* in, float* out, unsigned long frames, void* data){    

        for(unsigned long i = 0; i < frames; i++ ){
             phase += step;
             *out++ = sin(phase)*0.5;
         }
    }

    int main() {
        Pa a(paFunc, NULL);
        a.start(Pa::waitForKey);
        return 0;
    }