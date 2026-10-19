#include <gtest/gtest.h>
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <utility/CPSTL_allocator.h>
#include <stddef.h>
#include <stdlib.h>

namespace {
bool failAllocation = false;
void* AuditMalloc(size_t bytes) {
    return failAllocation ? nullptr : malloc(bytes);
}
}
// Isolated executable: exercise the documented C allocation failure path
// without exhausting memory or adding a test hook to the public API.
#define malloc AuditMalloc
#include <DspCore_LUT.h>
#undef malloc

TEST(LUTAllocation, FailurePreservesEmptyAndPopulatedTables) {
    DspCore::LUT::LUT1D<float> line;
    DspCore::LUT::LUT2D<int> surface;
    DspCore::LUT::LUT3D<double> volume;
    failAllocation = true;
    EXPECT_FALSE(line.Resize(2));
    EXPECT_FALSE(surface.Resize(2, 3));
    EXPECT_FALSE(volume.Resize(2, 3, 4));
    EXPECT_EQ(line.GetBuffer(), nullptr);
    EXPECT_EQ(surface.Height(), 0U);
    EXPECT_EQ(volume.Depth(), 0U);
    failAllocation = false;
    ASSERT_TRUE(line.Resize(2));
    ASSERT_TRUE(surface.Resize(2, 3));
    ASSERT_TRUE(volume.Resize(2, 3, 4));
    line.Fill(0.5f);
    surface.Fill(7);
    volume.Fill(0.125);
    auto* linePointer = line.GetBuffer();
    auto* surfacePointer = surface.GetBuffer();
    auto* volumePointer = volume.GetBuffer();
    failAllocation = true;
    EXPECT_FALSE(line.Reserve(16));
    EXPECT_FALSE(line.Resize(8));
    EXPECT_FALSE(surface.Resize(8, 8));
    EXPECT_FALSE(volume.Resize(8, 8, 8));
    // Operations inside existing capacity still work while allocation fails.
    EXPECT_TRUE(line.Resize(1));
    EXPECT_TRUE(line.Resize(2));
    failAllocation = false;
    EXPECT_EQ(line.GetBuffer(), linePointer);
    EXPECT_EQ(surface.GetBuffer(), surfacePointer);
    EXPECT_EQ(volume.GetBuffer(), volumePointer);
    EXPECT_EQ(line.Capacity(), 2U);
    EXPECT_EQ(surface.Width(), 2U);
    EXPECT_EQ(surface.Height(), 3U);
    EXPECT_EQ(surface.Capacity(), 6U);
    EXPECT_EQ(volume.Width(), 2U);
    EXPECT_EQ(volume.Height(), 3U);
    EXPECT_EQ(volume.Depth(), 4U);
    EXPECT_EQ(volume.Capacity(), 24U);
    EXPECT_FLOAT_EQ(line.GetValue(0), 0.5f);
    EXPECT_EQ(surface.GetValue(1, 2), 7);
    EXPECT_DOUBLE_EQ(volume.GetValue(1, 2, 3), 0.125);
}
