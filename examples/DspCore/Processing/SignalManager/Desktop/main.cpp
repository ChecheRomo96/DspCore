#include <iostream>

#include <DspCore/Processing/ProcessorBase.h>

namespace {

class GainAndOffset final : public DspCore::Processing::ProcessorBase {
public:
    enum Input : cpstd::size_t { Audio = 0U, Offset = 1U };
    enum Output : cpstd::size_t { ProcessedAudio = 0U };

    bool process(const DspCore::Processing::SignalManager& inputs,
        DspCore::Processing::SignalManager& outputs) noexcept override {
        const auto* audio = inputs.Read<float>(Audio);
        const auto* offset = inputs.Read<short>(Offset);
        auto* result = outputs.Write<float>(ProcessedAudio);
        if (inputs.Count() != 2U || outputs.Count() != 1U || !inputs.IsValid() || !outputs.IsValid() ||
            audio == nullptr || offset == nullptr || result == nullptr || audio->Size() != offset->Size() ||
            result->Capacity() < audio->Size()) {
            return false;
        }

        for (cpstd::size_t index = 0U; index < audio->Size(); ++index) {
            result->Data()[index] = 0.5F * audio->Data()[index] + static_cast<float>(offset->Data()[index]);
        }
        return outputs.Commit(ProcessedAudio, audio->Size());
    }

    void reset() noexcept override {}
};

} // namespace

int main() {
    const float audio[] = {1.0F, -0.5F, 0.25F, 0.0F};
    const short offset[] = {2, 2, -1, -1};
    float processed[4]{};

    auto audioBlock = DspCore::Processing::MakeInputSignalBlock(audio, 4U);
    auto offsetBlock = DspCore::Processing::MakeInputSignalBlock(offset, 4U);
    auto processedBlock = DspCore::Processing::MakeOutputSignalBlock(processed, 4U);

    DspCore::Processing::SignalBlockBase* inputBlocks[] = {&audioBlock, &offsetBlock};
    DspCore::Processing::SignalBlockBase* outputBlocks[] = {&processedBlock};
    const DspCore::Processing::SignalManager inputs{inputBlocks, 2U};
    DspCore::Processing::SignalManager outputs{outputBlocks, 1U};

    GainAndOffset processor;
    if (!processor.process(inputs, outputs)) {
        return 1;
    }

    std::cout << "========================================\n"
              << " DspCore :: Processing / SignalManager\n"
              << "========================================\n"
              << " Input[0] : float audio\n"
              << " Input[1] : short offset\n"
              << " Output[0]: float processed audio\n\n"
              << "   audio    offset    0.5 * audio + offset\n";
    for (cpstd::size_t index = 0U; index < processedBlock.Size(); ++index) {
        std::cout << "   " << audio[index] << "        " << offset[index] << "        " << processed[index] << '\n';
    }
    std::cout << "========================================\n";
    return 0;
}
