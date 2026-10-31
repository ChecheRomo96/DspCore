#include <DspCore_Transforms.h>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

namespace {
using Complex=Foundation::Math::Complex<double>;

bool Near(const Complex& actual,const Complex& expected,const double tolerance=1e-12) {
    return std::abs(actual.real-expected.real)<=tolerance &&
        std::abs(actual.imag-expected.imag)<=tolerance;
}

template<typename Transform> void CheckForwardInverse(Transform& transform) {
    Complex input[]={Complex(1.,0.),Complex(2.,-1.),Complex(-3.,0.5),Complex(4.,2.)};
    Complex spectrum[4]{};
    Complex reconstructed[4]{};
    ASSERT_TRUE(transform.Forward(DspCore::Processing::MakeInputBlock(input,4U),
        DspCore::Processing::MakeOutputBlock(spectrum,4U)));
    ASSERT_TRUE(transform.Inverse(DspCore::Processing::MakeInputBlock(spectrum,4U),
        DspCore::Processing::MakeOutputBlock(reconstructed,4U)));
    for(unsigned index=0U;index<4U;++index) EXPECT_TRUE(Near(reconstructed[index],input[index]));
}
}

TEST(DFT, ForwardAndInverseRoundTripComplexInput) {
    DspCore::Transforms::DFT<double> dft;
    ASSERT_TRUE(dft.Configure(4U));
    CheckForwardInverse(dft);
}

TEST(DFT, ForwardRealMapsAlternatingSignalToNyquistBin) {
    DspCore::Transforms::DFT<double> dft;
    ASSERT_TRUE(dft.Configure(4U));
    const double input[]={1.,-1.,1.,-1.};
    Complex spectrum[4]{};
    ASSERT_TRUE(dft.ForwardReal(DspCore::Processing::MakeInputBlock(input,4U),
        DspCore::Processing::MakeOutputBlock(spectrum,4U)));
    EXPECT_TRUE(Near(spectrum[0],Complex(0.,0.)));
    EXPECT_TRUE(Near(spectrum[1],Complex(0.,0.)));
    EXPECT_TRUE(Near(spectrum[2],Complex(4.,0.)));
    EXPECT_TRUE(Near(spectrum[3],Complex(0.,0.)));
}

TEST(DFT, RejectsBadBlocksAndNonFiniteInputWithoutWritingOutput) {
    DspCore::Transforms::DFT<double> dft;
    ASSERT_TRUE(dft.Configure(2U));
    Complex output[]={Complex(7.,8.),Complex(9.,10.)};
    const Complex invalid[]={Complex(std::numeric_limits<double>::infinity(),0.),Complex(0.,0.)};
    EXPECT_FALSE(dft.Forward(DspCore::Processing::MakeInputBlock(invalid,2U),
        DspCore::Processing::MakeOutputBlock(output,2U)));
    EXPECT_TRUE(Near(output[0],Complex(7.,8.)));
    EXPECT_FALSE(dft.Forward(DspCore::Processing::SampleBlock<const Complex>{nullptr,2U},
        DspCore::Processing::MakeOutputBlock(output,2U)));
}

TEST(FFT, ForwardAndInverseRoundTripComplexInputAndAcceptInPlaceOutput) {
    DspCore::Transforms::FFT<double> fft;
    ASSERT_TRUE(fft.Configure(4U));
    CheckForwardInverse(fft);
    Complex inPlace[]={Complex(1.,0.),Complex(2.,0.),Complex(3.,0.),Complex(4.,0.)};
    ASSERT_TRUE(fft.Forward(DspCore::Processing::MakeInputBlock(inPlace,4U),
        DspCore::Processing::MakeOutputBlock(inPlace,4U)));
    ASSERT_TRUE(fft.Inverse(DspCore::Processing::MakeInputBlock(inPlace,4U),
        DspCore::Processing::MakeOutputBlock(inPlace,4U)));
    for(unsigned index=0U;index<4U;++index) EXPECT_TRUE(Near(inPlace[index],Complex(static_cast<double>(index+1U),0.)));
}

TEST(FFT, MatchesDftForRealInputAndRequiresAPowerOfTwo) {
    DspCore::Transforms::DFT<double> dft;
    DspCore::Transforms::FFT<double> fft;
    ASSERT_TRUE(dft.Configure(8U));
    EXPECT_FALSE(fft.Configure(6U));
    ASSERT_TRUE(fft.Configure(8U));
    const double input[]={0.,1.,2.,3.,4.,-2.,1.,0.};
    Complex dftOutput[8]{};
    Complex fftOutput[8]{};
    ASSERT_TRUE(dft.ForwardReal(DspCore::Processing::MakeInputBlock(input,8U),
        DspCore::Processing::MakeOutputBlock(dftOutput,8U)));
    ASSERT_TRUE(fft.ForwardReal(DspCore::Processing::MakeInputBlock(input,8U),
        DspCore::Processing::MakeOutputBlock(fftOutput,8U)));
    for(unsigned index=0U;index<8U;++index) EXPECT_TRUE(Near(fftOutput[index],dftOutput[index],1e-11));
}
