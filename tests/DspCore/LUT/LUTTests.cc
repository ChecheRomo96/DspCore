#include <DspCore.h>
#include <gtest/gtest.h>
#include <limits>
#include <type_traits>
#include <utility>

using DspCore::LUT::LUT1D;
using DspCore::LUT::LUT2D;
using DspCore::LUT::LUT3D;

static_assert(!std::is_copy_constructible<LUT1D<float>>::value, "owning LUT must not copy");
static_assert(!std::is_copy_assignable<LUT3D<double>>::value, "owning LUT must not copy");
static_assert(std::is_nothrow_move_constructible<LUT2D<int>>::value, "moves must not throw");
static_assert(!std::is_polymorphic<LUT3D<float>>::value, "storage must not invoke virtual generators");

TEST(LUTStorage, DefaultStateIsEmptyAndBoundsAreChecked) {
    LUT1D<float> table;
    EXPECT_EQ(table.GetBuffer(), nullptr);
    EXPECT_EQ(table.Size(), 0U);
    EXPECT_EQ(table.Capacity(), 0U);
    float sentinel = 42;
    EXPECT_FALSE(table.TryGet(0, sentinel));
    EXPECT_EQ(sentinel, 42);
    EXPECT_FALSE(table.SetValue(0, 1));
    EXPECT_EQ(table.GetValue(0), 0);
    EXPECT_TRUE(table.Resize(0));
    table.Release();
    table.Release();
}

TEST(LUTStorage, ReserveAndResizePreserveValuesAndZeroNewLogicalElements) {
    LUT1D<int> table;
    ASSERT_TRUE(table.Reserve(8));
    auto* pointer = table.GetBuffer();
    EXPECT_EQ(table.Size(), 0U);
    ASSERT_TRUE(table.Resize(4));
    EXPECT_EQ(pointer, table.GetBuffer());
    for (size_t i = 0; i < 4; ++i) EXPECT_EQ(table.GetValue(i), 0);
    table.Fill(7);
    ASSERT_TRUE(table.Resize(2));
    ASSERT_TRUE(table.Resize(5));
    EXPECT_EQ(table.GetValue(0), 7);
    EXPECT_EQ(table.GetValue(1), 7);
    for (size_t i = 2; i < 5; ++i) EXPECT_EQ(table.GetValue(i), 0);
    ASSERT_TRUE(table.Reserve(16));
    EXPECT_EQ(table.Size(), 5U);
    EXPECT_EQ(table.GetValue(1), 7);
    EXPECT_TRUE(table.Resize(0));
    EXPECT_EQ(table.Capacity(), 16U);
    EXPECT_NE(table.GetBuffer(), nullptr);
    ASSERT_TRUE(table.Resize(2));
    EXPECT_EQ(table.GetValue(0), 0);
    table.Release();
    EXPECT_EQ(table.GetBuffer(), nullptr);
    EXPECT_EQ(table.Width(), 0U);
    EXPECT_EQ(table.Capacity(), 0U);
}

TEST(LUTStorage, TwoDimensionsUseRowMajorCoordinatesAndPreserveFlatPrefix) {
    LUT2D<int> table;
    ASSERT_TRUE(table.Resize(3, 2));
    for (size_t i = 0; i < 6; ++i) table.GetBuffer()[i] = static_cast<int>(i + 10);
    EXPECT_EQ(table.GetValue(2, 1), 15);
    ASSERT_TRUE(table.SetValue(1, 1, 99));
    EXPECT_EQ(table.GetBuffer()[4], 99);
    int value = -1;
    EXPECT_FALSE(table.TryGet(3, 0, value));
    EXPECT_EQ(value, -1);
    EXPECT_FALSE(table.SetValue(0, 2, 5));
    ASSERT_TRUE(table.Resize(2, 3));
    EXPECT_EQ(table.GetValue(0, 2), 99);
    EXPECT_EQ(table.Size(), 6U);
    ASSERT_TRUE(table.Resize(0, std::numeric_limits<size_t>::max()));
    EXPECT_EQ(table.Width(), 0U);
    EXPECT_EQ(table.Height(), 0U);
}

TEST(LUTStorage, ThreeDimensionsUseRowMajorCoordinatesAndCanonicalEmptyShapes) {
    LUT3D<double> table;
    ASSERT_TRUE(table.Resize(2, 3, 4));
    ASSERT_TRUE(table.SetValue(1, 2, 3, 0.125));
    EXPECT_DOUBLE_EQ(table.GetBuffer()[23], 0.125);
    double sample = 0;
    ASSERT_TRUE(table.TryGet(1, 2, 3, sample));
    EXPECT_DOUBLE_EQ(sample, 0.125);
    EXPECT_FALSE(table.SetValue(2, 0, 0, 8));
    EXPECT_FALSE(table.SetValue(0, 3, 0, 8));
    EXPECT_FALSE(table.SetValue(0, 0, 4, 8));
    EXPECT_FALSE(table.TryGet(0, 0, 4, sample));
    EXPECT_DOUBLE_EQ(sample, 0.125);
    EXPECT_TRUE(table.Resize(2, 0, 4));
    EXPECT_EQ(table.Width(), 0U);
    EXPECT_EQ(table.Height(), 0U);
    EXPECT_EQ(table.Depth(), 0U);
    EXPECT_EQ(table.Capacity(), 24U);
    // Regression: legacy LUT3D freed storage but retained a dangling pointer.
    table.Release();
    table.Release();
    ASSERT_TRUE(table.Resize(1, 1, 1));
    EXPECT_DOUBLE_EQ(table.GetValue(0, 0, 0), 0);
}

TEST(LUTStorage, OverflowRejectionPreservesPointerShapeAndSamples) {
    const auto maximum = std::numeric_limits<size_t>::max();
    LUT1D<double> line;
    ASSERT_TRUE(line.Resize(2));
    line.Fill(0.5);
    auto* linePointer = line.GetBuffer();
    EXPECT_FALSE(line.Resize(maximum / sizeof(double) + 1));
    EXPECT_FALSE(line.Reserve(maximum));
    EXPECT_EQ(line.GetBuffer(), linePointer);
    EXPECT_EQ(line.Size(), 2U);
    EXPECT_DOUBLE_EQ(line.GetValue(1), 0.5);
    LUT2D<double> plane;
    ASSERT_TRUE(plane.Resize(2, 3));
    plane.Fill(0.75);
    auto* planePointer = plane.GetBuffer();
    EXPECT_FALSE(plane.Resize(maximum / 2 + 1, 2));
    EXPECT_FALSE(plane.Resize(maximum / sizeof(double) + 1, 1));
    EXPECT_EQ(plane.GetBuffer(), planePointer);
    EXPECT_EQ(plane.Width(), 2U);
    EXPECT_EQ(plane.Height(), 3U);
    EXPECT_EQ(plane.Capacity(), 6U);
    EXPECT_DOUBLE_EQ(plane.GetValue(1, 2), 0.75);
    LUT3D<double> cube;
    ASSERT_TRUE(cube.Resize(2, 3, 4));
    cube.Fill(0.25);
    auto* cubePointer = cube.GetBuffer();
    EXPECT_FALSE(cube.Resize(maximum, 2, 1));
    EXPECT_FALSE(cube.Resize(maximum / 2 + 1, 1, 2));
    EXPECT_FALSE(cube.Resize(maximum / sizeof(double) + 1, 1, 1));
    EXPECT_EQ(cube.GetBuffer(), cubePointer);
    EXPECT_EQ(cube.Width(), 2U);
    EXPECT_EQ(cube.Height(), 3U);
    EXPECT_EQ(cube.Depth(), 4U);
    EXPECT_DOUBLE_EQ(cube.GetValue(1, 2, 3), 0.25);
}

TEST(LUTStorage, MoveTransfersOwnershipAndClearsEverySourceExtent) {
    LUT3D<float> source;
    ASSERT_TRUE(source.Resize(2, 3, 4));
    source.Fill(0.5f);
    auto* pointer = source.GetBuffer();
    LUT3D<float> destination(std::move(source));
    EXPECT_EQ(destination.GetBuffer(), pointer);
    EXPECT_EQ(destination.Width(), 2U);
    EXPECT_EQ(destination.Height(), 3U);
    EXPECT_EQ(destination.Depth(), 4U);
    EXPECT_EQ(source.GetBuffer(), nullptr);
    EXPECT_EQ(source.Width(), 0U);
    EXPECT_EQ(source.Height(), 0U);
    EXPECT_EQ(source.Depth(), 0U);
    EXPECT_EQ(source.Capacity(), 0U);
    ASSERT_TRUE(source.Resize(1, 1, 1));
    source = std::move(destination);
    EXPECT_EQ(source.GetBuffer(), pointer);
    EXPECT_EQ(destination.Size(), 0U);
    EXPECT_EQ(destination.Width(), 0U);
    EXPECT_EQ(destination.Height(), 0U);
    EXPECT_EQ(destination.Depth(), 0U);
    EXPECT_FLOAT_EQ(source.GetValue(1, 2, 3), 0.5f);
    auto* sameObject = &source;
    source = std::move(*sameObject);
    EXPECT_EQ(source.GetBuffer(), pointer);
}

TEST(LUTStorage, OneAndTwoDimensionalMovesLeaveReusableSources) {
    LUT1D<int> line;
    ASSERT_TRUE(line.Resize(3));
    line.Fill(11);
    LUT1D<int> moved(std::move(line));
    EXPECT_EQ(line.Width(), 0U);
    line = std::move(moved);
    EXPECT_EQ(moved.Width(), 0U);
    EXPECT_EQ(line.GetValue(2), 11);
    LUT2D<int> plane;
    ASSERT_TRUE(plane.Resize(2, 3));
    plane.Fill(12);
    LUT2D<int> movedPlane(std::move(plane));
    EXPECT_EQ(plane.Width(), 0U);
    EXPECT_EQ(plane.Height(), 0U);
    EXPECT_EQ(movedPlane.Width(), 2U);
    EXPECT_EQ(movedPlane.Height(), 3U);
    plane = std::move(movedPlane);
    EXPECT_EQ(movedPlane.Width(), 0U);
    EXPECT_EQ(movedPlane.Height(), 0U);
    EXPECT_EQ(plane.GetValue(1, 2), 12);
}

TEST(LUTStorage, RawAndConstAccessPreserveDoublePrecision) {
    LUT1D<double> table;
    ASSERT_TRUE(table.Resize(2));
    const double sample = 1.0 + std::numeric_limits<double>::epsilon();
    table.GetBuffer()[1] = sample;
    const auto& readOnly = table;
    static_assert(std::is_same<decltype(readOnly.GetBuffer()), const double*>::value,
                  "const tables must not expose mutable storage");
    EXPECT_DOUBLE_EQ(readOnly.GetValue(1), sample);
    double value = 0;
    ASSERT_TRUE(readOnly.TryGet(1, value));
    EXPECT_EQ(value, sample);
    EXPECT_NE(value, 1.0);
}
