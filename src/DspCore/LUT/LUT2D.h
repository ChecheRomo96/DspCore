#ifndef DSPCORE_LUT_LUT2D_H
#define DSPCORE_LUT_LUT2D_H
#include <CPtype_traits.h>
#include <CPutility.h>
#include <CPvector.h>

namespace DspCore { namespace LUT {
/// @ingroup DspCore_LUT
/// @brief Owning, move-only 2D arithmetic table. See @ref DspCore_LUT.
template<typename T>
class LUT2D {
private:
    static_assert(cpstd::is_arithmetic<T>::value &&
                  !cpstd::is_same<T, bool>::value &&
                  cpstd::is_same<T, typename cpstd::remove_cv<T>::type>::value,
                  "LUT storage requires a mutable arithmetic sample other than bool");

    cpstd::vector<T> samples_;
    cpstd::size_t width_;
    cpstd::size_t height_;

    static bool Product(cpstd::size_t left, cpstd::size_t right, cpstd::size_t& result) noexcept {
        if (right != 0U && left > static_cast<cpstd::size_t>(-1) / right) return false;
        result = left * right;
        return true;
    }

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
    LUT2D() noexcept : width_(0), height_(0) {}
    LUT2D(const LUT2D&) = delete;
    LUT2D& operator=(const LUT2D&) = delete;
    LUT2D(LUT2D&& other) noexcept
        : samples_(cpstd::move(other.samples_)), width_(other.width_), height_(other.height_) {
        other.width_ = other.height_ = 0;
    }
    LUT2D& operator=(LUT2D&& other) noexcept {
        if (this != &other) {
            samples_ = cpstd::move(other.samples_);
            width_ = other.width_;
            height_ = other.height_;
            other.width_ = other.height_ = 0;
        }
        return *this;
    }
    /// Resize with a preserved flat prefix and zero-filled new elements.
    bool Resize(cpstd::size_t width, cpstd::size_t height) noexcept {
        if (width == 0 || height == 0) {
            samples_.resize(0U);
            width_ = height_ = 0;
            return true;
        }
        cpstd::size_t count = 0U;
        if (!Product(width, height, count) || count > samples_.max_size()) return false;
        samples_.resize(count);
        if (samples_.size() != count) return false;
        width_ = width;
        height_ = height;
        return true;
    }
    /// Free capacity and reset all extents; safe to call repeatedly.
    void Release() noexcept {
        samples_ = cpstd::vector<T>();
        width_ = height_ = 0;
    }
    cpstd::size_t Width() const noexcept { return width_; }
    cpstd::size_t Height() const noexcept { return height_; }
    /// Return false without modifying value when coordinates are invalid.
    bool TryGet(cpstd::size_t x, cpstd::size_t y, T& value) const noexcept {
        if (x >= width_ || y >= height_) return false;
        value = this->GetBuffer()[y * width_ + x];
        return true;
    }
    /// Return false without writing when coordinates are invalid.
    bool SetValue(cpstd::size_t x, cpstd::size_t y, T value) noexcept {
        if (x >= width_ || y >= height_) return false;
        this->GetBuffer()[y * width_ + x] = value;
        return true;
    }
    /// Convenience read returning zero for invalid coordinates; use TryGet to distinguish.
    T GetValue(cpstd::size_t x, cpstd::size_t y) const noexcept {
        if (x >= width_ || y >= height_) return T{};
        return this->GetBuffer()[y * width_ + x];
    }
};
}} // namespace DspCore::LUT
#endif

