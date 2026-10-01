#ifndef DSPCORE_CORE_VERSION_H
#define DSPCORE_CORE_VERSION_H

namespace DspCore::Core {

/** @brief Returns the DspCore semantic version compiled into the library. */
const char* Version() noexcept;

/** @brief Returns the Foundation semantic version used to build DspCore. */
const char* FoundationVersion() noexcept;

} // namespace DspCore::Core

#endif // DSPCORE_CORE_VERSION_H
