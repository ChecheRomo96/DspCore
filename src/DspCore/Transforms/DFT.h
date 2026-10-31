#ifndef DSPCORE_TRANSFORMS_DFT_H
#define DSPCORE_TRANSFORMS_DFT_H

#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/SampleBlock.h>
#include <Foundation/Math/Complex.h>
#include <CPtype_traits.h>
#include <CPvector.h>

namespace DspCore { namespace Transforms {

/**
 * @brief Configured direct discrete Fourier transform with forward and inverse operations.
 * @ingroup DspCore_Transforms
 *
 * Configure allocates one caller-invisible workspace. Forward and Inverse
 * then perform no allocation, accept equal-sized configured blocks and permit
 * exact in-place operation. Forward is unnormalised; Inverse divides by the
 * configured size, so `Inverse(Forward(x))` reconstructs @p x subject to
 * floating-point rounding.
 */
template<typename Real=float> class DFT {
public:
    using Complex=Foundation::Math::Complex<Real>;
    static_assert(cpstd::is_floating_point<Real>::value,
        "DFT requires float or double precision");

    bool Configure(const cpstd::size_t size) noexcept {
        if(size==0U) return false;
        workspace_.resize(size);
        if(workspace_.size()!=size) { size_=0U; return false; }
        size_=size;
        return true;
    }

    cpstd::size_t Size() const noexcept { return size_; }
    bool IsConfigured() const noexcept { return size_!=0U; }

    bool Forward(const Processing::SampleBlock<const Complex> input,
        const Processing::SampleBlock<Complex> output) noexcept {
        return Transform(input,output,false);
    }

    bool ForwardReal(const Processing::SampleBlock<const Real> input,
        const Processing::SampleBlock<Complex> output) noexcept {
        if(!Valid(input,output)) return false;
        for(cpstd::size_t index=0U;index<size_;++index) if(!Detail::Signal::Finite(input.data[index])) return false;
        for(cpstd::size_t bin=0U;bin<size_;++bin) {
            Real real=Real(0),imag=Real(0);
            for(cpstd::size_t sample=0U;sample<size_;++sample) {
                const Real angle=-Real(2)*Detail::Signal::Pi<Real>()*
                    static_cast<Real>(bin)*static_cast<Real>(sample)/static_cast<Real>(size_);
                real+=input.data[sample]*Detail::Signal::Cos(angle);
                imag+=input.data[sample]*Detail::Signal::Sin(angle);
                if(!Detail::Signal::Finite(real) || !Detail::Signal::Finite(imag)) return false;
            }
            workspace_[bin].real=real;
            workspace_[bin].imag=imag;
        }
        for(cpstd::size_t index=0U;index<size_;++index) output.data[index]=workspace_[index];
        return true;
    }

    bool Inverse(const Processing::SampleBlock<const Complex> input,
        const Processing::SampleBlock<Complex> output) noexcept {
        return Transform(input,output,true);
    }

private:
    bool Valid(const Processing::SampleBlock<const Complex> input,
        const Processing::SampleBlock<Complex> output) const noexcept {
        return size_!=0U && input.IsValid() && output.IsValid() &&
            input.size==size_ && output.size==size_;
    }

    bool Valid(const Processing::SampleBlock<const Real> input,
        const Processing::SampleBlock<Complex> output) const noexcept {
        return size_!=0U && input.IsValid() && output.IsValid() &&
            input.size==size_ && output.size==size_;
    }

    static bool Finite(const Complex& value) noexcept {
        return Detail::Signal::Finite(value.real) && Detail::Signal::Finite(value.imag);
    }

    bool Transform(const Processing::SampleBlock<const Complex> input,
        const Processing::SampleBlock<Complex> output,const bool inverse) noexcept {
        if(!Valid(input,output)) return false;
        for(cpstd::size_t index=0U;index<size_;++index) if(!Finite(input.data[index])) return false;
        const Real direction=inverse ? Real(1) : Real(-1);
        const Real scale=inverse ? Real(1)/static_cast<Real>(size_) : Real(1);
        for(cpstd::size_t bin=0U;bin<size_;++bin) {
            Real real=Real(0),imag=Real(0);
            for(cpstd::size_t sample=0U;sample<size_;++sample) {
                const Real angle=direction*Real(2)*Detail::Signal::Pi<Real>()*
                    static_cast<Real>(bin)*static_cast<Real>(sample)/static_cast<Real>(size_);
                const Real cosine=Detail::Signal::Cos(angle);
                const Real sine=Detail::Signal::Sin(angle);
                const Complex& value=input.data[sample];
                real+=value.real*cosine-value.imag*sine;
                imag+=value.real*sine+value.imag*cosine;
                if(!Detail::Signal::Finite(real) || !Detail::Signal::Finite(imag)) return false;
            }
            workspace_[bin].real=real*scale;
            workspace_[bin].imag=imag*scale;
            if(!Finite(workspace_[bin])) return false;
        }
        for(cpstd::size_t index=0U;index<size_;++index) output.data[index]=workspace_[index];
        return true;
    }

    cpstd::vector<Complex> workspace_;
    cpstd::size_t size_{};
};

}} // namespace DspCore::Transforms

#endif // DSPCORE_TRANSFORMS_DFT_H
