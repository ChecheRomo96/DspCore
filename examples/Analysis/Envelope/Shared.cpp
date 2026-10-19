#include "Shared.h"
#include <DspCore_Analysis.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Analysis { namespace Envelope {
bool Run(void (*print)(const char*)) {
    DspCore::Analysis::EnvelopeFollower<float> follower;
    if(!follower.Configure(1.f,2.f)) return false;
    float output=0; char text[96];
    bool ok=follower.Update(-1.f,1.f,output) && fabsf(output-0.63212056f)<1e-6f;
    snprintf(text,sizeof(text),"Attack=1s after 1s: %d/1000; expected=632/1000",static_cast<int>(output*1000+0.5f)); print(text);
    ok=follower.Update(0.f,2.f,output) && fabsf(output-0.23254416f)<1e-6f && ok;
    snprintf(text,sizeof(text),"Release=2s after 2s: %d/1000; expected=233/1000",static_cast<int>(output*1000+0.5f)); print(text);
    ok=follower.Configure(0.f,0.f) && follower.Update(-2.f,0.01f,output) && output==2 && ok;
    print("Zero time constants rectify immediately: input=-2 -> envelope=2");
    return ok;
}
}}}
