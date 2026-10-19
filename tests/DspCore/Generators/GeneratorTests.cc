#include <DspCore_Generators.h>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <algorithm>
using namespace DspCore::Generators;

TEST(Periodic, ReferenceSamplesAndHistoricalPhaseConventions) {
    double out[4];
    ASSERT_TRUE(Periodic::Sine(out,4));
    EXPECT_NEAR(out[0],0,1e-14); EXPECT_NEAR(out[1],1,1e-14);
    EXPECT_NEAR(out[2],0,1e-14); EXPECT_NEAR(out[3],-1,1e-14);
    ASSERT_TRUE(Periodic::Saw(out,4));
    const double saw[]={0,0.5,-1,-0.5};
    for(size_t i=0;i<4;++i) EXPECT_EQ(out[i],saw[i]);
    ASSERT_TRUE(Periodic::Square(out,4));
    const double square[]={1,1,-1,-1};
    for(size_t i=0;i<4;++i) EXPECT_EQ(out[i],square[i]);
    ASSERT_TRUE(Periodic::Triangle(out,4));
    const double triangle[]={0,1,0,-1};
    for(size_t i=0;i<4;++i) EXPECT_EQ(out[i],triangle[i]);
}
TEST(Periodic, PhaseWrapGainOffsetAndValidation) {
    float positive[8], negative[8];
    ASSERT_TRUE(Periodic::Sine(positive,8,2.f,3.f,0.75f));
    ASSERT_TRUE(Periodic::Sine(negative,8,2.f,3.f,-0.25f));
    for(size_t i=0;i<8;++i) EXPECT_FLOAT_EQ(positive[i],negative[i]);
    EXPECT_NEAR(positive[0],1.f,1e-6f);
    float sentinel[]={9,9};
    EXPECT_FALSE(Periodic::Sine(sentinel,2,-1.f));
    EXPECT_FALSE(Periodic::Saw(sentinel,2,1.f,0.f,std::numeric_limits<float>::quiet_NaN()));
    EXPECT_FALSE(Periodic::Square(sentinel,2,std::numeric_limits<float>::max(),std::numeric_limits<float>::max()));
    EXPECT_FALSE(Periodic::Triangle(static_cast<float*>(nullptr),2));
    EXPECT_EQ(sentinel[0],9); EXPECT_EQ(sentinel[1],9);
    EXPECT_TRUE(Periodic::Sine(static_cast<float*>(nullptr),0));
}
TEST(Shaping, NormalizedSigmoidsHaveExactEndpointsAndOddSymmetry) {
    double out[9];
    using Generator=bool(*)(double*,size_t,double,double);
    const Generator functions[]={Shaping::AtanNorm<double>,Shaping::TanhNorm<double>};
    for(auto function:functions) {
        ASSERT_TRUE(function(out,9,2.,3.));
        EXPECT_EQ(out[0],-3.); EXPECT_EQ(out[8],3.); EXPECT_EQ(out[4],0.);
        for(size_t i=0;i<9;++i) EXPECT_NEAR(out[i],-out[8-i],1e-14);
        EXPECT_TRUE(std::is_sorted(out,out+9));
    }
}
TEST(Shaping, RawAndHalfCurvesMatchIndependentReferences) {
    double out[5];
    ASSERT_TRUE(Shaping::Atan(out,5,2.));
    EXPECT_NEAR(out[0],std::atan(-2.),1e-15); EXPECT_NEAR(out[3],std::atan(1.),1e-15);
    ASSERT_TRUE(Shaping::Tanh(out,5,2.)); EXPECT_NEAR(out[3],std::tanh(1.),1e-15);
    ASSERT_TRUE(Shaping::AtanHalf(out,5,2.)); EXPECT_EQ(out[0],0.); EXPECT_NEAR(out[4],std::atan(2.),1e-15);
    ASSERT_TRUE(Shaping::TanhHalf(out,5,2.)); EXPECT_EQ(out[0],0.); EXPECT_NEAR(out[4],std::tanh(2.),1e-15);
    ASSERT_TRUE(Shaping::AtanHalfNorm(out,5,2.)); EXPECT_EQ(out[0],0.); EXPECT_EQ(out[4],1.);
    EXPECT_NEAR(out[2],std::atan(1.)/std::atan(2.),1e-15);
    ASSERT_TRUE(Shaping::TanhHalfNorm(out,5,2.)); EXPECT_EQ(out[4],1.);
    EXPECT_NEAR(out[2],std::tanh(1.)/std::tanh(2.),1e-15);
}
TEST(Shaping, GammaMorphAndSingletonDomain) {
    double gamma[5], morph[5];
    ASSERT_TRUE(Shaping::Gamma(gamma,5,2.,4.));
    EXPECT_EQ(gamma[0],0.); EXPECT_EQ(gamma[2],1.); EXPECT_EQ(gamma[4],4.);
    ASSERT_TRUE(Shaping::GammaMorph(morph,5,1.,2.,4.));
    for(size_t i=0;i<5;++i) EXPECT_EQ(gamma[i],morph[i]);
    ASSERT_TRUE(Shaping::Gamma(gamma,1,2.)); EXPECT_EQ(gamma[0],0.);
    ASSERT_TRUE(Shaping::AtanNorm(gamma,1,2.)); EXPECT_EQ(gamma[0],0.);
    ASSERT_TRUE(Shaping::TanhHalfNorm(gamma,1,2.)); EXPECT_EQ(gamma[0],0.);
}
TEST(Shaping, InvalidAndExtremeParametersDoNotWriteOnFailure) {
    float out[]={7,7,7};
    EXPECT_FALSE(Shaping::Gamma(out,3,0.f));
    EXPECT_FALSE(Shaping::AtanNorm(out,3,-1.f));
    EXPECT_FALSE(Shaping::Tanh(out,3,std::numeric_limits<float>::infinity()));
    EXPECT_FALSE(Shaping::GammaMorph(out,3,1000.f,10.f));
    EXPECT_FALSE(Shaping::GammaMorph(out,3,-1000.f,10.f));
    EXPECT_FALSE(Shaping::Atan(out,3,1.f,std::numeric_limits<float>::max()));
    for(float sample:out) EXPECT_EQ(sample,7);
    ASSERT_TRUE(Shaping::TanhNorm(out,3,std::numeric_limits<float>::denorm_min()));
    EXPECT_EQ(out[0],-1); EXPECT_EQ(out[1],0); EXPECT_EQ(out[2],1);
}
TEST(Envelopes, LinearSegmentsAreHalfOpenAndPreserveSurroundingSamples) {
    double out[7]; std::fill(out,out+7,99.);
    ASSERT_TRUE(Envelopes::LinearSegment(out,2.,8.,7,1,5));
    EXPECT_EQ(out[0],99.); EXPECT_EQ(out[1],2.); EXPECT_EQ(out[2],4.); EXPECT_EQ(out[4],8.); EXPECT_EQ(out[5],99.);
    ASSERT_TRUE(Envelopes::LinearSegment(out,3.,9.,7,5,6)); EXPECT_EQ(out[5],3.);
    EXPECT_TRUE(Envelopes::LinearSegment(out,3.,9.,7,7,7));
    EXPECT_FALSE(Envelopes::LinearSegment(out,3.,9.,7,5,4));
    EXPECT_FALSE(Envelopes::LinearSegment(out,3.,9.,7,0,8));
    EXPECT_EQ(out[6],99.);
}
TEST(Envelopes, TinyCurvesAndZeroHaveStableLinearLimits) {
    double out[5];
    const double curves[]={0.,1e-30,1e-9,1e-5};
    for(double curve:curves) {
        ASSERT_TRUE(Envelopes::ExpSegment(out,0.,1.,curve,5));
        EXPECT_EQ(out[0],0.); EXPECT_EQ(out[4],1.);
        for(size_t i=0;i<5;++i) EXPECT_NEAR(out[i],static_cast<double>(i)/4.,2e-6);
        ASSERT_TRUE(Envelopes::LogSegment(out,0.,1.,curve,5));
        for(size_t i=0;i<5;++i) EXPECT_NEAR(out[i],static_cast<double>(i)/4.,2e-6);
    }
    float f[3];
    ASSERT_TRUE(Envelopes::ExpSegment(f,0.f,1.f,1e-9f,3)); EXPECT_FLOAT_EQ(f[1],0.5f);
    ASSERT_TRUE(Envelopes::LogSegment(f,0.f,1.f,1e-9f,3)); EXPECT_FLOAT_EQ(f[1],0.5f);
}
TEST(Envelopes, NonlinearValuesMatchExpm1Log1pReferences) {
    double out[9];
    const double curves[]={0.001,0.124,0.125,1.,20.,1000.};
    for(double curve:curves) {
        ASSERT_TRUE(Envelopes::ExpSegment(out,2.,8.,curve,9));
        for(size_t i=0;i<9;++i) {
            const double t=static_cast<double>(i)/8.;
            const double expected=2.+6.*std::expm1(-curve*t)/std::expm1(-curve);
            EXPECT_NEAR(out[i],expected,2e-13);
        }
        ASSERT_TRUE(Envelopes::LogSegment(out,2.,8.,curve,9));
        for(size_t i=0;i<9;++i) EXPECT_NEAR(out[i],2.+6.*std::log1p(curve*static_cast<double>(i)/8.)/std::log1p(curve),2e-13);
    }
}
TEST(Envelopes, ExtremeCurvesAndInvalidInputs) {
    double out[]={9,9,9};
    EXPECT_FALSE(Envelopes::ExpSegment(out,0.,1.,-1.,3));
    EXPECT_FALSE(Envelopes::LogSegment(out,0.,1.,std::numeric_limits<double>::quiet_NaN(),3));
    EXPECT_EQ(out[1],9.);
    ASSERT_TRUE(Envelopes::ExpSegment(out,0.,1.,std::numeric_limits<double>::max(),3));
    for(double x:out) EXPECT_TRUE(std::isfinite(x));
    ASSERT_TRUE(Envelopes::LogSegment(out,0.,1.,std::numeric_limits<double>::max(),3));
    for(double x:out) EXPECT_TRUE(std::isfinite(x));
    EXPECT_TRUE(Envelopes::LinearSegment(static_cast<float*>(nullptr),0.f,1.f,0));
}
TEST(Noise, ExplicitStatePreservesSequenceAcrossBlocks) {
    float whole[17], blocks[17];
    uint32_t a=0x12345678u,b=a;
    ASSERT_TRUE(Noise::White(whole,17,a));
    ASSERT_TRUE(Noise::White(blocks,5,b));
    ASSERT_TRUE(Noise::White(blocks+5,12,b));
    EXPECT_EQ(a,b);
    for(size_t i=0;i<17;++i) { EXPECT_EQ(whole[i],blocks[i]); EXPECT_GE(whole[i],-1); EXPECT_LE(whole[i],1); }
    uint32_t reference=1;
    double one;
    ASSERT_TRUE(Noise::White(&one,1,reference)); EXPECT_EQ(reference,270369u);
    EXPECT_NEAR(one,2.*270369./4294967295.-1.,1e-15);
}
TEST(Noise, ZeroStateAndInvalidParametersAreRejectedWithoutMutation) {
    float out[]={9,9}; uint32_t state=0;
    EXPECT_FALSE(Noise::White(out,2,state)); EXPECT_EQ(state,0u); EXPECT_EQ(out[0],9);
    state=123;
    EXPECT_FALSE(Noise::White(out,2,state,-1.f)); EXPECT_EQ(state,123u); EXPECT_EQ(out[1],9);
    EXPECT_TRUE(Noise::White(static_cast<float*>(nullptr),0,state)); EXPECT_EQ(state,123u);
}
TEST(Windows, SymmetricReferenceValuesEndpointsAndSingletons) {
    double out[5];
    ASSERT_TRUE(Windows::Hann(out,5));
    EXPECT_EQ(out[0],0.); EXPECT_NEAR(out[1],0.5,1e-15); EXPECT_EQ(out[2],1.); EXPECT_EQ(out[4],0.);
    EXPECT_NEAR(out[1],out[3],1e-15);
    ASSERT_TRUE(Windows::Hamming(out,5));
    EXPECT_EQ(out[0],0.08); EXPECT_NEAR(out[1],0.54,1e-15); EXPECT_EQ(out[2],1.); EXPECT_EQ(out[4],0.08);
    EXPECT_TRUE(Windows::Hann(out,1)); EXPECT_EQ(out[0],1.);
    EXPECT_TRUE(Windows::Hamming(out,1)); EXPECT_EQ(out[0],1.);
    EXPECT_TRUE(Windows::Hann(static_cast<float*>(nullptr),0));
    EXPECT_FALSE(Windows::Hamming(static_cast<float*>(nullptr),2));
}
TEST(Generators, InvalidByteCountsAreRejectedBeforeAccess) {
    float sentinel=9;
    const size_t overflow=std::numeric_limits<size_t>::max()/sizeof(float)+1;
    EXPECT_FALSE(Periodic::Sine(&sentinel,overflow));
    EXPECT_FALSE(Shaping::Gamma(&sentinel,overflow,1.f));
    EXPECT_FALSE(Envelopes::LinearSegment(&sentinel,0.f,1.f,overflow));
    EXPECT_FALSE(Windows::Hann(&sentinel,overflow));
    uint32_t seed=1; EXPECT_FALSE(Noise::White(&sentinel,overflow,seed));
    EXPECT_EQ(sentinel,9); EXPECT_EQ(seed,1u);
}
