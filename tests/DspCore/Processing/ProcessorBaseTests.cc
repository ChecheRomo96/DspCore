#include <gtest/gtest.h>
#include <DspCore/Processing/ProcessorBase.h>

namespace {

class DoublingProcessor : public DspCore::Processing::ProcessorBase<float> {
public:
    size_t InputPorts() const noexcept override { return 1; }
    size_t OutputPorts() const noexcept override { return 1; }
    bool Process(const DspCore::Processing::InputBlock<float>* inputs,size_t inputCount,
                 DspCore::Processing::OutputBlock<float>* outputs,size_t outputCount) noexcept override {
        if(inputCount!=1 || outputCount!=1 || !inputs || !outputs || !inputs[0].IsValid() ||
           !outputs[0].CanWrite(inputs[0].size)) return false;
        for(size_t i=0;i<inputs[0].size;++i) outputs[0].data[i]=inputs[0].data[i]*2.f;
        outputs[0].size=inputs[0].size;
        return true;
    }
    void Reset() noexcept override {}
};

TEST(ProcessorBase, DescribesMimoPortsAndWritesCallerOwnedOutput) {
    DoublingProcessor processor;
    const float input[]={1.f,2.f,3.f};
    float output[]={0.f,0.f,0.f};
    DspCore::Processing::InputBlock<float> inputs[]={DspCore::Processing::InputBlock<float>(input,3)};
    DspCore::Processing::OutputBlock<float> outputs[]={DspCore::Processing::OutputBlock<float>(output,3)};
    EXPECT_EQ(processor.InputPorts(),1U);
    EXPECT_EQ(processor.OutputPorts(),1U);
    EXPECT_TRUE(processor.Process(inputs,1,outputs,1));
    EXPECT_EQ(outputs[0].size,3U);
    EXPECT_FLOAT_EQ(output[0],2.f);
    EXPECT_FLOAT_EQ(output[2],6.f);
}

TEST(ProcessorBase, BlockViewsValidateNullStorageOnlyWhenNonempty) {
    EXPECT_TRUE(DspCore::Processing::InputBlock<float>().IsValid());
    EXPECT_FALSE(DspCore::Processing::InputBlock<float>(0,1).IsValid());
    EXPECT_TRUE(DspCore::Processing::OutputBlock<float>().CanWrite(0));
    EXPECT_FALSE(DspCore::Processing::OutputBlock<float>().CanWrite(1));
}

}
