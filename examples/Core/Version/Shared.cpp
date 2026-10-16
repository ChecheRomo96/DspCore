#include "Shared.h"

#include <DspCore/Core/Version.h>

namespace DspCoreExamples::Core::Version {

const char* DspCoreVersion() noexcept {
    return DspCore::Core::Version();
}

const char* FoundationVersion() noexcept {
    return DspCore::Core::FoundationVersion();
}

} // namespace DspCoreExamples::Core::Version
