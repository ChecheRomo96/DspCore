#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace Shaping {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::Generators;
    float out[5]={}; char message[96];
    bool ok=DspCore::Generators::Shaping::AtanNorm(out,5,2.f);
    snprintf(message,sizeof(message),"Normalized atan endpoints: %d%%, %d%%; expected=-100%%,+100%%",static_cast<int>(out[0]*100),static_cast<int>(out[4]*100)); print(message);
    ok=ok && out[0]==-1 && out[4]==1;
    ok=DspCore::Generators::Shaping::TanhHalfNorm(out,5,2.f) && out[0]==0 && out[4]==1 && ok;
    print("Normalized positive tanh endpoints=0,1");
    ok=DspCore::Generators::Shaping::Gamma(out,5,2.f) && out[2]==0.25f && ok;
    snprintf(message,sizeof(message),"Gamma=2 at x=0.5: %d%%; expected=25%%",static_cast<int>(out[2]*100)); print(message);
    ok=DspCore::Generators::Shaping::GammaMorph(out,5,1.f,2.f) && out[2]==0.25f && ok;
    print("GammaMorph gain=2,morph=1 matches Gamma=2");
    return ok;
}
}}}
