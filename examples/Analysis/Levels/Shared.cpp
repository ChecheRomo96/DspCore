#include "Shared.h"
#include <DspCore_Analysis.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Analysis { namespace Levels {
bool Run(void (*print)(const char*)) {
    const float input[]={-3,4,0,0};
    float peak=0,rms=0; char text[96];
    const auto block=DspCore::Processing::MakeInputBlock(input,4U);
    bool ok=DspCore::Analysis::Peak(block,peak) && DspCore::Analysis::RMS(block,rms);
    snprintf(text,sizeof(text),"Input [-3,4,0,0]: peak=%d; expected=4",static_cast<int>(peak)); print(text);
    snprintf(text,sizeof(text),"RMS=%d/10; expected=25/10",static_cast<int>(rms*10)); print(text);
    ok=ok && peak==4 && rms==2.5f;
    const float maximum=cpstd::numeric_limits<float>::max();
    const float large[]={maximum,-maximum};
    const bool scaled=DspCore::Analysis::RMS(DspCore::Processing::MakeInputBlock(large,2U),rms) && rms==maximum;
    print(scaled ? "Maximum finite input: scaled RMS stays finite: PASS" : "Scaled RMS: FAIL");
    return ok && scaled;
}
}}}
