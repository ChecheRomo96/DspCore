#include "../Shared.h"
#include <cstdio>
void PrintLine(const char* message) { std::puts(message); }
int main() {
    std::puts("DspCore :: Interpolation / Lookup");
    const bool ok = DspCoreExamples::Interpolation::Lookup::Run(PrintLine);
    std::puts(ok ? "Overall: PASS" : "Overall: FAIL");
    return ok ? 0 : 1;
}
