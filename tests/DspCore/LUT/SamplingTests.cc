#include <DspCore_LUT.h>
#include <gtest/gtest.h>
#include <limits>
#include <cmath>
using namespace DspCore::LUT;

TEST(Sampling, ClosedDomainInterpolatesClampsAndRejects) {
    DspCore::LUT::LUT1D<int> table;
    ASSERT_TRUE(table.Resize(3));
    table.SetValue(0,0); table.SetValue(1,10); table.SetValue(2,30);
    double result = -9;
    ASSERT_TRUE(Sample(table,0.25,result,Boundary::Reject)); EXPECT_EQ(result,5.);
    ASSERT_TRUE(Sample(table,1.,result,Boundary::Reject)); EXPECT_EQ(result,30.);
    ASSERT_TRUE(Sample(table,-2.,result,Boundary::Clamp)); EXPECT_EQ(result,0.);
    ASSERT_TRUE(Sample(table,2.,result,Boundary::Clamp)); EXPECT_EQ(result,30.);
    EXPECT_FALSE(Sample(table,2.,result,Boundary::Reject)); EXPECT_EQ(result,30.);
}
TEST(Sampling, PeriodicDomainInterpolatesAcrossSeamAndWrapsNegativeCycles) {
    DspCore::LUT::LUT1D<float> table;
    ASSERT_TRUE(table.Resize(4));
    table.SetValue(0,0); table.SetValue(1,10); table.SetValue(2,20); table.SetValue(3,30);
    float result = -1;
    ASSERT_TRUE(Sample(table,0.875f,result,Boundary::Wrap)); EXPECT_FLOAT_EQ(result,15);
    ASSERT_TRUE(Sample(table,-0.125f,result,Boundary::Wrap)); EXPECT_FLOAT_EQ(result,15);
    ASSERT_TRUE(Sample(table,1.f,result,Boundary::Wrap)); EXPECT_FLOAT_EQ(result,0);
    ASSERT_TRUE(Sample(table,1000.25f,result,Boundary::Wrap)); EXPECT_FLOAT_EQ(result,10);
    ASSERT_TRUE(Sample(table,0.5f,result,Boundary::Clamp)); EXPECT_FLOAT_EQ(result,15);
    ASSERT_TRUE(Sample(table,0.5f,result,Boundary::Wrap)); EXPECT_FLOAT_EQ(result,20);
}
TEST(Sampling, BilinearAndTrilinearReproduceAsymmetricFields) {
    DspCore::LUT::LUT2D<double> plane;
    ASSERT_TRUE(plane.Resize(3,4));
    for (size_t y=0;y<4;++y) for (size_t x=0;x<3;++x)
        plane.SetValue(x,y,3.+2.*static_cast<double>(x)/2.+4.*static_cast<double>(y)/3.);
    double result = 0;
    ASSERT_TRUE(Sample(plane,0.25,0.75,result,Boundary::Reject));
    EXPECT_NEAR(result,6.5,1e-14);
    DspCore::LUT::LUT3D<double> volume;
    ASSERT_TRUE(volume.Resize(3,4,5));
    for (size_t z=0;z<5;++z) for (size_t y=0;y<4;++y) for (size_t x=0;x<3;++x)
        volume.SetValue(x,y,z,1.+static_cast<double>(x)+4.*static_cast<double>(y)/3.+2.*static_cast<double>(z));
    ASSERT_TRUE(Sample(volume,0.25,0.5,0.75,result,Boundary::Reject));
    EXPECT_NEAR(result,9.5,1e-14);
    ASSERT_TRUE(Sample(volume,1.,1.,1.,result,Boundary::Clamp));
    EXPECT_EQ(result,15.);
}
TEST(Sampling, SingletonAxesAreConstantButStillValidateCoordinates) {
    DspCore::LUT::LUT1D<double> point;
    ASSERT_TRUE(point.Resize(1)); point.Fill(7);
    double result = 99;
    ASSERT_TRUE(Sample(point,-100.,result,Boundary::Wrap)); EXPECT_EQ(result,7.);
    EXPECT_FALSE(Sample(point,-1.,result,Boundary::Reject)); EXPECT_EQ(result,7.);
    DspCore::LUT::LUT2D<int> line;
    ASSERT_TRUE(line.Resize(1,2)); line.SetValue(0,0,2); line.SetValue(0,1,6);
    ASSERT_TRUE(Sample(line,0.7,0.25,result,Boundary::Reject)); EXPECT_EQ(result,3.);
    DspCore::LUT::LUT3D<int> flat;
    ASSERT_TRUE(flat.Resize(2,1,1)); flat.SetValue(0,0,0,10); flat.SetValue(1,0,0,20);
    ASSERT_TRUE(Sample(flat,0.5,0.2,0.8,result,Boundary::Reject)); EXPECT_EQ(result,15.);
}
TEST(Sampling, InvalidInputNeverChangesOutputOrCastsInvalidIndices) {
    DspCore::LUT::LUT1D<double> table;
    double result = 123;
    EXPECT_FALSE(Sample(table,0.,result,Boundary::Clamp));
    ASSERT_TRUE(table.Resize(2)); table.Fill(1.);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    EXPECT_FALSE(Sample(table,nan,result,Boundary::Clamp));
    EXPECT_FALSE(Sample(table,infinity,result,Boundary::Wrap));
    EXPECT_FALSE(Sample(table,-infinity,result,Boundary::Reject));
    EXPECT_FALSE(Sample(table,0.,result,static_cast<Boundary>(99)));
    EXPECT_EQ(result,123.);
    const double maximum = std::numeric_limits<double>::max();
    ASSERT_TRUE(Sample(table,maximum,result,Boundary::Clamp)); EXPECT_EQ(result,1.);
    ASSERT_TRUE(Sample(table,-maximum,result,Boundary::Wrap)); EXPECT_EQ(result,1.);
}
TEST(Sampling, MultiAxisRejectionPreservesOutputAndPeriodicSeamsWork) {
    DspCore::LUT::LUT2D<int> plane;
    ASSERT_TRUE(plane.Resize(2,2));
    plane.SetValue(0,0,0); plane.SetValue(1,0,2); plane.SetValue(0,1,4); plane.SetValue(1,1,6);
    double result = 77;
    EXPECT_FALSE(Sample(plane,0.5,1.1,result,Boundary::Reject)); EXPECT_EQ(result,77.);
    ASSERT_TRUE(Sample(plane,0.75,0.75,result,Boundary::Wrap)); EXPECT_EQ(result,3.);
    DspCore::LUT::LUT3D<int> volume;
    ASSERT_TRUE(volume.Resize(2,2,2));
    for(size_t z=0;z<2;++z) for(size_t y=0;y<2;++y) for(size_t x=0;x<2;++x)
        volume.SetValue(x,y,z,static_cast<int>(2*x+4*y+8*z));
    EXPECT_FALSE(Sample(volume,0.5,0.5,1.1,result,Boundary::Reject)); EXPECT_EQ(result,3.);
    ASSERT_TRUE(Sample(volume,0.75,0.75,0.75,result,Boundary::Wrap)); EXPECT_EQ(result,7.);
}

TEST(Sampling, NarrowingStorageOverflowHasDefinedNonFiniteOutput) {
    DspCore::LUT::LUT1D<double> table;
    ASSERT_TRUE(table.Resize(1));
    table.Fill(std::numeric_limits<double>::max());
    float result = 0;
    ASSERT_TRUE(Sample(table,0.f,result,Boundary::Clamp));
    EXPECT_EQ(result,std::numeric_limits<float>::infinity());
    table.Fill(-std::numeric_limits<double>::max());
    ASSERT_TRUE(Sample(table,0.f,result,Boundary::Clamp));
    EXPECT_EQ(result,-std::numeric_limits<float>::infinity());
}

TEST(Sampling, CoordinatesAdjacentToEndpointsStayWithinStorage) {
    DspCore::LUT::LUT1D<float> table;
    ASSERT_TRUE(table.Resize(4));
    for (size_t i = 0; i < 4; ++i) table.SetValue(i, static_cast<float>(i));
    float output = 0;
    const float belowOne = std::nextafter(1.f, 0.f);
    ASSERT_TRUE(Sample(table,belowOne,output,Boundary::Reject));
    EXPECT_NEAR(output,3.f,1e-5f);
    ASSERT_TRUE(Sample(table,belowOne,output,Boundary::Wrap));
    EXPECT_NEAR(output,0.f,1e-5f);
    ASSERT_TRUE(Sample(table,-std::numeric_limits<float>::denorm_min(),output,Boundary::Wrap));
    EXPECT_FLOAT_EQ(output,0.f);
}
