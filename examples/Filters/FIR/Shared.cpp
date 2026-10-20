#include "Shared.h"
#include <DspCore_Filters.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Filters { namespace FIR {
bool Run(void (*print)(const char*)) {
    DspCore::Filters::FIR<float> filter;
    const float taps[]={0.25f,0.25f,0.25f,0.25f};
    if(!filter.Configure(taps,4)) return false;
    float output=0; char text[96]; bool ok=true;
    for(unsigned i=0;i<5;++i) {
        ok=filter.Process(i==0 ? 1.f : 0.f,output) && ok;
        const int expected=i<4 ? 25 : 0;
        snprintf(text,sizeof(text),"Impulse[%u]=%d%%; expected=%d%%",i,static_cast<int>(output*100),expected); print(text);
        ok=ok && output==(i<4 ? 0.25f : 0.f);
    }
    filter.Reset();
    for(unsigned i=0;i<8;++i) { if(!filter.Process(1.f,output)) return false; }
    print("Constant input=1 settles exactly to 1 (unity DC gain)");
    return ok && output==1;
}
}}}
