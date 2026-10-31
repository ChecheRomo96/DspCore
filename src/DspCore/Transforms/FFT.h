#ifndef DSPCORE_TRANSFORMS_FFT_H
#define DSPCORE_TRANSFORMS_FFT_H

#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/SampleBlock.h>
#include <Foundation/Math/Complex.h>
#include <CPtype_traits.h>
#include <CPvector.h>

namespace DspCore { namespace Transforms {

/**
 * @brief Configured radix-2 Cooley-Tukey FFT with forward and inverse operations.
 * @ingroup DspCore_Transforms
 *
 * Configure accepts only nonzero powers of two and allocates one workspace.
 * Forward and Inverse are allocation-free thereafter and support identical
 * input/output storage. Forward is unnormalised; Inverse applies `1/N`.
 */
template<typename Real=float> class FFT {
public:
    using Complex=Foundation::Math::Complex<Real>;
    static_assert(cpstd::is_floating_point<Real>::value,
        "FFT requires float or double precision");

    bool Configure(const cpstd::size_t size) noexcept {
        if(!PowerOfTwo(size)) return false;
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
        for(cpstd::size_t index=0U;index<size_;++index) {
            if(!Detail::Signal::Finite(input.data[index])) return false;
            workspace_[index].real=input.data[index];
            workspace_[index].imag=Real(0);
        }
        return Transform(Processing::SampleBlock<const Complex>{workspace_.data(),size_},output,false);
    }

    bool Inverse(const Processing::SampleBlock<const Complex> input,
        const Processing::SampleBlock<Complex> output) noexcept {
        return Transform(input,output,true);
    }

private:
    static bool PowerOfTwo(const cpstd::size_t value) noexcept {
        return value!=0U && (value&(value-1U))==0U;
    }

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
        for(cpstd::size_t index=0U;index<size_;++index) {
            if(!Finite(input.data[index])) return false;
            workspace_[index]=input.data[index];
        }
        BitReverse();
        const Real direction=inverse ? Real(1) : Real(-1);
        for(cpstd::size_t length=2U;;length<<=1U) {
            const Real angle=direction*Real(2)*Detail::Signal::Pi<Real>()/static_cast<Real>(length);
            Complex step;
            step.real=Detail::Signal::Cos(angle);
            step.imag=Detail::Signal::Sin(angle);
            const cpstd::size_t half=length/2U;
            for(cpstd::size_t start=0U;start<size_;start+=length) {
                Complex twiddle;
                twiddle.real=Real(1); twiddle.imag=Real(0);
                for(cpstd::size_t offset=0U;offset<half;++offset) {
                    const Complex upper=workspace_[start+offset];
                    const Complex lower=workspace_[start+offset+half];
                    Complex product;
                    product.real=lower.real*twiddle.real-lower.imag*twiddle.imag;
                    product.imag=lower.real*twiddle.imag+lower.imag*twiddle.real;
                    workspace_[start+offset].real=upper.real+product.real;
                    workspace_[start+offset].imag=upper.imag+product.imag;
                    workspace_[start+offset+half].real=upper.real-product.real;
                    workspace_[start+offset+half].imag=upper.imag-product.imag;
                    if(!Finite(workspace_[start+offset]) || !Finite(workspace_[start+offset+half])) return false;
                    const Real nextReal=twiddle.real*step.real-twiddle.imag*step.imag;
                    twiddle.imag=twiddle.real*step.imag+twiddle.imag*step.real;
                    twiddle.real=nextReal;
                    if(!Finite(twiddle)) return false;
                }
            }
            if(length==size_) break;
        }
        const Real scale=inverse ? Real(1)/static_cast<Real>(size_) : Real(1);
        for(cpstd::size_t index=0U;index<size_;++index) {
            workspace_[index].real*=scale;
            workspace_[index].imag*=scale;
            if(!Finite(workspace_[index])) return false;
        }
        for(cpstd::size_t index=0U;index<size_;++index) output.data[index]=workspace_[index];
        return true;
    }

    void BitReverse() noexcept {
        cpstd::size_t reversed=0U;
        for(cpstd::size_t index=1U;index<size_;++index) {
            cpstd::size_t bit=size_>>1U;
            while((reversed&bit)!=0U) { reversed^=bit; bit>>=1U; }
            reversed^=bit;
            if(index<reversed) {
                const Complex temporary=workspace_[index];
                workspace_[index]=workspace_[reversed];
                workspace_[reversed]=temporary;
            }
        }
    }

    cpstd::vector<Complex> workspace_;
    cpstd::size_t size_{};
};

}} // namespace DspCore::Transforms

#endif // DSPCORE_TRANSFORMS_FFT_H
