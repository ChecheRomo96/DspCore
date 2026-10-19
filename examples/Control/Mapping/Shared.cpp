#include "Shared.h"
#include <DspCore_Control.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Control { namespace Mapping {
bool Run(void (*print)(const char*)) {
    float out=0; char text[100];
    bool ok=DspCore::Control::Map(0.25f,0.f,1.f,100.f,0.f,out,DspCore::Control::MappingBoundary::Clamp) && out==75;
    snprintf(text,sizeof(text),"Input 0.25 mapped from [0,1] to [100,0]: %d; expected=75",static_cast<int>(out)); print(text);
    const bool rejected=!DspCore::Control::Map(2.f,0.f,1.f,0.f,100.f,out,DspCore::Control::MappingBoundary::Reject) && out==75;
    print(rejected ? "Out-of-domain input rejected; output preserved: PASS" : "Boundary policy: FAIL");
    ok=DspCore::Control::Deadband(0.5f,0.2f,out) && fabsf(out-0.3f)<1e-6f && ok;
    print("Deadband width=0.2: input=0.5 -> output=0.3");
    return ok && rejected;
}
}}}
