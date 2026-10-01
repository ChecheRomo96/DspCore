#include "../Shared.h"

#include <iostream>

int main() {
    std::cout << "========================================\n"
              << " DspCore :: Core / Version\n"
              << "========================================\n"
              << " DspCore ....... "
              << DspCoreExamples::Core::Version::DspCoreVersion() << '\n'
              << " Foundation .... "
              << DspCoreExamples::Core::Version::FoundationVersion() << '\n'
              << "========================================\n";
    return 0;
}
