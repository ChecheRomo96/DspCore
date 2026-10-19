#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace Envelopes {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::Generators;
    float out[5]={}; char message[96];
    bool ok=DspCore::Generators::Envelopes::ExpSegment(out,0.f,1.f,1e-9f,5) && out[2]==0.5f;
    snprintf(message,sizeof(message),"Exp curve=1e-9 midpoint: %d%%; expected=50%%",static_cast<int>(out[2]*100)); print(message);
    ok=DspCore::Generators::Envelopes::LogSegment(out,0.f,1.f,1e-9f,5) && out[2]==0.5f && ok;
    print("Log curve=1e-9 midpoint=50%; no NaN");
    ok=DspCore::Generators::Envelopes::ExpSegment(out,0.f,1.f,4.f,5) && fabsf(out[2]-0.8807971f)<1e-5f && ok;
    snprintf(message,sizeof(message),"Exp curve=4 midpoint: %d/1000; expected=881/1000",static_cast<int>(out[2]*1000+0.5f)); print(message);
    ok=DspCore::Generators::Envelopes::LogSegment(out,0.f,1.f,4.f,5) && fabsf(out[2]-0.6826062f)<1e-5f && ok;
    snprintf(message,sizeof(message),"Log curve=4 midpoint: %d/1000; expected=683/1000",static_cast<int>(out[2]*1000+0.5f)); print(message);
    return ok;
}
}}}
