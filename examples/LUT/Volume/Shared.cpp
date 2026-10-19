#include "Shared.h"
#include <DspCore_LUT.h>
#include <stdio.h>
namespace DspCoreExamples { namespace LUT { namespace Volume {
bool Run(void (*print)(const char*)) {
    DspCore::LUT::LUT3D<int> source;
    if (!source.Resize(2, 3, 4)) { print("FAIL: allocation"); return false; }
    source.SetValue(1, 2, 3, 123);
    auto* buffer = source.GetBuffer();
    DspCore::LUT::LUT3D<int> table(static_cast<DspCore::LUT::LUT3D<int>&&>(source));
    const bool moved = source.Empty() && source.GetBuffer() == nullptr && table.GetBuffer() == buffer;
    char message[80];
    snprintf(message, sizeof(message), "2x3x4 volume: (1,2,3)=%d; expected=123", table.GetValue(1, 2, 3));
    print(message);
    print(moved ? "Move transferred storage and cleared source: PASS" : "Move: FAIL");
    const bool overflow = !table.Resize(static_cast<size_t>(-1), 2, 1) && table.GetValue(1, 2, 3) == 123;
    print(overflow ? "Overflow rejected; shape and sample preserved: PASS" : "Overflow: FAIL");
    table.Resize(0, 0, 0);
    table.Release();
    table.Release();
    const bool released = table.GetBuffer() == nullptr && table.Capacity() == 0;
    print(released ? "Empty resize and repeated Release: PASS" : "Release: FAIL");
    return moved && overflow && released;
}
}}}
