#include "Shared.h"
#include <DspCore_Interpolation.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Interpolation { namespace Linear {
bool Run(void (*print)(const char*)) {
    double value = 0;
    bool ok = DspCore::Interpolation::TryLinear(10.,20.,2.,8.,15.,value);
    char message[96];
    snprintf(message,sizeof(message),"Physical x=15 in [10,20]: value=%d; expected=5",static_cast<int>(value));
    print(message); ok = ok && value == 5.;
    const double extrapolated = DspCore::Interpolation::Linear(2.,8.,2.);
    snprintf(message,sizeof(message),"Normalized t=2: extrapolated=%d; expected=14",static_cast<int>(extrapolated));
    print(message); ok = ok && extrapolated == 14.;
    const bool rejected = !DspCore::Interpolation::TryLinear(2.,2.,0.,1.,2.,value) && value == 5.;
    print(rejected ? "Collapsed bounds rejected; output preserved: PASS" : "Collapsed bounds: FAIL");
    return ok && rejected;
}
}}}
