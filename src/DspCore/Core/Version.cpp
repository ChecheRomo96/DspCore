#include <DspCore/Core/Version.h>

#include <Foundation_BuildSettings.h>
#include <DspCore_BuildSettings.h>

namespace DspCore::Core {

const char* Version() noexcept {
    return DSPCORE_VERSION;
}

const char* FoundationVersion() noexcept {
    return FOUNDATION_VERSION;
}

} // namespace DspCore::Core
