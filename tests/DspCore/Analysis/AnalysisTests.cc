#include <DspCore_Analysis.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::Analysis;

TEST(Levels, PeakAndRmsHaveExplicitBlockAndEmptySemantics) {
    const double input[]={-3.,4.,0.,0.}; double output=99;
    ASSERT_TRUE(Peak(input,4,output)); EXPECT_EQ(output,4.);
    ASSERT_TRUE(RMS(input,4,output)); EXPECT_EQ(output,2.5);
    ASSERT_TRUE(Peak(static_cast<const double*>(nullptr),0,output)); EXPECT_EQ(output,0.);
    ASSERT_TRUE(RMS(static_cast<const double*>(nullptr),0,output)); EXPECT_EQ(output,0.);
}
TEST(Levels, ScaledRmsHandlesExtremeFiniteValuesAndSmallSignals) {
    const double maximum=std::numeric_limits<double>::max();
    const double large[]={maximum,-maximum}; double output=0;
    ASSERT_TRUE(RMS(large,2,output)); EXPECT_EQ(output,maximum);
    const double tiny=std::numeric_limits<double>::min(); const double small[]={tiny,-tiny};
    ASSERT_TRUE(RMS(small,2,output)); EXPECT_EQ(output,tiny);
    const double mixed[]={maximum,0};
    ASSERT_TRUE(RMS(mixed,2,output)); EXPECT_NEAR(output/maximum,std::sqrt(0.5),1e-15);
}
TEST(Levels, InvalidBuffersSamplesAndCountsPreserveOutput) {
    float output=99;
    const float nan[]={0,std::numeric_limits<float>::quiet_NaN()};
    EXPECT_FALSE(Peak(nan,2,output)); EXPECT_FALSE(RMS(nan,2,output));
    EXPECT_FALSE(Peak(static_cast<const float*>(nullptr),1,output));
    EXPECT_FALSE(RMS(nan,std::numeric_limits<size_t>::max()/sizeof(float)+1,output));
    EXPECT_EQ(output,99);
}
TEST(Levels, SineReferenceAndOutputAliasingAreSupported) {
    double wave[64];
    for(size_t i=0;i<64;++i) wave[i]=std::sin(2.*std::acos(-1.)*static_cast<double>(i)/64.);
    double output=0;
    ASSERT_TRUE(RMS(wave,64,output)); EXPECT_NEAR(output,std::sqrt(0.5),1e-15);
    ASSERT_TRUE(Peak(wave,64,wave[0])); EXPECT_EQ(wave[0],1.);
    double input[]={-3,4}; ASSERT_TRUE(RMS(input,2,input[0])); EXPECT_NEAR(input[0],std::sqrt(12.5),1e-15);
}
TEST(Follower, RectificationAttackReleaseAndZeroTimes) {
    EnvelopeFollower<double> follower;
    ASSERT_TRUE(follower.Configure(1.,2.)); double output=0;
    ASSERT_TRUE(follower.Update(-1.,1.,output)); EXPECT_NEAR(output,1.-std::exp(-1.),1e-15);
    const double peak=output;
    ASSERT_TRUE(follower.Update(0.,2.,output)); EXPECT_NEAR(output,peak*std::exp(-1.),1e-15);
    ASSERT_TRUE(follower.Configure(0.,0.));
    ASSERT_TRUE(follower.Update(-3.,1.,output)); EXPECT_EQ(output,3.);
}
TEST(Follower, TinyIntervalsAndInvalidInputPreserveState) {
    EnvelopeFollower<double> follower;
    ASSERT_TRUE(follower.Configure(1.,1.)); double output=99;
    ASSERT_TRUE(follower.Update(1.,1e-12,output)); EXPECT_NEAR(output,1e-12,1e-24);
    const double previous=output;
    EXPECT_FALSE(follower.Configure(-1.,1.)); EXPECT_FALSE(follower.Reset(-1.));
    EXPECT_FALSE(follower.Update(1.,0.,output));
    EXPECT_FALSE(follower.Update(std::numeric_limits<double>::infinity(),1.,output));
    EXPECT_EQ(output,previous); EXPECT_EQ(follower.Value(),previous);
}
TEST(Follower, CopiesAreIndependentAndDoublePrecisionIsPreserved) {
    EnvelopeFollower<double> original; auto copy=original; double output=0;
    const double precise=1.+8.*std::numeric_limits<double>::epsilon();
    ASSERT_TRUE(copy.Update(precise,1.,output)); EXPECT_EQ(output,precise); EXPECT_EQ(original.Value(),0.);
}
