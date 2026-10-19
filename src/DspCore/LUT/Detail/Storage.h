#ifndef DSPCORE_LUT_DETAIL_STORAGE_H
#define DSPCORE_LUT_DETAIL_STORAGE_H

#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <utility/CPSTL_allocator.h> // Portable placement new, including bare AVR.
#include <stddef.h>
#include <stdlib.h>

/// @cond DSPCORE_INTERNAL
namespace DspCore { namespace LUT { namespace Detail {

// Shared ownership mechanics; dimensions belong to the public table classes.
template<typename T>
class Storage {
    static_assert(cpstd::is_arithmetic<T>::value &&
                  !cpstd::is_same<T, bool>::value &&
                  cpstd::is_same<T, typename cpstd::remove_cv<T>::type>::value,
                  "LUT storage requires a mutable arithmetic sample other than bool");
public:
    Storage() noexcept : data_(nullptr), size_(0), capacity_(0) {}
    ~Storage() noexcept { free(data_); }
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage(Storage&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }
    Storage& operator=(Storage&& other) noexcept {
        if (this != &other) {
            free(data_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = other.capacity_ = 0;
        }
        return *this;
    }
    size_t Size() const noexcept { return size_; }
    size_t Capacity() const noexcept { return capacity_; }
    T* GetBuffer() noexcept { return data_; }
    const T* GetBuffer() const noexcept { return data_; }
    bool Empty() const noexcept { return size_ == 0; }
    /// Reserve element capacity, preserving logical size and values on failure.
    bool Reserve(size_t count) noexcept {
        if (count > static_cast<size_t>(-1) / sizeof(T)) return false;
        if (count <= capacity_) return true;
        T* replacement = static_cast<T*>(malloc(count * sizeof(T)));
        if (!replacement) return false;
        // Placement construction starts arithmetic object lifetimes in raw storage.
        for (size_t i = 0; i < count; ++i) new (replacement + i) T{};
        for (size_t i = 0; i < size_; ++i) replacement[i] = data_[i];
        free(data_);
        data_ = replacement;
        capacity_ = count;
        return true;
    }
    void Fill(T value) noexcept {
        for (size_t i = 0; i < size_; ++i) data_[i] = value;
    }
protected:
    bool ResizeElements(size_t count) noexcept {
        if (!Reserve(count)) return false;
        for (size_t i = size_; i < count; ++i) data_[i] = T{};
        size_ = count;
        return true;
    }
    void ReleaseStorage() noexcept {
        free(data_);
        data_ = nullptr;
        size_ = capacity_ = 0;
    }
    static bool Product(size_t a, size_t b, size_t& result) noexcept {
        if (b != 0 && a > static_cast<size_t>(-1) / b) return false;
        result = a * b;
        return true;
    }
private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

}}} // namespace DspCore::LUT::Detail
/// @endcond
#endif
