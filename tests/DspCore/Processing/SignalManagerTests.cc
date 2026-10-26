#include <DspCore/Processing/SignalManager.h>
#include <gtest/gtest.h>

namespace {

struct UserDefinedSample {
    int value{};
};

struct DifferentUserDefinedSample {
    int value{};
};

TEST(SignalBlock, BorrowsExternalMemoryWithoutCopying) {
    float samples[] = {0.25F, 0.5F};
    auto block = DspCore::Processing::MakeInputSignalBlock(samples, 2U);
    static_assert(cpstd::is_same<typename decltype(block)::Type, const float>::value,
        "Input blocks preserve their const sample type");

    ASSERT_TRUE(block.IsValid());
    EXPECT_EQ(block.Size(), 2U);
    EXPECT_EQ(block.Capacity(), 2U);
    EXPECT_FLOAT_EQ(block.Data()[0], 0.25F);
    samples[0] = 0.75F;
    EXPECT_FLOAT_EQ(block.Data()[0], 0.75F);
}

TEST(SignalManager, ExposesOrderedHeterogeneousInputBlocks) {
    const float audio[] = {0.5F, 1.5F};
    const short control[] = {2, -1};
    auto audioBlock = DspCore::Processing::MakeInputSignalBlock(audio, 2U);
    auto controlBlock = DspCore::Processing::MakeInputSignalBlock(control, 2U);
    DspCore::Processing::SignalBlockBase* blocks[] = {&audioBlock, &controlBlock};
    const DspCore::Processing::SignalManager inputs{blocks, 2U};

    ASSERT_TRUE(inputs.IsValid());
    ASSERT_NE(inputs.Read<float>(0U), nullptr);
    ASSERT_NE(inputs.Read<short>(1U), nullptr);
    EXPECT_EQ(inputs.Read<float>(0U)->Data()[1], 1.5F);
    EXPECT_EQ(inputs.Read<short>(1U)->Data()[0], 2);
    EXPECT_EQ(inputs.Read<short>(0U), nullptr);
    EXPECT_EQ(inputs.Read<float>(2U), nullptr);
}

TEST(SignalManager, AcceptsUserDefinedSampleTypesWithoutRegistration) {
    const UserDefinedSample source[] = {{3}, {-2}};
    auto sourceBlock = DspCore::Processing::MakeInputSignalBlock(source, 2U);
    DspCore::Processing::SignalBlockBase* blocks[] = {&sourceBlock};
    const DspCore::Processing::SignalManager inputs{blocks, 1U};

    const auto* typed = inputs.Read<UserDefinedSample>(0U);
    ASSERT_NE(typed, nullptr);
    EXPECT_EQ(typed->Data()[0].value, 3);
    EXPECT_EQ(inputs.Read<DifferentUserDefinedSample>(0U), nullptr);
}

TEST(SignalManager, CommitsWritableOutputBlocksWithoutOwningTheirMemory) {
    float result[3]{};
    auto resultBlock = DspCore::Processing::MakeOutputSignalBlock(result, 3U);
    DspCore::Processing::SignalBlockBase* blocks[] = {&resultBlock};
    DspCore::Processing::SignalManager outputs{blocks, 1U};

    ASSERT_TRUE(outputs.IsValid());
    auto* writable = outputs.Write<float>(0U);
    ASSERT_NE(writable, nullptr);
    writable->Data()[0] = 0.25F;
    writable->Data()[1] = 0.75F;
    ASSERT_TRUE(outputs.Commit(0U, 2U));
    EXPECT_EQ(resultBlock.Size(), 2U);
    EXPECT_FLOAT_EQ(result[0], 0.25F);
    EXPECT_FLOAT_EQ(result[1], 0.75F);
    EXPECT_FALSE(outputs.Commit(0U, 4U));
    EXPECT_EQ(resultBlock.Size(), 2U);
    EXPECT_EQ(outputs.Write<short>(0U), nullptr);
}

TEST(SignalManager, KeepsInputAndOutputAccessSeparate) {
    const float input[] = {1.0F};
    auto inputBlock = DspCore::Processing::MakeInputSignalBlock(input, 1U);
    DspCore::Processing::SignalBlockBase* blocks[] = {&inputBlock};
    DspCore::Processing::SignalManager manager{blocks, 1U};

    EXPECT_NE(manager.Read<float>(0U), nullptr);
    EXPECT_EQ(manager.Write<float>(0U), nullptr);
    EXPECT_FALSE(manager.Commit(0U, 1U));
}

} // namespace
