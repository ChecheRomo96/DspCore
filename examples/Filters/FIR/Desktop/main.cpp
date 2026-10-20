#include "../../../Desktop/SignalCsv.h"
#include <DspCore_Filters.h>
#include <array>
#include <cmath>
#include <cstdio>

int main(int argc,char* argv[]) {
    std::filesystem::path outputDirectory;
    if(!DspCoreExamples::Desktop::ParseOutputDirectory(argc,argv,outputDirectory)) return 2;

    constexpr size_t sampleCount=4096;
    constexpr float sampleRate=48000.f;
    constexpr float pi=3.14159265358979323846f;
    std::array<float,33> taps={};
    for(float& tap:taps) tap=1.f/static_cast<float>(taps.size());
    std::array<float,sampleCount> input={};
    std::array<float,sampleCount> output={};
    for(size_t sample=0;sample<sampleCount;++sample) {
        const float time=static_cast<float>(sample)/sampleRate;
        input[sample]=0.6f*std::sin(2.f*pi*300.f*time)+0.3f*std::sin(2.f*pi*6000.f*time);
    }

    DspCore::Filters::FIR<float> filter;
    if(!filter.Configure(taps.data(),taps.size()) ||
       !filter.Process(input.data(),output.data(),output.size())) return 1;
    const DspCoreExamples::Desktop::SignalColumn columns[]={
        {"input",input.data()}, {"moving_average_33",output.data()}};
    std::filesystem::path outputPath;
    if(!DspCoreExamples::Desktop::WriteSignalCsv(outputDirectory,"fir_moving_average.csv",
        "FIR moving-average low-pass","300 Hz + 6 kHz input; 33-tap moving average",sampleRate,
        columns,2,sampleCount,outputPath)) {
        std::fputs("Could not write CSV. Use --output <writable directory>.\n",stderr);
        return 1;
    }

    std::puts("DspCore :: FIR moving-average low-pass");
    DspCoreExamples::Desktop::PrintPlotHint(outputPath,argv[0]);
    return 0;
}
