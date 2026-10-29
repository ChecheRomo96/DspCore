#include <DspCore/Processing/SampleBlock.h>
#include <gtest/gtest.h>

namespace {

struct UserDefinedSample {
    int value{};
};

TEST(SampleBlock, BorrowsExternalMemoryWithoutCopying) {
    float samples[] = {0.25F, 0.5F};
    const auto block = DspCore::Processing::MakeInputBlock(samples, 2U);
    static_assert(cpstd::is_same<typename decltype(block)::Type, const float>::value,
        "Input blocks preserve their const sample type");

    ASSERT_TRUE(block.IsValid());
    EXPECT_EQ(block.size, 2U);
    EXPECT_FLOAT_EQ(block.data[0], 0.25F);
    samples[0] = 0.75F;
    EXPECT_FLOAT_EQ(block.data[0], 0.75F);
}

TEST(SampleBlock, DirectConstructionLetsTheMemoryOwnerRetainItsViews) {
    float inputSamples[] = {0.25F, 0.5F};
    float outputSamples[2]{};
    const DspCore::Processing::SampleBlock<const float> input{inputSamples, 2U};
    const DspCore::Processing::SampleBlock<float> output{outputSamples, 2U};

    ASSERT_TRUE(input.IsValid());
    ASSERT_TRUE(output.IsValid());
    EXPECT_EQ(input.data, inputSamples);
    EXPECT_EQ(output.data, outputSamples);
}

TEST(SampleBlock, TreatsAnEmptyNullViewAsValidButRejectsNonEmptyNullMemory) {
    const DspCore::Processing::SampleBlock<const float> empty{};
    const DspCore::Processing::SampleBlock<const float> invalid{nullptr, 1U};

    EXPECT_TRUE(empty.IsValid());
    EXPECT_FALSE(invalid.IsValid());
}

TEST(SampleBlock, RepresentsUserDefinedElementsWithoutRegistrationOrOwnership) {
    UserDefinedSample samples[] = {{3}, {-2}};
    const DspCore::Processing::SampleBlock<UserDefinedSample> block{samples, 2U};

    ASSERT_TRUE(block.IsValid());
    EXPECT_EQ(block.data[0].value, 3);
    block.data[1].value = 4;
    EXPECT_EQ(samples[1].value, 4);
}

} // namespace
