#include "Shared.h"
#include <DspCore_Filters.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Filters { namespace FIR {
bool Run(void (*print)(const char*)) {
    DspCore::Filters::FIR<float> filter;
    const float taps[]={0.25f,0.25f,0.25f,0.25f};
    if(!filter.Configure(taps,4)) return false;
    const float impulse[]={1.f,0.f,0.f,0.f,0.f};
    float output[5]={};
    DspCore::Processing::InputBlock<float> impulseInput(impulse,5);
    DspCore::Processing::OutputBlock<float> impulseOutput(output,5);
    if(!filter.Process(&impulseInput,1,&impulseOutput,1) || impulseOutput.size!=5) return false;
    char text[96]; bool ok=true;
    for(unsigned i=0;i<5;++i) {
        const int expected=i<4 ? 25 : 0;
        snprintf(text,sizeof(text),"Impulse[%u]=%d%%; expected=%d%%",i,static_cast<int>(output[i]*100),expected); print(text);
        ok=ok && output[i]==(i<4 ? 0.25f : 0.f);
    }
    filter.Reset();
    const float constant[]={1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f};
    float settled[8]={};
    DspCore::Processing::InputBlock<float> constantInput(constant,8);
    DspCore::Processing::OutputBlock<float> settledOutput(settled,8);
    if(!filter.Process(&constantInput,1,&settledOutput,1) || settledOutput.size!=8) return false;
    print("Constant input=1 settles exactly to 1 (unity DC gain)");
    return ok && settled[7]==1;
}
}}}
