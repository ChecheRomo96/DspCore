#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace Windows {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::Generators;
    float out[5]={}; char message[96];
    bool ok=DspCore::Generators::Windows::Hann(out,5) && out[0]==0 && out[2]==1 && out[4]==0;
    for(size_t i=0;i<5;++i) {
        snprintf(message,sizeof(message),"Hann[%u]=%d%%",static_cast<unsigned>(i),static_cast<int>(out[i]*100+0.5f)); print(message);
    }
    print("Expected Hann: 0%,50%,100%,50%,0%");
    ok=DspCore::Generators::Windows::Hamming(out,5) && out[0]==0.08f && out[4]==0.08f && fabsf(out[2]-1)<1e-6f && ok;
    print("Hamming endpoints=8%; center=100%");
    ok=DspCore::Generators::Windows::Hann(out,1) && out[0]==1 && ok;
    print("Singleton window=1");
    return ok;
}
}}}
