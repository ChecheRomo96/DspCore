#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace LUTEnvelope {
bool Run(void (*print)(const char*)) {
    DspCore::LUT::LUT1D<float> shape;
    if(!shape.Resize(3)) return false;
    shape.SetValue(0,0); shape.SetValue(1,0.25f); shape.SetValue(2,1);
    float out[5]={}; char message[96];
    bool ok=DspCore::Generators::Envelopes::LUTSegment(out,0.f,1.f,5,shape);
    const int expected[]={0,125,250,625,1000};
    for(size_t i=0;i<5;++i) {
        const int actual=static_cast<int>(out[i]*1000+0.5f);
        snprintf(message,sizeof(message),"LUT envelope[%u]=%d/1000; expected=%d/1000",static_cast<unsigned>(i),actual,expected[i]); print(message);
        ok=ok && actual==expected[i];
    }
    if(!shape.Resize(1)) return false;
    shape.Fill(0.25f);
    ok=DspCore::Generators::Envelopes::LUTSegment(out,2.f,10.f,5,shape) && ok;
    for(float sample:out) ok=ok && sample==4;
    print("Singleton shape=0.25 maps [2,10] to 4 in all five output samples");
    return ok;
}
}}}
