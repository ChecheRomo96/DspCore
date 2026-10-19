#include "../Shared.h"
#include <cstdio>
void PrintLine(const char* text) { std::puts(text); }
int main() {
    std::puts("DspCore :: Generators / Windows");
    const bool ok=DspCoreExamples::Generators::Windows::Run(PrintLine);
    std::puts(ok ? "Overall: PASS" : "Overall: FAIL");
    return ok ? 0 : 1;
}
