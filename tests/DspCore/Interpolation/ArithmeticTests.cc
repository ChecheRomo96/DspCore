#include <DspCore_Interpolation.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::Interpolation;

TEST(Interpolation, LinearEndpointsExtrapolationAndDoublePrecision) {
    EXPECT_EQ(Linear(2.0, 8.0, 0.0), 2.0);
    EXPECT_EQ(Linear(2.0, 8.0, 1.0), 8.0);
    EXPECT_EQ(Linear(2.0, 8.0, 0.25), 3.5);
    EXPECT_EQ(Linear(2.0, 8.0, 2.0), 14.0);
    const double delta = std::numeric_limits<double>::epsilon() * 8;
    EXPECT_EQ(Linear(1.0, 1.0 + delta, 0.5), 1.0 + delta / 2);
    const auto maximum = std::numeric_limits<double>::max();
    EXPECT_EQ(Linear(-maximum, maximum, 0.5), 0.0);
    EXPECT_EQ(Linear(maximum / 2, maximum, 1.0), maximum);
}
TEST(Interpolation, BilinearReproducesAffinePlaneAndCornerOrder) {
    // f(x,y)=3+2*x+4*y, not symmetric under an axis swap.
    EXPECT_DOUBLE_EQ(Bilinear(3.0, 5.0, 7.0, 9.0, 0.25, 0.75), 6.5);
    EXPECT_DOUBLE_EQ(Bilinear(3.0, 5.0, 7.0, 9.0, 1.0, 0.0), 5.0);
    EXPECT_DOUBLE_EQ(Bilinear(3.0, 5.0, 7.0, 9.0, 0.0, 1.0), 7.0);
    EXPECT_DOUBLE_EQ(Bilinear(3.0, 5.0, 7.0, 9.0, 2.0, -1.0), 3.0);
}
TEST(Interpolation, TrilinearReproducesAffineVolumeAndCorners) {
    // f(x,y,z)=1+2*x+4*y+8*z.
    EXPECT_DOUBLE_EQ(Trilinear(1.,3.,5.,7.,9.,11.,13.,15.,0.25,0.5,0.75), 9.5);
    EXPECT_DOUBLE_EQ(Trilinear(1.,3.,5.,7.,9.,11.,13.,15.,1.,0.,1.), 11.);
}
TEST(Interpolation, CoordinateValidationPreservesOutputOnFailure) {
    double output = 77;
    EXPECT_FALSE(TryLinear(2.,2.,0.,1.,2.,output));
    EXPECT_FALSE(TryLinear(3.,2.,0.,1.,2.,output));
    EXPECT_FALSE(TryLinear(0.,1.,0.,1.,std::numeric_limits<double>::quiet_NaN(),output));
    EXPECT_FALSE(TryLinear(0.,std::numeric_limits<double>::infinity(),0.,1.,0.,output));
    const double maximum = std::numeric_limits<double>::max();
    EXPECT_FALSE(TryLinear(-maximum,maximum,0.,1.,0.,output));
    EXPECT_EQ(output, 77);
    EXPECT_FALSE(TryBilinear(0.,1.,2.,2.,0.,1.,2.,3.,0.5,2.,output));
    EXPECT_FALSE(TryTrilinear(0.,1.,0.,1.,0.,0.,0.,1.,2.,3.,4.,5.,6.,7.,0.5,0.5,0.,output));
    EXPECT_EQ(output, 77);
}
TEST(Interpolation, PhysicalCoordinatesNormalizeAndPermitExtrapolation) {
    double result = 0;
    ASSERT_TRUE(TryLinear(10.,20.,2.,8.,15.,result));
    EXPECT_DOUBLE_EQ(result, 5.);
    ASSERT_TRUE(TryLinear(10.,20.,2.,8.,30.,result));
    EXPECT_DOUBLE_EQ(result, 14.);
    ASSERT_TRUE(TryBilinear(10.,20.,-1.,1.,3.,5.,7.,9.,12.5,0.5,result));
    EXPECT_DOUBLE_EQ(result, 6.5);
    ASSERT_TRUE(TryTrilinear(10.,20.,0.,2.,-2.,2.,1.,3.,5.,7.,9.,11.,13.,15.,12.5,1.,1.,result));
    EXPECT_DOUBLE_EQ(result, 9.5);
}
TEST(Interpolation, NonFiniteSamplesPropagateWithoutChangingCoordinateContract) {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(std::isnan(Linear(nan,1.f,0.5f)));
    EXPECT_EQ(Linear(2.f,nan,0.f), 2.f);
    float output = 0;
    EXPECT_TRUE(TryLinear(0.f,1.f,nan,1.f,0.5f,output));
    EXPECT_TRUE(std::isnan(output));
}
