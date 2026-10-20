// Deliberately include biquad first: public headers must not depend on include order.
#include <DspCore/Filters/Biquad.h>
#include <DspCore/Filters/FIR.h>
#include <DspCore/Filters/IIR.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::Filters;

TEST(FIR, ImpulseMatchesAsymmetricTapOrderAndHistoryReset) {
    FIR<double> fir; const double taps[]={0.25,0.5,-0.125};
    ASSERT_TRUE(fir.Configure(taps,3)); double output=0;
    ASSERT_TRUE(fir.Process(1.,output)); EXPECT_EQ(output,0.25);
    ASSERT_TRUE(fir.Process(0.,output)); EXPECT_EQ(output,0.5);
    ASSERT_TRUE(fir.Process(0.,output)); EXPECT_EQ(output,-0.125);
    ASSERT_TRUE(fir.Process(0.,output)); EXPECT_EQ(output,0.);
    fir.Reset(); ASSERT_TRUE(fir.Process(1.,output)); EXPECT_EQ(output,0.25);
}
TEST(FIR, DefaultIdentityAndDcGain) {
    FIR<double> identity; double out=0;
    ASSERT_TRUE(identity.Process(0.123,out)); EXPECT_EQ(out,0.123);
    FIR<double> average; const double taps[]={0.25,0.5,0.25};
    ASSERT_TRUE(average.Configure(taps,3));
    for(int i=0;i<10;++i) { ASSERT_TRUE(average.Process(1.,out)); }
    EXPECT_EQ(out,1.); EXPECT_EQ(average.Taps(),3U);
}
TEST(FIR, ActiveTapCountChangesAtRuntimeWithVectorStorage) {
    FIR<double> filter; double output=0;
    const double twoTaps[]={0.5,0.5};
    ASSERT_TRUE(filter.Configure(twoTaps,2));
    EXPECT_EQ(filter.Taps(),2U); EXPECT_GE(filter.Capacity(),2U);
    ASSERT_TRUE(filter.Process(1.,output)); EXPECT_EQ(output,0.5);
    ASSERT_TRUE(filter.Process(0.,output)); EXPECT_EQ(output,0.5);

    const double threeTaps[]={0.25,0.5,0.25};
    ASSERT_TRUE(filter.Configure(threeTaps,3));
    EXPECT_EQ(filter.Taps(),3U); EXPECT_GE(filter.Capacity(),3U);
    ASSERT_TRUE(filter.Process(1.,output)); EXPECT_EQ(output,0.25);
    ASSERT_TRUE(filter.Process(0.,output)); EXPECT_EQ(output,0.5);
    const double fiveTaps[]={0.1,0.2,0.3,0.2,0.1};
    ASSERT_TRUE(filter.Configure(fiveTaps,5));
    EXPECT_EQ(filter.Taps(),5U); EXPECT_GE(filter.Capacity(),5U);
    EXPECT_FALSE(filter.Configure(fiveTaps,0));
    EXPECT_EQ(filter.Taps(),5U);
}
TEST(FIR, FailurePreservesHistoryCoefficientsAndCallerOutput) {
    FIR<float> fir; const float taps[]={0.5f,0.5f};
    ASSERT_TRUE(fir.Configure(taps,2)); float out=0;
    ASSERT_TRUE(fir.Process(1.f,out));
    auto copy=fir;
    const float bad[]={1,std::numeric_limits<float>::quiet_NaN()};
    EXPECT_FALSE(fir.Configure(bad,2)); EXPECT_FALSE(fir.Configure(0,2));
    out=99; EXPECT_FALSE(fir.Process(std::numeric_limits<float>::infinity(),out)); EXPECT_EQ(out,99);
    float reference=0; ASSERT_TRUE(fir.Process(0.f,out)); ASSERT_TRUE(copy.Process(0.f,reference)); EXPECT_EQ(out,reference);
    const float huge[]={std::numeric_limits<float>::max(),1};
    ASSERT_TRUE(fir.Configure(huge,2)); out=99;
    EXPECT_FALSE(fir.Process(2.f,out)); EXPECT_EQ(out,99);
    ASSERT_TRUE(fir.Process(0.f,out)); EXPECT_EQ(out,0);
}
TEST(FIR, ImplementsSisoProcessorBaseAndRejectsInvalidBlocksBeforeProcessing) {
    FIR<float> fir; const float taps[]={0.5f,0.5f};
    ASSERT_TRUE(fir.Configure(taps,2));
    DspCore::Processing::ProcessorBase<float>& processor=fir;
    const float input[]={1.f,0.f,0.f}; float output[]={99.f,99.f,99.f};
    DspCore::Processing::InputBlock<float> inputs[]={DspCore::Processing::InputBlock<float>(input,3)};
    DspCore::Processing::OutputBlock<float> outputs[]={DspCore::Processing::OutputBlock<float>(output,3)};
    EXPECT_EQ(processor.InputPorts(),1U); EXPECT_EQ(processor.OutputPorts(),1U);
    ASSERT_TRUE(processor.Process(inputs,1,outputs,1));
    EXPECT_EQ(outputs[0].size,3U); EXPECT_FLOAT_EQ(output[0],0.5f);
    EXPECT_FLOAT_EQ(output[1],0.5f); EXPECT_FLOAT_EQ(output[2],0.f);

    FIR<float> reference=fir;
    float tooSmall[]={77.f,88.f};
    DspCore::Processing::OutputBlock<float> shortOutput(tooSmall,2); shortOutput.size=2;
    EXPECT_FALSE(processor.Process(inputs,1,&shortOutput,1));
    EXPECT_EQ(shortOutput.size,2U); EXPECT_FLOAT_EQ(tooSmall[0],77.f);
    float actual=0,expected=0;
    ASSERT_TRUE(fir.Process(0.f,actual)); ASSERT_TRUE(reference.Process(0.f,expected));
    EXPECT_FLOAT_EQ(actual,expected);

    const float invalid[]={0.f,std::numeric_limits<float>::infinity()};
    float unchanged[]={66.f,55.f};
    DspCore::Processing::InputBlock<float> invalidInput(invalid,2);
    DspCore::Processing::OutputBlock<float> unchangedOutput(unchanged,2); unchangedOutput.size=2;
    reference=fir;
    EXPECT_FALSE(processor.Process(&invalidInput,1,&unchangedOutput,1));
    EXPECT_EQ(unchangedOutput.size,2U); EXPECT_FLOAT_EQ(unchanged[0],66.f);
    ASSERT_TRUE(fir.Process(0.f,actual)); ASSERT_TRUE(reference.Process(0.f,expected));
    EXPECT_FLOAT_EQ(actual,expected);
}

TEST(IIR, FirstOrderImpulseAndDefaultIdentity) {
    IIR<double> filter;double out=0;
    ASSERT_TRUE(filter.Process(0.25,out));EXPECT_EQ(out,0.25);
    Biquad::Coefficients<double> c;c.numerator[0]=1;c.denominator[0]=2;c.denominator[1]=-1;
    ASSERT_TRUE(filter.Configure(c));
    ASSERT_TRUE(filter.Process(1.,out));EXPECT_EQ(out,0.5);
    ASSERT_TRUE(filter.Process(0.,out));EXPECT_EQ(out,0.25);
    ASSERT_TRUE(filter.Process(0.,out));EXPECT_EQ(out,0.125);
    double normalized=99;ASSERT_TRUE(filter.GetDenominator(0,normalized));EXPECT_EQ(normalized,1.);
    ASSERT_TRUE(filter.GetNumerator(0,normalized));EXPECT_EQ(normalized,0.5);
    EXPECT_EQ(c.denominator[0],2.);
    filter.Reset();ASSERT_TRUE(filter.Process(1.,out));EXPECT_EQ(out,0.5);
}
TEST(IIR, ArbitraryNumeratorAndDenominatorCountsAreIndependentOfBiquad) {
    IIR<double,4,2> filter;
    const double b[]={1,2,3,4},a[]={2,-1};
    ASSERT_TRUE(filter.Configure(b,a));double out=0;
    const double expected[]={0.5,1.25,2.125,3.0625,1.53125};
    for(size_t i=0;i<5;++i) { ASSERT_TRUE(filter.Process(i==0 ? 1. : 0.,out));EXPECT_EQ(out,expected[i]); }
    Biquad::Coefficients<double> wrongSize;
    EXPECT_FALSE(filter.Configure(wrongSize));
    IIR<double,1,1> gain;const double numerator[]={3.},denominator[]={2.};
    ASSERT_TRUE(gain.Configure(numerator,denominator));ASSERT_TRUE(gain.Process(2.,out));EXPECT_EQ(out,3.);
}
TEST(IIR, GenericRecurrenceMatchesIndependentDifferenceEquation) {
    IIR<double> filter;Biquad::Coefficients<double> c;
    c.numerator[0]=0.4;c.numerator[1]=0.6;c.numerator[2]=0.2;
    c.denominator[0]=2;c.denominator[1]=-1;c.denominator[2]=0.4;
    ASSERT_TRUE(filter.Configure(c));double x1=0,x2=0,y1=0,y2=0;
    for(int i=0;i<100;++i) {
        const double x=std::sin(static_cast<double>(i)*0.37);
        const double reference=(c.numerator[0]*x+c.numerator[1]*x1+c.numerator[2]*x2-c.denominator[1]*y1-c.denominator[2]*y2)/c.denominator[0];
        double out=0;ASSERT_TRUE(filter.Process(x,out));EXPECT_NEAR(out,reference,2e-15);
        x2=x1;x1=x;y2=y1;y1=reference;
    }
}
TEST(IIR, InvalidNormalizationAndOverflowPreserveConfigurationAndHistory) {
    IIR<double> filter;Biquad::Coefficients<double> c;
    c.numerator[0]=0.5;c.denominator[1]=-0.5;ASSERT_TRUE(filter.Configure(c));
    double out=0;ASSERT_TRUE(filter.Process(1.,out));auto copy=filter;
    c.denominator[0]=0;EXPECT_FALSE(filter.Configure(c));
    c.denominator[0]=std::numeric_limits<double>::quiet_NaN();EXPECT_FALSE(filter.Configure(c));
    c.denominator[0]=std::numeric_limits<double>::min();c.numerator[0]=std::numeric_limits<double>::max();
    EXPECT_FALSE(filter.Configure(c));
    out=99;EXPECT_FALSE(filter.Process(std::numeric_limits<double>::infinity(),out));EXPECT_EQ(out,99.);
    double reference=0;ASSERT_TRUE(filter.Process(0.,out));ASSERT_TRUE(copy.Process(0.,reference));EXPECT_EQ(out,reference);
    c=Biquad::Coefficients<double>{};c.numerator[0]=std::numeric_limits<double>::max();
    ASSERT_TRUE(filter.Configure(c));out=99;EXPECT_FALSE(filter.Process(2.,out));EXPECT_EQ(out,99.);
    ASSERT_TRUE(filter.Process(0.,out));EXPECT_EQ(out,0.);
}
TEST(IIR, ImplementsSisoProcessorBaseWithOneOutputPerInput) {
    IIR<double> filter; Biquad::Coefficients<double> c;
    c.numerator[0]=1.; c.denominator[0]=2.; c.denominator[1]=-1.;
    ASSERT_TRUE(filter.Configure(c));
    DspCore::Processing::ProcessorBase<double>& processor=filter;
    const double input[]={1.,0.,0.}; double output[]={0.,0.,0.};
    DspCore::Processing::InputBlock<double> inputs[]={DspCore::Processing::InputBlock<double>(input,3)};
    DspCore::Processing::OutputBlock<double> outputs[]={DspCore::Processing::OutputBlock<double>(output,3)};
    ASSERT_TRUE(processor.Process(inputs,1,outputs,1));
    EXPECT_EQ(outputs[0].size,3U); EXPECT_EQ(output[0],0.5);
    EXPECT_EQ(output[1],0.25); EXPECT_EQ(output[2],0.125);
}
TEST(BiquadCalculator, SixRawCoefficientsNormalizeToKnownButterworthValues) {
    const auto c=Biquad::LowPass(48000.,12000.,std::sqrt(0.5));
    ASSERT_TRUE(Biquad::IsStable(c));
    EXPECT_NEAR(c.numerator[0],0.5,1e-15);EXPECT_NEAR(c.numerator[1],1.,1e-15);
    EXPECT_NEAR(c.denominator[0],1.+std::sqrt(0.5),1e-15);
    IIR<double> filter;ASSERT_TRUE(filter.Configure(c));double value=0;
    ASSERT_TRUE(filter.GetNumerator(0,value));EXPECT_NEAR(value,0.2928932188134524,1e-15);
    ASSERT_TRUE(filter.GetNumerator(1,value));EXPECT_NEAR(value,0.5857864376269049,1e-15);
    ASSERT_TRUE(filter.GetDenominator(2,value));EXPECT_NEAR(value,0.17157287525380993,1e-15);
    const auto high=Biquad::HighPass(48000.,12000.,std::sqrt(0.5));
    ASSERT_TRUE(filter.Configure(high));ASSERT_TRUE(filter.GetNumerator(1,value));EXPECT_NEAR(value,-0.5857864376269049,1e-15);
}
TEST(BiquadCalculator, OneCalculatedStructConfiguresIndependentIirInstances) {
    const auto coefficients=Biquad::LowPass(48000.,12000.,std::sqrt(0.5));
    IIR<double> left,right;ASSERT_TRUE(left.Configure(coefficients));ASSERT_TRUE(right.Configure(coefficients));
    double l=0,r=0;ASSERT_TRUE(left.Process(1.,l));ASSERT_TRUE(right.Process(0.,r));
    EXPECT_GT(l,0.);EXPECT_EQ(r,0.);
}
TEST(BiquadCalculator, DcNyquistAndCutoffResponsesMatchExpectedGains) {
    const auto low=Biquad::LowPass(48000.,12000.,std::sqrt(0.5));
    const auto high=Biquad::HighPass(48000.,12000.,std::sqrt(0.5));
    IIR<double> lp,hp;ASSERT_TRUE(lp.Configure(low));ASSERT_TRUE(hp.Configure(high));
    double l=0,h=0;
    for(int i=0;i<512;++i) { ASSERT_TRUE(lp.Process(1.,l));ASSERT_TRUE(hp.Process(1.,h)); }
    EXPECT_NEAR(l,1.,1e-14);EXPECT_NEAR(h,0.,1e-14);
    lp.Reset();hp.Reset();
    for(int i=0;i<512;++i) { const double x=i%2==0 ? 1. : -1.;ASSERT_TRUE(lp.Process(x,l));ASSERT_TRUE(hp.Process(x,h)); }
    EXPECT_NEAR(l,0.,1e-14);EXPECT_NEAR(h,-1.,1e-14);
    lp.Reset();const double wave[]={0,1,0,-1};double power=0;
    for(int i=0;i<256;++i) { ASSERT_TRUE(lp.Process(wave[i%4],l));if(i>=128) power+=l*l; }
    EXPECT_NEAR(power/128.,0.25,1e-14);
}
TEST(BiquadCalculator, InvalidDesignReturnsZeroA0AndIirRejectsIt) {
    const auto c=Biquad::LowPass(0.f,1.f,1.f);
    EXPECT_EQ(c.denominator[0],0);EXPECT_FALSE(Biquad::IsStable(c));
    EXPECT_EQ(Biquad::LowPass(48000.f,24000.f,1.f).denominator[0],0);
    EXPECT_EQ(Biquad::HighPass(48000.f,0.f,1.f).denominator[0],0);
    EXPECT_EQ(Biquad::HighPass(48000.f,1000.f,0.f).denominator[0],0);
    EXPECT_EQ(Biquad::LowPass(std::numeric_limits<float>::max(),std::numeric_limits<float>::min(),1.f).denominator[0],0);
    IIR<float> filter;EXPECT_FALSE(filter.Configure(c));float out=0;
    ASSERT_TRUE(filter.Process(1.f,out));EXPECT_EQ(out,1);
}
TEST(IIR, GenericProcessorDoesNotInferStabilityForUserCoefficients) {
    Biquad::Coefficients<double> c;c.denominator[1]=1;
    EXPECT_FALSE(Biquad::IsStable(c));IIR<double> filter;
    EXPECT_TRUE(filter.Configure(c)); // General recurrence policy is independent of the calculator.
}
