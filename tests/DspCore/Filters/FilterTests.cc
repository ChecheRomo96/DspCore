#include <cmath>

#include <DspCore/Filters/Biquad.h>
#include <DspCore/Filters/FIR.h>
#include <DspCore/Filters/IIR.h>
#include <gtest/gtest.h>

namespace {

template <typename Filter, typename SampleType>
bool processSample(Filter& filter, const SampleType input, SampleType& output) {
    return filter.Process(&input, &output, 1U);
}

class HeterogeneousMimoProbe final : public DspCore::Processing::ProcessorBase {
public:
    enum Input : cpstd::size_t { Audio = 0U, Control = 1U };
    enum Output : cpstd::size_t { Mixed = 0U };

    bool process(const DspCore::Processing::SignalManager& inputs,
        DspCore::Processing::SignalManager& outputs) noexcept override {
        const auto* audio = inputs.Read<float>(Audio);
        const auto* control = inputs.Read<short>(Control);
        auto* output = outputs.Write<float>(Mixed);
        if (inputs.Count() != 2U || outputs.Count() != 1U || !inputs.IsValid() || !outputs.IsValid() ||
            audio == nullptr || control == nullptr || output == nullptr || audio->Size() != control->Size() ||
            output->Capacity() < audio->Size()) {
            return false;
        }

        for (cpstd::size_t index = 0U; index < audio->Size(); ++index) {
            output->Data()[index] = audio->Data()[index] + static_cast<float>(control->Data()[index]);
        }
        return outputs.Commit(Mixed, audio->Size());
    }
    void reset() noexcept override {}
};

TEST(ProcessorBase, SupportsHeterogeneousMimoPorts) {
    const float audio[] = {0.5F, 1.5F};
    const short control[] = {2, -1};
    float output[2]{};
    auto audioBlock = DspCore::Processing::MakeInputSignalBlock(audio, 2U);
    auto controlBlock = DspCore::Processing::MakeInputSignalBlock(control, 2U);
    auto outputBlock = DspCore::Processing::MakeOutputSignalBlock(output, 2U);
    DspCore::Processing::SignalBlockBase* inputBlocks[] = {&audioBlock, &controlBlock};
    DspCore::Processing::SignalBlockBase* outputBlocks[] = {&outputBlock};
    const DspCore::Processing::SignalManager inputs{inputBlocks, 2U};
    DspCore::Processing::SignalManager outputs{outputBlocks, 1U};

    HeterogeneousMimoProbe processor;
    ASSERT_TRUE(processor.process(inputs, outputs));
    EXPECT_EQ(outputBlock.Size(), 2U);
    EXPECT_FLOAT_EQ(output[0], 2.5F);
    EXPECT_FLOAT_EQ(output[1], 0.5F);
}

TEST(FIR, CoefficientsConfigureTheRuntimeProcessor) {
    using Filter = DspCore::Filters::FIR<float>;
    const float taps[] = {0.25F, 0.5F, 0.25F};
    Filter::Coefficients coefficients;
    ASSERT_TRUE(coefficients.Assign(taps, 3U));

    Filter filter;
    ASSERT_TRUE(filter.Configure(coefficients));
    float output{};
    ASSERT_TRUE(processSample(filter, 1.0F, output));
    EXPECT_FLOAT_EQ(output, 0.25F);
    ASSERT_TRUE(processSample(filter, 0.0F, output));
    EXPECT_FLOAT_EQ(output, 0.5F);
}

TEST(FIR, SeparatesStreamCoefficientsAndStatePrecision) {
    using Filter = DspCore::Filters::FIR<float, double>;
    const double taps[] = {0.25, 0.5, 0.25};
    Filter::Coefficients coefficients;
    ASSERT_TRUE(coefficients.Assign(taps, 3U));

    Filter filter;
    ASSERT_TRUE(filter.Configure(coefficients));
    float output{};
    ASSERT_TRUE(processSample(filter, 1.0F, output));
    EXPECT_FLOAT_EQ(output, 0.25F);
    ASSERT_TRUE(processSample(filter, 0.0F, output));
    EXPECT_FLOAT_EQ(output, 0.5F);
}

TEST(IIR, GenericCoefficientsNormalizeAndProcessTheRecurrence) {
    using Filter = DspCore::Filters::IIR<float>;
    const float numerator[] = {1.0F};
    const float denominator[] = {2.0F, -1.0F};
    Filter::Coefficients coefficients;
    ASSERT_TRUE(coefficients.Assign(numerator, 1U, denominator, 2U));

    Filter filter;
    ASSERT_TRUE(filter.Configure(coefficients));
    float output{};
    ASSERT_TRUE(processSample(filter, 1.0F, output));
    EXPECT_FLOAT_EQ(output, 0.5F);
    ASSERT_TRUE(processSample(filter, 0.0F, output));
    EXPECT_FLOAT_EQ(output, 0.25F);
}

TEST(IIR, SeparatesStreamCoefficientsAndStatePrecision) {
    using Filter = DspCore::Filters::IIR<float, double>;
    const double numerator[] = {1.0};
    const double denominator[] = {2.0, -1.0};

    Filter filter;
    ASSERT_TRUE(filter.Configure(numerator, denominator));
    float output{};
    ASSERT_TRUE(processSample(filter, 1.0F, output));
    EXPECT_FLOAT_EQ(output, 0.5F);
    ASSERT_TRUE(processSample(filter, 0.0F, output));
    EXPECT_FLOAT_EQ(output, 0.25F);
}

TEST(IIR, ConvertsAnIntegralStreamAtTheProcessorBoundary) {
    using Filter = DspCore::Filters::IIR<short>;
    const float numerator[] = {0.5F};
    const float denominator[] = {1.0F};

    Filter filter;
    ASSERT_TRUE(filter.Configure(numerator, denominator));
    short output{};
    ASSERT_TRUE(processSample(filter, static_cast<short>(4), output));
    EXPECT_EQ(output, 2);
}

TEST(Biquad, DesignersReturnIirCoefficientValues) {
    using Filter = DspCore::Filters::IIR<float>;
    const auto lowPass = DspCore::Filters::Biquad::LowPass(48000.0F, 1200.0F, 0.707F);
    EXPECT_EQ(lowPass.numerator.size(), 3U);
    EXPECT_EQ(lowPass.denominator.size(), 3U);
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(lowPass));

    Filter filter;
    ASSERT_TRUE(filter.Configure(lowPass));
    const auto notch = DspCore::Filters::Biquad::Notch(48000.0F, 1200.0F, 1.0F);
    const auto allPass = DspCore::Filters::Biquad::AllPass(48000.0F, 1200.0F, 1.0F);
    const auto peak = DspCore::Filters::Biquad::Peak(48000.0F, 1200.0F, 1.0F, 6.0F);
    const auto lowShelf = DspCore::Filters::Biquad::LowShelf(48000.0F, 1200.0F, 1.0F, 6.0F);
    const auto highShelf = DspCore::Filters::Biquad::HighShelf(48000.0F, 1200.0F, 1.0F, -6.0F);
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(notch));
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(allPass));
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(peak));
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(lowShelf));
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(highShelf));
}

TEST(Biquad, DoublePrecisionDesignConfiguresAFloatStream) {
    using Filter = DspCore::Filters::IIR<float, double>;
    const auto lowPass = DspCore::Filters::Biquad::Design<double>::LowPass(48000.0, 1200.0, 0.707);

    Filter filter;
    ASSERT_TRUE(filter.Configure(lowPass));
    float output{};
    ASSERT_TRUE(processSample(filter, 1.0F, output));
    EXPECT_GT(output, 0.0F);
}

TEST(Biquad, DefaultDesignerUsesFloatCoefficients) {
    using Filter = DspCore::Filters::IIR<float>;
    const auto lowPass = DspCore::Filters::Biquad::Design<>::LowPass(48000.0F, 1200.0F, 0.707F);

    Filter filter;
    ASSERT_TRUE(filter.Configure(lowPass));
    EXPECT_TRUE(DspCore::Filters::Biquad::IsStable(lowPass));
}

TEST(Biquad, ExposesTheControlsRequiredByEachDesign) {
    using namespace DspCore::Filters::Biquad;

    EXPECT_EQ(ParameterCount(Type::Peak), 4U);
    ParameterInfo parameter;
    ASSERT_TRUE(ParameterAt(Type::Peak, 0U, parameter));
    EXPECT_EQ(parameter.parameter, Parameter::SampleRate);
    EXPECT_EQ(parameter.label, ParameterLabel::SampleRate);
    EXPECT_EQ(parameter.unit, Unit::Hertz);
    ASSERT_TRUE(ParameterAt(Type::Peak, 1U, parameter));
    EXPECT_EQ(parameter.parameter, Parameter::Cutoff);
    EXPECT_EQ(parameter.label, ParameterLabel::CenterFrequency);
    ASSERT_TRUE(ParameterAt(Type::Peak, 2U, parameter));
    EXPECT_EQ(parameter.parameter, Parameter::Quality);
    ASSERT_TRUE(ParameterAt(Type::Peak, 3U, parameter));
    EXPECT_EQ(parameter.parameter, Parameter::GainDecibels);
    EXPECT_EQ(parameter.label, ParameterLabel::Gain);
    EXPECT_EQ(parameter.unit, Unit::Decibels);
    EXPECT_FALSE(ParameterAt(Type::Peak, 4U, parameter));

    EXPECT_TRUE(HasParameter(Type::LowShelf, Parameter::Slope));
    EXPECT_TRUE(HasParameter(Type::LowShelf, Parameter::GainDecibels));
    EXPECT_FALSE(HasParameter(Type::LowShelf, Parameter::Quality));
}

} // namespace
