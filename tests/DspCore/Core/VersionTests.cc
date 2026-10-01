#include <gtest/gtest.h>

#include <Foundation/Math/Arithmetic.h>
#include <DspCore.h>

#ifndef DSPCORE_CORE
    #error "DspCore.h must expose DSPCORE_CORE when the Core facade is available"
#endif

TEST(DspCoreCoreTests, ReportsDspCoreVersion) {
    EXPECT_STREQ(DspCore::Core::Version(), DSPCORE_VERSION);
}

TEST(DspCoreCoreTests, ReportsFoundationVersion) {
    EXPECT_STREQ(DspCore::Core::FoundationVersion(), FOUNDATION_VERSION);
}

TEST(DspCoreCoreTests, PropagatesFoundationDependency) {
    EXPECT_EQ(Foundation::Math::GCD(12, 8), 4U);
}
