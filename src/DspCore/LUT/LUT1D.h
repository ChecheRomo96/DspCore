#ifndef DSPCORE_LUT_LUT1D_H
#define DSPCORE_LUT_LUT1D_H
#include <CPtype_traits.h>
#include <CPutility.h>
#include <CPvector.h>

namespace DspCore { namespace LUT {
/// @ingroup DspCore_LUT
/// @brief Owning, move-only 1D arithmetic table. See @ref DspCore_LUT.
template<typename T>
class LUT1D {
private:
    static_assert(cpstd::is_arithmetic<T>::value &&
                  !cpstd::is_same<T, bool>::value &&
                  cpstd::is_same<T, typename cpstd::remove_cv<T>::type>::value,
                  "LUT storage requires a mutable arithmetic sample other than bool");

    cpstd::vector<T> samples_;
    cpstd::size_t width_;

public:
    /// Number of logical samples, excluding spare capacity.
    cpstd::size_t Size() const noexcept { return samples_.size(); }
    /// Number of allocated elements (not bytes).
    cpstd::size_t Capacity() const noexcept { return samples_.capacity(); }
    /// Whether there are no logical samples.
    bool Empty() const noexcept { return samples_.empty(); }
    /// Mutable contiguous storage; only Size() elements may be accessed.
    T* GetBuffer() noexcept { return samples_.data(); }
    /// Read-only contiguous storage.
    const T* GetBuffer() const noexcept { return samples_.data(); }
    /// Grow capacity without changing size; failure preserves all state.
    bool Reserve(cpstd::size_t count) noexcept {
        samples_.reserve(count);
        return samples_.capacity() >= count;
    }
    /// Set every logical sample without allocating.
    void Fill(T value) noexcept {
        for (cpstd::size_t index = 0U; index < samples_.size(); ++index) samples_[index] = value;
    }
    LUT1D() noexcept : width_(0) {}
    LUT1D(const LUT1D&) = delete;
    LUT1D& operator=(const LUT1D&) = delete;
    LUT1D(LUT1D&& other) noexcept
        : samples_(cpstd::move(other.samples_)), width_(other.width_) {
        other.width_ = 0;
    }
    LUT1D& operator=(LUT1D&& other) noexcept {
        if (this != &other) {
            samples_ = cpstd::move(other.samples_);
            width_ = other.width_;
            other.width_ = 0;
        }
        return *this;
    }
    /// Resize with a preserved flat prefix and zero-filled new elements.
    bool Resize(cpstd::size_t width) noexcept {
        if (width == 0) {
            samples_.resize(0U);
            width_ = 0;
            return true;
        }
        if (width > samples_.max_size()) return false;
        samples_.resize(width);
        if (samples_.size() != width) return false;
        width_ = width;
        return true;
    }
    /// Free capacity and reset all extents; safe to call repeatedly.
    void Release() noexcept {
        samples_ = cpstd::vector<T>();
        width_ = 0;
    }
    cpstd::size_t Width() const noexcept { return width_; }
    /// Return false without modifying value when coordinates are invalid.
    bool TryGet(cpstd::size_t x, T& value) const noexcept {
        if (x >= width_) return false;
        value = this->GetBuffer()[x];
        return true;
    }
    /// Return false without writing when coordinates are invalid.
    bool SetValue(cpstd::size_t x, T value) noexcept {
        if (x >= width_) return false;
        this->GetBuffer()[x] = value;
        return true;
    }
    /// Convenience read returning zero for invalid coordinates; use TryGet to distinguish.
    T GetValue(cpstd::size_t x) const noexcept {
        if (x >= width_) return T{};
        return this->GetBuffer()[x];
    }
};
}} // namespace DspCore::LUT
#endif

