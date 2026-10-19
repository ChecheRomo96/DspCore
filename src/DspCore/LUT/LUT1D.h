#ifndef DSPCORE_LUT_LUT1D_H
#define DSPCORE_LUT_LUT1D_H
#include <DspCore/LUT/Detail/Storage.h>

namespace DspCore { namespace LUT {
/// @ingroup DspCore_LUT
/// @brief Owning, move-only 1D arithmetic table. See @ref DspCore_LUT.
template<typename T>
class LUT1D : private Detail::Storage<T> {
    using Base = Detail::Storage<T>;
public:
    /// Number of logical samples, excluding spare capacity.
    size_t Size() const noexcept { return Base::Size(); }
    /// Number of allocated elements (not bytes).
    size_t Capacity() const noexcept { return Base::Capacity(); }
    /// Whether there are no logical samples.
    bool Empty() const noexcept { return Base::Empty(); }
    /// Mutable contiguous storage; only Size() elements may be accessed.
    T* GetBuffer() noexcept { return Base::GetBuffer(); }
    /// Read-only contiguous storage.
    const T* GetBuffer() const noexcept { return Base::GetBuffer(); }
    /// Grow capacity without changing size; failure preserves all state.
    bool Reserve(size_t count) noexcept { return Base::Reserve(count); }
    /// Set every logical sample without allocating.
    void Fill(T value) noexcept { Base::Fill(value); }
    LUT1D() noexcept : width_(0) {}
    LUT1D(const LUT1D&) = delete;
    LUT1D& operator=(const LUT1D&) = delete;
    LUT1D(LUT1D&& other) noexcept
        : Base(static_cast<Base&&>(other)), width_(other.width_) {
        other.width_ = 0;
    }
    LUT1D& operator=(LUT1D&& other) noexcept {
        if (this != &other) {
            Base::operator=(static_cast<Base&&>(other));
            width_ = other.width_;
            other.width_ = 0;
        }
        return *this;
    }
    /// Resize with a preserved flat prefix and zero-filled new elements.
    bool Resize(size_t width) noexcept {
        if (width == 0) {
            Base::ResizeElements(0);
            width_ = 0;
            return true;
        }
        const size_t count = width;
        if (!Base::ResizeElements(count)) return false;
        width_ = width;
        return true;
    }
    /// Free capacity and reset all extents; safe to call repeatedly.
    void Release() noexcept {
        Base::ReleaseStorage();
        width_ = 0;
    }
    size_t Width() const noexcept { return width_; }
    /// Return false without modifying value when coordinates are invalid.
    bool TryGet(size_t x, T& value) const noexcept {
        if (x >= width_) return false;
        value = this->GetBuffer()[x];
        return true;
    }
    /// Return false without writing when coordinates are invalid.
    bool SetValue(size_t x, T value) noexcept {
        if (x >= width_) return false;
        this->GetBuffer()[x] = value;
        return true;
    }
    /// Convenience read returning zero for invalid coordinates; use TryGet to distinguish.
    T GetValue(size_t x) const noexcept {
        if (x >= width_) return T{};
        return this->GetBuffer()[x];
    }
private:
    size_t width_;
};
}} // namespace DspCore::LUT
#endif

