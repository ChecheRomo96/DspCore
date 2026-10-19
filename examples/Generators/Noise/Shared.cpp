#include "Shared.h"
#include <DspCore_Generators.h>
#include <stdio.h>
namespace DspCoreExamples { namespace Generators { namespace Noise {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::Generators;
    float whole[8]={},blocks[8]={};
    uint32_t first=0x12345678u,second=first;
    bool ok=DspCore::Generators::Noise::White(whole,8,first) && DspCore::Generators::Noise::White(blocks,3,second) && DspCore::Generators::Noise::White(blocks+3,5,second);
    for(size_t i=0;i<8;++i) ok=ok && whole[i]==blocks[i] && whole[i]>=-1 && whole[i]<=1;
    ok=ok && first==second;
    print(ok ? "One 8-sample block equals split 3+5 blocks: PASS" : "Block continuity: FAIL");
    uint32_t zero=0; float sentinel=99;
    const bool rejected=!DspCore::Generators::Noise::White(&sentinel,1,zero) && sentinel==99;
    print(rejected ? "Zero seed rejected without changing output: PASS" : "Zero seed: FAIL");
    char message[80];
    snprintf(message,sizeof(message),"Continued PRNG state=%lu",static_cast<unsigned long>(first)); print(message);
    return ok && rejected;
}
}}}
