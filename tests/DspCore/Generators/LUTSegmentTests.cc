#include <DspCore_Generators.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <limits>
using DspCore::Generators::Envelopes::LUTSegment;

TEST(LUTEnvelope, NormalizedShapeMapsBothEndpointsAndSubrange) {
    DspCore::LUT::LUT1D<double> shape;
    ASSERT_TRUE(shape.Resize(3));
    shape.SetValue(0,0.); shape.SetValue(1,0.25); shape.SetValue(2,1.);
    double out[7]; std::fill(out,out+7,99.);
    ASSERT_TRUE(LUTSegment(out,2.,10.,7,shape,1,6));
    EXPECT_EQ(out[0],99.); EXPECT_EQ(out[1],2.); EXPECT_EQ(out[2],3.);
    EXPECT_EQ(out[3],4.); EXPECT_EQ(out[4],7.); EXPECT_EQ(out[5],10.); EXPECT_EQ(out[6],99.);
}
TEST(LUTEnvelope, SingletonTableFillsTheWholeSegmentWithAConstant) {
    DspCore::LUT::LUT1D<float> shape;
    ASSERT_TRUE(shape.Resize(1)); shape.Fill(0.25f);
    float out[5]={99,99,99,99,99};
    ASSERT_TRUE(LUTSegment(out,2.f,10.f,5,shape));
    for(float x:out) EXPECT_EQ(x,4);
    ASSERT_TRUE(LUTSegment(out,2.f,10.f,5,shape,1,2)); EXPECT_EQ(out[1],2);
}
TEST(LUTEnvelope, InvalidTablesAndOverlapAreRejectedBeforeWriting) {
    DspCore::LUT::LUT1D<float> shape;
    float out[3]={9,9,9};
    EXPECT_FALSE(LUTSegment(out,0.f,1.f,3,shape));
    ASSERT_TRUE(shape.Resize(3));
    shape.SetValue(0,0); shape.SetValue(1,2); shape.SetValue(2,1);
    EXPECT_FALSE(LUTSegment(out,0.f,1.f,3,shape));
    shape.SetValue(1,std::numeric_limits<float>::quiet_NaN());
    EXPECT_FALSE(LUTSegment(out,0.f,1.f,3,shape));
    for(float x:out) EXPECT_EQ(x,9);
    shape.SetValue(1,0.5f);
    EXPECT_FALSE(LUTSegment(shape.GetBuffer(),0.f,1.f,3,shape));
    EXPECT_EQ(shape.GetValue(1),0.5f);
    EXPECT_TRUE(LUTSegment(static_cast<float*>(nullptr),0.f,1.f,0,shape));
}
TEST(LUTEnvelope, GeneratorsFillPublicOneTwoAndThreeDimensionalStorage) {
    DspCore::LUT::LUT1D<float> line;
    DspCore::LUT::LUT2D<float> plane;
    DspCore::LUT::LUT3D<float> volume;
    ASSERT_TRUE(line.Resize(5)); ASSERT_TRUE(plane.Resize(5,2)); ASSERT_TRUE(volume.Resize(5,2,2));
    ASSERT_TRUE(DspCore::Generators::Shaping::Gamma(line.GetBuffer(),line.Size(),2.f));
    for(size_t y=0;y<2;++y) {
        ASSERT_TRUE(DspCore::Generators::Shaping::Gamma(plane.GetBuffer()+y*5,5,static_cast<float>(y+1)));
        for(size_t z=0;z<2;++z) {
            ASSERT_TRUE(DspCore::Generators::Shaping::GammaMorph(
                volume.GetBuffer()+(z*2+y)*5,5,static_cast<float>(y),static_cast<float>(z+1)));
        }
    }
    EXPECT_FLOAT_EQ(line.GetValue(2),0.25f);
    EXPECT_FLOAT_EQ(plane.GetValue(2,0),0.5f); EXPECT_FLOAT_EQ(plane.GetValue(2,1),0.25f);
    EXPECT_FLOAT_EQ(volume.GetValue(2,1,0),0.5f); EXPECT_FLOAT_EQ(volume.GetValue(2,1,1),0.25f);
}
