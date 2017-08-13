# PortAudio Wrapper

Helper class for PortAudio

---

Provides a simple interface to PortAudio's callback API for reading and writing to audio devices. 

### Basic Example:

    #include "Pa.h"
    #include "math.h"
    #define TWO_PI 6.2831853

    double phase = 0, step = TWO_PI*440/44100.0;

    void paFunc(const float* in, float* out, unsigned long frames, void* data){    
        // play a 440Hz sine tone
        for(unsigned long i = 0; i< frames; i++ ){
             *out++ = sin(phase)*0.5;
             phase += step;
         }
    }

    int main() {
        // use default parameters, not passing any data
        Pa a(paFunc, NULL);
        // start stream
        a.start(Pa::waitForKey);
        return 0;
    }
    

### Usage:

**Requirements:**

PortAudio can be obtained [here](http://www.portaudio.com/).


**Initializing:**

Define a callback where audio processing will occur and pass it as the constructor's first argument. The callback can be a simplified type: 
    
    void paCallback(const float* inputBuffer, float* ouputBuffer, unsigned long framesPerBuffer, void* userData){...}
    
or have the full signature specified by PortAudio  

    int paCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, 
            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData){...}
            
The wrapper's constructor can either take 2 arguments:&nbsp; the callback and a reference for passing user data (see passing data):

`Pa a(callback, &data);` 

or 6 arguments: &nbsp;callback, input channels, output channels, sampling rate, frames per buffer (0 will autoselect), userData:

`Pa a(callback, 2, 2, 44100, 256, &data);`
    
The 2-argument constructor is equivalent to: 

`Pa a(callback, 0, 1, 44100, 0, &data);` 

<br>

**Starting / stopping the audio stream:**

Calling `start()` will start the stream.

For convenience, there are four  enum values that can be passed  to `start()` to control the lifetime of the stream:

- `Pa::dontTerminate` &nbsp;(_default_) Starts the stream only. &nbsp;Will stop if `stop()` is called, or the program exits.

- `Pa::wait` &nbsp;Keeps the stream alive in a while loop, can be stopped from another thread.
- `Pa::sleep`&nbsp; Keeps the stream alive for a specified number of milliseconds and then stops the stream. `setSleepTime(millis)` can be called to set the time, or a time in milliseconds can be passed directly i.e `start(5000)`
- `Pa::waitForKey` &nbsp;Keeps the stream alive until a key is entered and then stops the stream.

`stop()` &nbsp;stops the stream. &nbsp;`stop(true)` &nbsp;also closes the stream, which frees some additional resources.

PortAudio will be terminated when the wrapper object is destroyed, but can be explicitly terminated by calling `terminate()`

<br>

**Passing data:**

PortAudio allows for user data  to be passed to the audio callback. This can be omitted in favor of global variables. NULL can be passed for the user data parameter in this case.

To pass data, pass a reference to your data (you can omit the reference if the data is already pointer to an object) to the constructor, and cast the userData argument in the callback to your data type. For example:

    void paFunc(const float* in, float* out, unsigned long frames, void* data){    
        Osc* o = (Osc*)data;
        for(unsigned long i=0; i< frames; i++ ){
             *out++ = o->out();
         }
    }
        
    Osc osc(440);
    Pa a(paFunc, &osc);


PortAudio provides the option to register a callback that will be called when a stream has stopped playing which takes the user data as an argument. This can be used to delete dynamically allocated data, and can be set with `setFinishedCallBack()`.  
An example  might look like:

    void streamFinished(void* data){
        Osc* o = (Osc*)data;
        delete o;
    }

    int main() {
        Osc* osc = new Osc(440);
        Pa a(paFunc, osc);
        a.setFinishedCallBack(streamFinished);
        a.start(Pa::waitForKey);
        return 0;
    }
    
Alternatively, you can pass the data as a shared_ptr, which will delete resources automatically:

    Osc* osc = new Osc(440);
    Pa a(paFunc, std::shared_ptr<Osc>(osc));

<br>

**Querying / setting audio devices:**

`listDevices()` prints a list of available input and output devices, sorted by device ID. 

To set an input or output device, enter a device ID with `setInputDevice(ID)` or `setOutputDevice(ID)` respectively. 

`getDeviceInfo(ID)` prints additional information about a device. 

---
### Reference:

**Constructors**

`Pa(callBack, userData)` &nbsp;

`Pa(callBack, inChannels, outChannels, SampleRate, FramesPerBuffer, userData)`

**Functions**

`void start()`

`void start(RunMode mode)`

`void start(unsigned long sleepTime)`
    
`void stop()`

`void stop(bool close)`

`void terminate()`

`void setSleepTime(unsigned long time)`

`void setSampleFormat(PaSampleFormat format)`

`void setFinishedCallBack(void(*callback)(void* data))`

`void listDevices()`

`void getDeviceInfo(unsigned int index)`

`void setInputDevice(unsigned int index)`

`void setOutputDevice(unsigned int index)`

**Typedefs**

`void miniCallBack(const float*, float*, unsigned long, void*)`

`int mainCallBack(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*)`

**Enums**

`RunMode {dontTerminate, wait, sleep, waitForKey}`
    
