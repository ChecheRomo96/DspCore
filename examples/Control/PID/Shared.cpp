#include "Shared.h"
#include <DspCore_Control.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Control { namespace PID {
bool Run(void (*print)(const char*)) {
    DspCore::Control::PID<float> pid;
    DspCore::Control::PidParameters<float> p;
    p.kp=2;p.ki=1;p.outputMin=0;p.outputMax=1;
    if(!pid.Configure(p)) return false;
    float output=0;
    for(unsigned i=0;i<100;++i) { if(!pid.Update(10.f,0.f,0.1f,output)) return false; }
    bool ok=output==1 && pid.Integral()==0;
    print("Saturated command=1; integral remains zero after 100 updates");
    ok=pid.Update(0.f,0.f,0.1f,output) && output==0 && ok;
    print("Setpoint returns to zero: output=0 immediately (no integral windup)");
    p.outputMax=2;
    if(!pid.Configure(p) || !pid.Reset()) return false;
    float measured=0;
    for(unsigned i=0;i<3000;++i) {
        if(!pid.Update(1.f,measured,0.01f,output)) return false;
        measured+=0.01f*(output-measured);
    }
    char text[100];
    snprintf(text,sizeof(text),"First-order plant after 30s: measurement=%d/1000; expected=1000/1000",static_cast<int>(measured*1000+0.5f)); print(text);
    return ok && fabsf(measured-1.f)<1e-3f;
}
}}}
