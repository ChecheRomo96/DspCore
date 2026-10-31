#include "Shared.h"
#include <DspCore_Interpolation.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Interpolation { namespace Linear {
bool Run(void (*print)(const char*)) {
    const double value = DspCore::Interpolation::LinearAt(10.,20.,2.,8.,15.);
    bool ok = value == 5.;
    char message[96];
    snprintf(message,sizeof(message),"Physical x=15 in [10,20]: value=%d; expected=5",static_cast<int>(value));
    print(message);
    const double extrapolated = DspCore::Interpolation::NormalizedLinear(2.,8.,2.);
    snprintf(message,sizeof(message),"Normalized t=2: extrapolated=%d; expected=14",static_cast<int>(extrapolated));
    print(message); ok = ok && extrapolated == 14.;
    const double collapsed = DspCore::Interpolation::LinearAt(2.,2.,5.,9.,100.);
    print(collapsed == 5. ? "Collapsed axis selects value0: PASS" : "Collapsed axis: FAIL");
    return ok && collapsed == 5.;
}
}}}
