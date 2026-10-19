#ifndef DSPCORE_LUT_LUT3D_H
#define DSPCORE_LUT_LUT3D_H
#include <DspCore/LUT/Detail/Storage.h>

namespace DspCore { namespace LUT {
/// @ingroup DspCore_LUT
/// @brief Owning, move-only 3D arithmetic table. See @ref DspCore_LUT.
template<typename T>
class LUT3D : private Detail::Storage<T> {
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
    LUT3D() noexcept : width_(0), height_(0), depth_(0) {}
    LUT3D(const LUT3D&) = delete;
    LUT3D& operator=(const LUT3D&) = delete;
    LUT3D(LUT3D&& other) noexcept
        : Base(static_cast<Base&&>(other)), width_(other.width_), height_(other.height_), depth_(other.depth_) {
        other.width_ = other.height_ = other.depth_ = 0;
    }
    LUT3D& operator=(LUT3D&& other) noexcept {
        if (this != &other) {
            Base::operator=(static_cast<Base&&>(other));
            width_ = other.width_;
            height_ = other.height_;
            depth_ = other.depth_;
            other.width_ = other.height_ = other.depth_ = 0;
        }
        return *this;
    }
    /// Resize with a preserved flat prefix and zero-filled new elements.
    bool Resize(size_t width, size_t height, size_t depth) noexcept {
        if (width == 0 || height == 0 || depth == 0) {
            Base::ResizeElements(0);
            width_ = height_ = depth_ = 0;
            return true;
        }
        size_t plane = 0, count = 0;
        if (!Base::Product(width, height, plane) || !Base::Product(plane, depth, count)) return false;
        if (!Base::ResizeElements(count)) return false;
        width_ = width;
        height_ = height;
        depth_ = depth;
        return true;
    }
    /// Free capacity and reset all extents; safe to call repeatedly.
    void Release() noexcept {
        Base::ReleaseStorage();
        width_ = height_ = depth_ = 0;
    }
    size_t Width() const noexcept { return width_; }
    size_t Height() const noexcept { return height_; }
    size_t Depth() const noexcept { return depth_; }
    /// Return false without modifying value when coordinates are invalid.
    bool TryGet(size_t x, size_t y, size_t z, T& value) const noexcept {
        if (x >= width_ || y >= height_ || z >= depth_) return false;
        value = this->GetBuffer()[(z * height_ + y) * width_ + x];
        return true;
    }
    /// Return false without writing when coordinates are invalid.
    bool SetValue(size_t x, size_t y, size_t z, T value) noexcept {
        if (x >= width_ || y >= height_ || z >= depth_) return false;
        this->GetBuffer()[(z * height_ + y) * width_ + x] = value;
        return true;
    }
    /// Convenience read returning zero for invalid coordinates; use TryGet to distinguish.
    T GetValue(size_t x, size_t y, size_t z) const noexcept {
        if (x >= width_ || y >= height_ || z >= depth_) return T{};
        return this->GetBuffer()[(z * height_ + y) * width_ + x];
    }
private:
    size_t width_;
    size_t height_;
    size_t depth_;
};
}} // namespace DspCore::LUT
#endif

