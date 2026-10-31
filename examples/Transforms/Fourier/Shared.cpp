#include "Shared.h"
#include <DspCore_Transforms.h>
#include <stdio.h>

namespace DspCoreExamples { namespace Transforms { namespace Fourier {
bool Run(void (*print)(const char*)) {
    const float input[]={1.f,-1.f,1.f,-1.f};
    Foundation::Math::Complex<float> dftBins[4]{};
    Foundation::Math::Complex<float> fftBins[4]{};
    Foundation::Math::Complex<float> restored[4]{};
    DspCore::Transforms::DFT<float> dft;
    DspCore::Transforms::FFT<float> fft;
    if(!dft.Configure(4U) || !fft.Configure(4U)) return false;
    const auto samples=DspCore::Processing::MakeInputBlock(input,4U);
    bool ok=dft.ForwardReal(samples,DspCore::Processing::MakeOutputBlock(dftBins,4U));
    ok=fft.ForwardReal(samples,DspCore::Processing::MakeOutputBlock(fftBins,4U)) && ok;
    ok=fft.Inverse(DspCore::Processing::MakeInputBlock(fftBins,4U),
        DspCore::Processing::MakeOutputBlock(restored,4U)) && ok;
    char text[96];
    snprintf(text,sizeof(text),"Nyquist bin: DFT=%d, FFT=%d; expected=4",static_cast<int>(dftBins[2].real),static_cast<int>(fftBins[2].real));
    print(text);
    snprintf(text,sizeof(text),"FFT inverse first sample=%d; expected=1",static_cast<int>(restored[0].real));
    print(text);
    return ok && dftBins[2].real==4.f && fftBins[2].real==4.f && restored[0].real==1.f;
}
}}}
