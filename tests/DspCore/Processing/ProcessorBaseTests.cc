#include <gtest/gtest.h>
#include <DspCore/Processing/ProcessorBase.h>

namespace {

class MimoProcessor : public DspCore::Processing::ProcessorBase<float> {
public:
    size_t InputPorts() const noexcept override { return 2; }
    size_t OutputPorts() const noexcept override { return 2; }
    bool Process(const DspCore::Processing::InputBlock<float>* inputs,size_t inputCount,
                 DspCore::Processing::OutputBlock<float>* outputs,size_t outputCount) noexcept override {
        if(inputCount!=2 || outputCount!=2 || !inputs || !outputs || !inputs[0].IsValid() ||
           !inputs[1].IsValid() || inputs[1].size!=inputs[0].size ||
           !outputs[0].CanWrite(inputs[0].size) || !outputs[1].CanWrite(inputs[0].size)) return false;
        for(size_t i=0;i<inputs[0].size;++i) {
            outputs[0].data[i]=inputs[0].data[i]+inputs[1].data[i];
            outputs[1].data[i]=inputs[0].data[i]-inputs[1].data[i];
        }
        outputs[0].size=inputs[0].size;
        outputs[1].size=inputs[0].size;
        return true;
    }
    void Reset() noexcept override {}
};

TEST(ProcessorBase, DescribesMimoPortsAndWritesNValuesToEveryOutput) {
    MimoProcessor processor;
    const float left[]={1.f,2.f,3.f},right[]={0.5f,1.f,1.5f};
    float sum[]={0.f,0.f,0.f},difference[]={0.f,0.f,0.f};
    DspCore::Processing::InputBlock<float> inputs[]={
        DspCore::Processing::InputBlock<float>(left,3),DspCore::Processing::InputBlock<float>(right,3)};
    DspCore::Processing::OutputBlock<float> outputs[]={
        DspCore::Processing::OutputBlock<float>(sum,3),DspCore::Processing::OutputBlock<float>(difference,3)};
    EXPECT_EQ(processor.InputPorts(),2U);
    EXPECT_EQ(processor.OutputPorts(),2U);
    EXPECT_TRUE(processor.Process(inputs,2,outputs,2));
    EXPECT_EQ(outputs[0].size,3U);
    EXPECT_EQ(outputs[1].size,3U);
    EXPECT_FLOAT_EQ(sum[0],1.5f);
    EXPECT_FLOAT_EQ(difference[2],1.5f);

    const DspCore::Processing::InputBlock<float> mismatched[]={
        DspCore::Processing::InputBlock<float>(left,3),DspCore::Processing::InputBlock<float>(right,2)};
    EXPECT_FALSE(processor.Process(mismatched,2,outputs,2));
}

TEST(ProcessorBase, BlockViewsValidateNullStorageOnlyWhenNonempty) {
    EXPECT_TRUE(DspCore::Processing::InputBlock<float>().IsValid());
    EXPECT_FALSE(DspCore::Processing::InputBlock<float>(0,1).IsValid());
    EXPECT_TRUE(DspCore::Processing::OutputBlock<float>().CanWrite(0));
    EXPECT_FALSE(DspCore::Processing::OutputBlock<float>().CanWrite(1));
}

}
