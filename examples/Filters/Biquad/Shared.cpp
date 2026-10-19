#include "Shared.h"
#include <DspCore_Filters.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Filters { namespace Biquad {
bool Run(void (*print)(const char*)) {
    auto coefficients=DspCore::Filters::Biquad::LowPass(48000.f,12000.f,0.70710678f);
    DspCore::Filters::IIR<float> filter;
    if(!filter.Configure(coefficients)) return false;
    float output=0; char text[96];
    bool ok=filter.Process(1.f,output) && fabsf(output-0.29289322f)<1e-5f;
    snprintf(text,sizeof(text),"Low-pass first impulse sample=%d/1000; expected=293/1000",static_cast<int>(output*1000+0.5f)); print(text);
    for(unsigned i=0;i<128;++i) { if(!filter.Process(1.f,output)) return false; }
    ok=ok && fabsf(output-1.f)<1e-5f;
    print("Low-pass constant input=1 settles to 1");
    coefficients=DspCore::Filters::Biquad::HighPass(48000.f,12000.f,0.70710678f);
    if(!filter.Configure(coefficients)) return false;
    for(unsigned i=0;i<128;++i) { if(!filter.Process(1.f,output)) return false; }
    print("High-pass constant input=1 settles to 0");
    return ok && fabsf(output)<1e-5f;
}
}}}
