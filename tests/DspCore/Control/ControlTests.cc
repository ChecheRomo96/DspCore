#include <DspCore_Control.h>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>
using namespace DspCore::Control;

TEST(Smoother, TimeConstantAndBlockPartitionMatchAnalyticResponse) {
    Smoother<double> one, split;
    ASSERT_TRUE(one.Configure(1.)); ASSERT_TRUE(split.Configure(1.));
    double output=0;
    ASSERT_TRUE(one.Update(1.,1.,output)); EXPECT_NEAR(output,1.-std::exp(-1.),1e-15);
    for(int i=0;i<10;++i) { ASSERT_TRUE(split.Update(1.,0.1,output)); }
    EXPECT_NEAR(output,one.Value(),2e-15);
    ASSERT_TRUE(split.Reset()); ASSERT_TRUE(split.Update(1.,1e-12,output));
    EXPECT_NEAR(output,1e-12,1e-24);
}
TEST(Smoother, ZeroTauAndLargeIntervalsSnapAndInvalidUpdatesPreserveState) {
    Smoother<double> smoother;
    double out=99;
    ASSERT_TRUE(smoother.Update(3.,0.1,out)); EXPECT_EQ(out,3.);
    EXPECT_FALSE(smoother.Configure(-1.)); EXPECT_EQ(smoother.TimeConstant(),0.);
    EXPECT_FALSE(smoother.Update(5.,0.,out)); EXPECT_EQ(out,3.); EXPECT_EQ(smoother.Value(),3.);
    EXPECT_FALSE(smoother.Reset(std::numeric_limits<double>::quiet_NaN()));
    ASSERT_TRUE(smoother.Configure(std::numeric_limits<double>::min()));
    ASSERT_TRUE(smoother.Update(-2.,std::numeric_limits<double>::max(),out)); EXPECT_EQ(out,-2.);
}
TEST(Ramp, ExactArrivalRetargetAndOvershootInterval) {
    LinearRamp<double> ramp;
    double out=99;
    ASSERT_TRUE(ramp.SetTarget(10.,2.));
    ASSERT_TRUE(ramp.Update(0.5,out)); EXPECT_EQ(out,2.5); EXPECT_EQ(ramp.Remaining(),1.5);
    ASSERT_TRUE(ramp.SetTarget(-1.5,1.));
    ASSERT_TRUE(ramp.Update(0.5,out)); EXPECT_EQ(out,0.5);
    ASSERT_TRUE(ramp.Update(10.,out)); EXPECT_EQ(out,-1.5); EXPECT_EQ(ramp.Remaining(),0.);
    ASSERT_TRUE(ramp.SetTarget(7.,0.)); EXPECT_EQ(ramp.Value(),7.);
    ASSERT_TRUE(ramp.Update(1.,out)); EXPECT_EQ(out,7.);
}
TEST(Ramp, InvalidTargetAndIntervalDoNotChangeProgress) {
    LinearRamp<float> ramp;
    ASSERT_TRUE(ramp.SetTarget(1.f,1.f)); float out=42;
    EXPECT_FALSE(ramp.SetTarget(5.f,-1.f)); EXPECT_EQ(ramp.Target(),1);
    EXPECT_FALSE(ramp.Update(0.f,out)); EXPECT_EQ(out,42); EXPECT_EQ(ramp.Remaining(),1);
    EXPECT_FALSE(ramp.Update(std::numeric_limits<float>::infinity(),out));
    ASSERT_TRUE(ramp.Update(0.5f,out)); EXPECT_FLOAT_EQ(out,0.5f);
}
TEST(Slew, AsymmetricRatesHoldAndExactSnapping) {
    SlewLimiter<double> slew;
    ASSERT_TRUE(slew.Configure(2.,4.)); double out=0;
    ASSERT_TRUE(slew.Update(10.,0.5,out)); EXPECT_EQ(out,1.);
    ASSERT_TRUE(slew.Update(-10.,0.5,out)); EXPECT_EQ(out,-1.);
    ASSERT_TRUE(slew.Configure(0.,1.));
    ASSERT_TRUE(slew.Update(100.,1.,out)); EXPECT_EQ(out,-1.);
    ASSERT_TRUE(slew.Reset(1e20)); ASSERT_TRUE(slew.Configure(1e20,1e20));
    ASSERT_TRUE(slew.Update(1.,1.,out)); EXPECT_EQ(out,1.);
}
TEST(Slew, OverflowAndInvalidRatesLeaveOutputAndStateUnchanged) {
    SlewLimiter<float> slew; float out=42;
    EXPECT_FALSE(slew.Configure(-1.f,1.f));
    ASSERT_TRUE(slew.Configure(std::numeric_limits<float>::max(),1.f));
    EXPECT_FALSE(slew.Update(1.f,std::numeric_limits<float>::max(),out));
    EXPECT_EQ(out,42); EXPECT_EQ(slew.Value(),0);
    EXPECT_FALSE(slew.Update(std::numeric_limits<float>::quiet_NaN(),1.f,out));
}
TEST(Hysteresis, InclusiveThresholdsLatchAndInvalidConfigurationPreservesState) {
    Hysteresis<double> trigger;
    ASSERT_TRUE(trigger.Configure(0.25,0.75)); bool out=true;
    ASSERT_TRUE(trigger.Update(0.5,out)); EXPECT_FALSE(out);
    ASSERT_TRUE(trigger.Update(0.75,out)); EXPECT_TRUE(out);
    ASSERT_TRUE(trigger.Update(0.5,out)); EXPECT_TRUE(out);
    EXPECT_FALSE(trigger.Configure(1.,1.));
    EXPECT_FALSE(trigger.Update(std::numeric_limits<double>::quiet_NaN(),out)); EXPECT_TRUE(out);
    ASSERT_TRUE(trigger.Update(0.25,out)); EXPECT_FALSE(out);
    trigger.Reset(true); EXPECT_TRUE(trigger.Active());
}
TEST(Mapping, ExplicitBoundaryPoliciesAndInvertedOutputRanges) {
    double out=99;
    ASSERT_TRUE(Map(0.25,0.,1.,100.,0.,out,MappingBoundary::Clamp)); EXPECT_EQ(out,75.);
    ASSERT_TRUE(Map(2.,0.,1.,0.,10.,out,MappingBoundary::Clamp)); EXPECT_EQ(out,10.);
    EXPECT_FALSE(Map(2.,0.,1.,0.,10.,out,MappingBoundary::Reject)); EXPECT_EQ(out,10.);
    ASSERT_TRUE(Map(2.,0.,1.,0.,10.,out,MappingBoundary::Extrapolate)); EXPECT_EQ(out,20.);
    EXPECT_FALSE(Map(0.,1.,1.,0.,10.,out,MappingBoundary::Clamp)); EXPECT_EQ(out,20.);
    EXPECT_FALSE(Map(0.,0.,1.,0.,1.,out,static_cast<MappingBoundary>(99))); EXPECT_EQ(out,20.);
}
TEST(Deadband, ContinuousWidthSubtractionIsSymmetric) {
    double out=99;
    ASSERT_TRUE(Deadband(0.1,0.2,out)); EXPECT_EQ(out,0.);
    ASSERT_TRUE(Deadband(0.2,0.2,out)); EXPECT_EQ(out,0.);
    ASSERT_TRUE(Deadband(0.5,0.2,out)); EXPECT_DOUBLE_EQ(out,0.3);
    ASSERT_TRUE(Deadband(-0.5,0.2,out)); EXPECT_DOUBLE_EQ(out,-0.3);
    EXPECT_FALSE(Deadband(1.,-1.,out)); EXPECT_DOUBLE_EQ(out,-0.3);
}
TEST(Control, DoublePrecisionAndValueCopiesPreserveIndependentState) {
    Smoother<double> original; original.Reset(1.);
    auto copy=original;
    const double precise=1.+8.*std::numeric_limits<double>::epsilon();
    double out=0;
    ASSERT_TRUE(copy.Update(precise,1.,out)); EXPECT_EQ(out,precise); EXPECT_NE(out,1.);
    EXPECT_EQ(original.Value(),1.);
    LinearRamp<double> ramp;
    ASSERT_TRUE(ramp.Reset(-std::numeric_limits<double>::max()));
    ASSERT_TRUE(ramp.SetTarget(std::numeric_limits<double>::max(),2.));
    ASSERT_TRUE(ramp.Update(1.,out)); EXPECT_EQ(out,0.);
}
