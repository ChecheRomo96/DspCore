#include "../Shared.h"
#include <cstdio>
void PrintLine(const char* text) { std::puts(text); }
int main() {
    std::puts("DspCore :: Control / Smoothing");
    const bool ok=DspCoreExamples::Control::Smoothing::Run(PrintLine);
    std::puts(ok ? "Overall: PASS" : "Overall: FAIL");
    return ok ? 0 : 1;
}
