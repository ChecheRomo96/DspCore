#include "../Shared.h"
#include <cstdio>
void PrintLine(const char* message) { std::puts(message); }
int main() {
    std::puts("DspCore :: LUT / Surface");
    const bool ok = DspCoreExamples::LUT::Surface::Run(PrintLine);
    std::puts(ok ? "Overall: PASS" : "Overall: FAIL");
    return ok ? 0 : 1;
}
