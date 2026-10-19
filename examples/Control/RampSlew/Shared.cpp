#include "Shared.h"
#include <DspCore_Control.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Control { namespace RampSlew {
bool Run(void (*print)(const char*)) {
    DspCore::Control::LinearRamp<float> ramp;
    DspCore::Control::SlewLimiter<float> slew;
    float out=0; char text[100];
    bool ok=ramp.SetTarget(10.f,2.f) && ramp.Update(0.5f,out) && out==2.5f;
    snprintf(text,sizeof(text),"Ramp 0->10 over 2s, after 0.5s: %d/10; expected=25/10",static_cast<int>(out*10)); print(text);
    ok=ramp.Update(2.f,out) && out==10 && ramp.Remaining()==0 && ok;
    print("Oversized final interval arrives exactly at target=10");
    ok=slew.Configure(2.f,4.f) && slew.Update(10.f,0.5f,out) && out==1 && ok;
    snprintf(text,sizeof(text),"Rise rate=2/s, dt=0.5s: output=%d; expected=1",static_cast<int>(out)); print(text);
    ok=slew.Update(-10.f,0.5f,out) && out==-1 && ok;
    print("Fall rate=4/s, next 0.5s: output=-1");
    return ok;
}
}}}
