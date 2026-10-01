#include <Foundation/Math/Arithmetic.h>
#include <DspCore.h>

#include <cstring>
#include <iostream>

#ifndef DSPCORE_CORE
    #error "The installed DspCore package must export DSPCORE_CORE"
#endif

int main() {
    // The installed package must bring its transitive Foundation target with
    // it.
    const auto gcd = Foundation::Math::GCD(12, 8);

    std::cout << "DspCore: " << DspCore::Core::Version() << '\n';
    std::cout << "Foundation: " << DspCore::Core::FoundationVersion() << '\n';
    std::cout << "Foundation::Math::GCD(12, 8): " << gcd << '\n';

    return (gcd == 4U &&
            std::strcmp(DspCore::Core::Version(), DSPCORE_VERSION) == 0) ? 0 : 1;
}
