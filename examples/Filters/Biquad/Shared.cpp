#include "Shared.h"
#include <DspCore_Filters.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Filters { namespace Biquad {
bool Run(void (*print)(const char*)) {
    auto coefficients=DspCore::Filters::Biquad::LowPass(48000.f,12000.f,0.70710678f);
    DspCore::Filters::IIR<float> filter;
    if(!filter.Configure(coefficients)) return false;
    const float impulse[]={1.f}; float firstOutput[1]={};
    if(!filter.Process(impulse,firstOutput,1)) return false;
    char text[96];
    bool ok=fabsf(firstOutput[0]-0.29289322f)<1e-5f;
    snprintf(text,sizeof(text),"Low-pass first impulse sample=%d/1000; expected=293/1000",static_cast<int>(firstOutput[0]*1000+0.5f)); print(text);
    const float constant[]={1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f};
    float output[16]={};
    for(unsigned i=0;i<8;++i) if(!filter.Process(constant,output,16)) return false;
    ok=ok && fabsf(output[15]-1.f)<1e-5f;
    print("Low-pass constant input=1 settles to 1");
    coefficients=DspCore::Filters::Biquad::HighPass(48000.f,12000.f,0.70710678f);
    if(!filter.Configure(coefficients)) return false;
    for(unsigned i=0;i<8;++i) if(!filter.Process(constant,output,16)) return false;
    print("High-pass constant input=1 settles to 0");
    return ok && fabsf(output[15])<1e-5f;
}
}}}
