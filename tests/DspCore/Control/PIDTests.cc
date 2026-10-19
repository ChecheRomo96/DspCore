#include <DspCore_Control.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::Control;

TEST(PID, ProportionalGainAndExplicitOutputLimits) {
    PID<double> pid; PidParameters<double> p;
    p.kp=2.;p.outputMin=-10;p.outputMax=10;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    ASSERT_TRUE(pid.Update(3.,1.,0.1,out)); EXPECT_EQ(out,4.);
    ASSERT_TRUE(pid.Update(100.,0.,0.1,out)); EXPECT_EQ(out,10.);
    ASSERT_TRUE(pid.Update(-100.,0.,0.1,out)); EXPECT_EQ(out,-10.);
}
TEST(PID, IntegralGainUsesSecondsAndCanBeReset) {
    PID<double> pid; PidParameters<double> p;
    p.kp=0;p.ki=2;p.outputMin=-10;p.outputMax=10;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    ASSERT_TRUE(pid.Update(1.,0.,0.5,out)); EXPECT_EQ(out,1.);
    ASSERT_TRUE(pid.Update(1.,0.,0.5,out)); EXPECT_EQ(out,2.); EXPECT_EQ(pid.Integral(),2.);
    ASSERT_TRUE(pid.Reset(3.)); EXPECT_EQ(pid.Output(),3.); EXPECT_EQ(pid.Integral(),3.);
}
TEST(PID, ConditionalIntegrationPreventsWindupAndAllowsRecovery) {
    PID<double> pid; PidParameters<double> p;
    p.kp=2;p.ki=1;p.outputMin=-1;p.outputMax=1;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    for(int i=0;i<100;++i) { ASSERT_TRUE(pid.Update(10.,0.,0.1,out)); }
    EXPECT_EQ(out,1.); EXPECT_EQ(pid.Integral(),0.);
    ASSERT_TRUE(pid.Update(0.,0.,0.1,out)); EXPECT_EQ(out,0.);
    ASSERT_TRUE(pid.Reset(0.8));
    ASSERT_TRUE(pid.Update(-0.1,0.,0.1,out));
    EXPECT_LT(pid.Integral(),0.8); EXPECT_NEAR(out,0.59,1e-15);
}
TEST(PID, DerivativeOnMeasurementAvoidsSetpointKickAndStartsWithoutImpulse) {
    PID<double> pid; PidParameters<double> p;
    p.kp=0;p.kd=2;p.outputMin=-100;p.outputMax=100;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    ASSERT_TRUE(pid.Update(0.,10.,0.5,out)); EXPECT_EQ(out,0.);
    ASSERT_TRUE(pid.Update(50.,10.,0.5,out)); EXPECT_EQ(out,0.);
    ASSERT_TRUE(pid.Update(50.,11.,0.5,out)); EXPECT_EQ(out,-4.);
    ASSERT_TRUE(pid.Reset());
    ASSERT_TRUE(pid.Update(50.,20.,0.5,out)); EXPECT_EQ(out,0.);
}
TEST(PID, DerivativeFilterMatchesAnalyticOnePole) {
    PID<double> pid; PidParameters<double> p;
    p.kp=0;p.kd=1;p.derivativeTimeConstant=1;p.outputMin=-100;p.outputMax=100;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    ASSERT_TRUE(pid.Update(0.,0.,1.,out));
    ASSERT_TRUE(pid.Update(0.,1.,1.,out));
    EXPECT_NEAR(out,-(1.-std::exp(-1.)),1e-15);
    EXPECT_NEAR(pid.MeasurementDerivative(),1.-std::exp(-1.),1e-15);
}
TEST(PID, InvalidUpdatesPreserveHistoryIntegralAndOutput) {
    PID<double> pid; PidParameters<double> p;
    p.kp=0;p.kd=1;p.ki=0;p.outputMin=-10;p.outputMax=10;
    ASSERT_TRUE(pid.Configure(p)); double out=0;
    ASSERT_TRUE(pid.Update(0.,1.,1.,out));
    out=42;
    EXPECT_FALSE(pid.Update(0.,5.,0.,out)); EXPECT_EQ(out,42.); EXPECT_EQ(pid.Output(),0.);
    EXPECT_FALSE(pid.Update(std::numeric_limits<double>::infinity(),5.,1.,out)); EXPECT_EQ(out,42.);
    ASSERT_TRUE(pid.Update(0.,2.,1.,out)); EXPECT_EQ(out,-1.);
    p.outputMin=10;p.outputMax=-10;
    EXPECT_FALSE(pid.Configure(p)); EXPECT_EQ(pid.Parameters().outputMin,-10.);
}
TEST(PID, ConfigureClampsStoredIntegralAndOutputToNewLimits) {
    PID<double> pid; PidParameters<double> p;
    p.outputMin=-10;p.outputMax=10;
    ASSERT_TRUE(pid.Configure(p)); ASSERT_TRUE(pid.Reset(8.));
    p.outputMin=-1;p.outputMax=1;
    ASSERT_TRUE(pid.Configure(p)); EXPECT_EQ(pid.Output(),1.); EXPECT_EQ(pid.Integral(),1.);
    EXPECT_FALSE(pid.Reset(std::numeric_limits<double>::quiet_NaN())); EXPECT_EQ(pid.Integral(),1.);
}
TEST(PID, ArithmeticOverflowIsRejectedWithoutStateMutation) {
    PID<float> pid; PidParameters<float> p;
    p.kp=std::numeric_limits<float>::max();
    ASSERT_TRUE(pid.Configure(p)); float out=99;
    EXPECT_FALSE(pid.Update(2.f,0.f,1.f,out));
    EXPECT_EQ(out,99); EXPECT_EQ(pid.Output(),0); EXPECT_EQ(pid.Integral(),0);
}
TEST(PID, FeedbackLoopConvergesOnSimpleFirstOrderPlant) {
    PID<double> pid; PidParameters<double> p;
    p.kp=2;p.ki=1;p.outputMin=0;p.outputMax=2;
    ASSERT_TRUE(pid.Configure(p));
    double measurement=0,control=0;
    for(int i=0;i<3000;++i) {
        ASSERT_TRUE(pid.Update(1.,measurement,0.01,control));
        measurement+=0.01*(control-measurement);
    }
    EXPECT_NEAR(measurement,1.,1e-5);
    EXPECT_GE(control,0.); EXPECT_LE(control,2.);
}
