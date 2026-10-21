#include "../../../Desktop/SignalCsv.h"
#include <DspCore_Filters.h>
#include <array>
#include <cmath>
#include <cstdio>

int main(int argc,char* argv[]) {
    std::filesystem::path outputDirectory;
    if(!DspCoreExamples::Desktop::ParseOutputDirectory(argc,argv,outputDirectory)) return 2;

    constexpr size_t sampleCount=8192;
    constexpr float sampleRate=48000.f;
    constexpr float pi=3.14159265358979323846f;
    std::array<float,sampleCount> input={};
    std::array<float,sampleCount> lowPass={};
    std::array<float,sampleCount> highPass={};
    float phase=0.f;
    for(size_t sample=0;sample<sampleCount;++sample) {
        const float position=static_cast<float>(sample)/static_cast<float>(sampleCount-1);
        const float frequency=100.f*std::pow(20000.f/100.f,position);
        phase+=2.f*pi*frequency/sampleRate;
        input[sample]=0.75f*std::sin(phase);
    }

    DspCore::Filters::IIR<float> lowPassFilter;
    DspCore::Filters::IIR<float> highPassFilter;
    if(!lowPassFilter.Configure(DspCore::Filters::Biquad::LowPass(sampleRate,12000.f,0.70710678f)) ||
       !highPassFilter.Configure(DspCore::Filters::Biquad::HighPass(sampleRate,12000.f,0.70710678f)) ||
       !lowPassFilter.Process(input.data(),lowPass.data(),lowPass.size()) ||
       !highPassFilter.Process(input.data(),highPass.data(),highPass.size())) return 1;
    const DspCoreExamples::Desktop::SignalColumn columns[]={
        {"input",input.data()}, {"lowpass_12khz",lowPass.data()}, {"highpass_12khz",highPass.data()}};
    std::filesystem::path outputPath;
    if(!DspCoreExamples::Desktop::WriteSignalCsv(outputDirectory,"biquad_sweep.csv",
        "Biquad low-pass and high-pass sweep","100 Hz to 20 kHz sine sweep; 12 kHz cutoff, Q=0.7071",
        "time_and_spectrum",sampleRate,columns,3,sampleCount,outputPath)) {
        std::fputs("Could not write CSV. Use --output <writable directory>.\n",stderr);
        return 1;
    }

    std::puts("DspCore :: Biquad 12 kHz low-pass / high-pass sweep");
    DspCoreExamples::Desktop::PrintPlotHint(outputPath,argv[0]);
    return 0;
}
