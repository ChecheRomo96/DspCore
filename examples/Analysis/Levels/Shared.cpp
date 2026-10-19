#include "Shared.h"
#include <DspCore_Analysis.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Analysis { namespace Levels {
bool Run(void (*print)(const char*)) {
    const float input[]={-3,4,0,0};
    float peak=0,rms=0; char text[96];
    bool ok=DspCore::Analysis::Peak(input,4,peak) && DspCore::Analysis::RMS(input,4,rms);
    snprintf(text,sizeof(text),"Input [-3,4,0,0]: peak=%d; expected=4",static_cast<int>(peak)); print(text);
    snprintf(text,sizeof(text),"RMS=%d/10; expected=25/10",static_cast<int>(rms*10)); print(text);
    ok=ok && peak==4 && rms==2.5f;
    const float maximum=cpstd::numeric_limits<float>::max();
    const float large[]={maximum,-maximum};
    const bool scaled=DspCore::Analysis::RMS(large,2,rms) && rms==maximum;
    print(scaled ? "Maximum finite input: scaled RMS stays finite: PASS" : "Scaled RMS: FAIL");
    return ok && scaled;
}
}}}
