#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace Periodic {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::Generators;
    float out[4]={}; char message[96];
    bool ok=DspCore::Generators::Periodic::Sine(out,4);
    const float sine[]={0,1,0,-1};
    for(size_t i=0;i<4;++i) {
        snprintf(message,sizeof(message),"Sine[%u]=%d%%; expected=%d%%",static_cast<unsigned>(i),static_cast<int>(out[i]*100),static_cast<int>(sine[i]*100)); print(message);
        ok=ok && fabsf(out[i]-sine[i])<1e-5f;
    }
    ok=DspCore::Generators::Periodic::Saw(out,4) && out[0]==0 && out[2]==-1 && ok;
    print("Saw phase 0 starts at zero; half-cycle=-1");
    ok=DspCore::Generators::Periodic::Square(out,4) && out[0]==1 && out[2]==-1 && ok;
    print("Square phase 0=+1; half-cycle=-1");
    ok=DspCore::Generators::Periodic::Triangle(out,4) && out[0]==0 && out[1]==1 && out[3]==-1 && ok;
    print("Triangle quarter-cycle=+1; three-quarter-cycle=-1");
    return ok;
}
}}}
