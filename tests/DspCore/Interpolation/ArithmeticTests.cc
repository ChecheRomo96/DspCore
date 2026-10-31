#include <DspCore_Interpolation.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::Interpolation;

TEST(Interpolation, NormalizedLinearEndpointsExtrapolationAndDoublePrecision) {
    EXPECT_EQ(NormalizedLinear(2.0, 8.0, 0.0), 2.0);
    EXPECT_EQ(NormalizedLinear(2.0, 8.0, 1.0), 8.0);
    EXPECT_EQ(NormalizedLinear(2.0, 8.0, 0.25), 3.5);
    EXPECT_EQ(NormalizedLinear(2.0, 8.0, 2.0), 14.0);
    const double delta = std::numeric_limits<double>::epsilon() * 8;
    EXPECT_EQ(NormalizedLinear(1.0, 1.0 + delta, 0.5), 1.0 + delta / 2);
    const auto maximum = std::numeric_limits<double>::max();
    EXPECT_EQ(NormalizedLinear(-maximum, maximum, 0.5), 0.0);
    EXPECT_EQ(NormalizedLinear(maximum / 2, maximum, 1.0), maximum);
}
TEST(Interpolation, NormalizedBilinearReproducesAffinePlaneAndCornerOrder) {
    // f(x,y)=3+2*x+4*y, not symmetric under an axis swap.
    EXPECT_DOUBLE_EQ(NormalizedBilinear(3.0, 5.0, 7.0, 9.0, 0.25, 0.75), 6.5);
    EXPECT_DOUBLE_EQ(NormalizedBilinear(3.0, 5.0, 7.0, 9.0, 1.0, 0.0), 5.0);
    EXPECT_DOUBLE_EQ(NormalizedBilinear(3.0, 5.0, 7.0, 9.0, 0.0, 1.0), 7.0);
    EXPECT_DOUBLE_EQ(NormalizedBilinear(3.0, 5.0, 7.0, 9.0, 2.0, -1.0), 3.0);
}
TEST(Interpolation, NormalizedTrilinearReproducesAffineVolumeAndCorners) {
    // f(x,y,z)=1+2*x+4*y+8*z.
    EXPECT_DOUBLE_EQ(NormalizedTrilinear(1.,3.,5.,7.,9.,11.,13.,15.,0.25,0.5,0.75), 9.5);
    EXPECT_DOUBLE_EQ(NormalizedTrilinear(1.,3.,5.,7.,9.,11.,13.,15.,1.,0.,1.), 11.);
}
TEST(Interpolation, PhysicalCoordinateFunctionsSupportReversedAndCollapsedAxes) {
    EXPECT_DOUBLE_EQ(LinearAt(10.,20.,2.,8.,15.), 5.);
    EXPECT_DOUBLE_EQ(LinearAt(20.,10.,2.,8.,15.), 5.);
    EXPECT_DOUBLE_EQ(LinearAt(2.,2.,5.,9.,-100.), 5.);
    EXPECT_DOUBLE_EQ(BilinearAt(10.,20.,-1.,1.,3.,5.,7.,9.,12.5,0.5), 6.5);
    EXPECT_DOUBLE_EQ(BilinearAt(0.,0.,0.,2.,3.,5.,7.,9.,100.,1.), 5.);
    EXPECT_DOUBLE_EQ(TrilinearAt(10.,20.,0.,2.,-2.,2.,1.,3.,5.,7.,9.,11.,13.,15.,12.5,1.,1.), 9.5);
    EXPECT_DOUBLE_EQ(TrilinearAt(0.,1.,0.,1.,2.,2.,1.,3.,5.,7.,9.,11.,13.,15.,1.,1.,99.), 7.);
}
TEST(Interpolation, NonFiniteSamplesPropagateWithoutChangingCoordinateContract) {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(std::isnan(NormalizedLinear(nan,1.f,0.5f)));
    EXPECT_EQ(NormalizedLinear(2.f,nan,0.f), 2.f);
    EXPECT_TRUE(std::isnan(LinearAt(0.f,1.f,nan,1.f,0.5f)));
}
