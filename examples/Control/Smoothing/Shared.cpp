#include "Shared.h"
#include <DspCore_Control.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Control { namespace Smoothing {
bool Run(void (*print)(const char*)) {
    DspCore::Control::Smoother<float> smoother;
    if(!smoother.Configure(1.f)) return false;
    float out=0; char text[100];
    bool ok=smoother.Update(1.f,1.f,out);
    snprintf(text,sizeof(text),"tau=1s, dt=1s: output=%d/1000; expected=632/1000",static_cast<int>(out*1000+0.5f)); print(text);
    ok=ok && fabsf(out-0.63212056f)<1e-6f;
    const bool rejected=!smoother.Update(1.f,0.f,out);
    print(rejected ? "Zero interval rejected without changing state: PASS" : "Interval validation: FAIL");
    return ok && rejected;
}
}}}
