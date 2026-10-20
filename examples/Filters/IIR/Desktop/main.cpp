#include "../../../Desktop/SignalCsv.h"
#include <DspCore_Filters.h>
#include <array>
#include <cstdio>

int main(int argc,char* argv[]) {
    std::filesystem::path outputDirectory;
    if(!DspCoreExamples::Desktop::ParseOutputDirectory(argc,argv,outputDirectory)) return 2;

    constexpr size_t sampleCount=4096;
    constexpr float sampleRate=48000.f;
    std::array<float,sampleCount> input={};
    std::array<float,sampleCount> output={};
    for(size_t sample=256;sample<2048;++sample) input[sample]=1.f;
    const float numerator[]={1.f,2.f,3.f,4.f};
    const float denominator[]={2.f,-1.f};

    DspCore::Filters::IIR<float> filter;
    if(!filter.Configure(numerator,denominator) ||
       !filter.Process(input.data(),output.data(),output.size())) return 1;
    const DspCoreExamples::Desktop::SignalColumn columns[]={
        {"input",input.data()}, {"generic_iir",output.data()}};
    std::filesystem::path outputPath;
    if(!DspCoreExamples::Desktop::WriteSignalCsv(outputDirectory,"iir_step_pulse.csv",
        "Generic IIR step-pulse response","4 numerator and 2 denominator coefficients; a0 normalized from 2",
        sampleRate,columns,2,sampleCount,outputPath)) {
        std::fputs("Could not write CSV. Use --output <writable directory>.\n",stderr);
        return 1;
    }

    std::puts("DspCore :: Generic IIR step-pulse response");
    DspCoreExamples::Desktop::PrintPlotHint(outputPath,argv[0]);
    return 0;
}
