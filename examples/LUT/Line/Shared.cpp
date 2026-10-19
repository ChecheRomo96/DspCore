#include "Shared.h"
#include <DspCore_LUT.h>
#include <stdio.h>
namespace DspCoreExamples { namespace LUT { namespace Line {
bool Run(void (*print)(const char*)) {
    DspCore::LUT::LUT1D<float> table;
    if (!table.Reserve(8) || !table.Resize(5)) { print("FAIL: allocation"); return false; }
    auto* buffer = table.GetBuffer();
    for (size_t i = 0; i < table.Size(); ++i) table.SetValue(i, static_cast<float>(i) / 4.0f);
    char message[80];
    for (size_t i = 0; i < table.Size(); ++i) {
        snprintf(message, sizeof(message), "curve[%u]=%d%%; expected=%u%%",
                 static_cast<unsigned>(i), static_cast<int>(table.GetValue(i) * 100.0f),
                 static_cast<unsigned>(i * 25));
        print(message);
    }
    bool ok = table.GetValue(0) == 0.0f && table.GetValue(2) == 0.5f && table.GetValue(4) == 1.0f;
    print(ok ? "Curve [0, 0.25, 0.5, 0.75, 1]: PASS" : "Curve values: FAIL");
    if (!table.Resize(2) || !table.Resize(5)) return false;
    const bool regrown = table.GetValue(2) == 0 && table.GetBuffer() == buffer && table.Capacity() == 8;
    print(regrown ? "Regrown samples=0; pointer retained; capacity=8: PASS" : "Regrowth: FAIL");
    return ok && regrown;
}
}}}
