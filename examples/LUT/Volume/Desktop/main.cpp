#include "../Shared.h"
#include <cstdio>
void PrintLine(const char* message) { std::puts(message); }
int main() {
    std::puts("DspCore :: LUT / Volume");
    const bool ok = DspCoreExamples::LUT::Volume::Run(PrintLine);
    std::puts(ok ? "Overall: PASS" : "Overall: FAIL");
    return ok ? 0 : 1;
}
