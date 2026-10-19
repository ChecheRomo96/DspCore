#include "Shared.h"
#include <DspCore_Control.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Control { namespace Hysteresis {
bool Run(void (*print)(const char*)) {
    DspCore::Control::Hysteresis<float> trigger;
    if(!trigger.Configure(0.25f,0.75f)) return false;
    const float input[]={0.5f,0.75f,0.5f,0.25f};
    const bool expected[]={false,true,true,false};
    bool ok=true,output=false; char text[100];
    for(unsigned i=0;i<4;++i) {
        ok=trigger.Update(input[i],output) && output==expected[i] && ok;
        snprintf(text,sizeof(text),"Input=%d%%: active=%u; expected=%u",static_cast<int>(input[i]*100),static_cast<unsigned>(output),static_cast<unsigned>(expected[i])); print(text);
    }
    return ok;
}
}}}
