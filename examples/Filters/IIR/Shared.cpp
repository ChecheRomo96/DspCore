#include "Shared.h"
#include <DspCore/Filters/IIR.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Filters { namespace IIR {
bool Run(void (*print)(const char*)) {
    DspCore::Filters::IIR<float,4,2> filter;
    const float numerator[]={1,2,3,4},denominator[]={2,-1};
    if(!filter.Configure(numerator,denominator)) return false;
    const float expected[]={0.5f,1.25f,2.125f,3.0625f,1.53125f};
    bool ok=true;float output=0;char text[100];
    print("Generic IIR: 4 numerator, 2 denominator coefficients; a0=2 normalized internally");
    for(unsigned i=0;i<5;++i) {
        if(!filter.Process(i==0 ? 1.f : 0.f,output)) return false;
        snprintf(text,sizeof(text),"Impulse[%u]=%d/32; expected=%d/32",i,static_cast<int>(output*32),static_cast<int>(expected[i]*32));
        print(text);ok=ok && output==expected[i];
    }
    return ok;
}
}}}
