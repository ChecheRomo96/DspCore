#include "Shared.h"
#include <DspCore_LUT.h>
#include <stdio.h>
namespace DspCoreExamples { namespace LUT { namespace Surface {
bool Run(void (*print)(const char*)) {
    DspCore::LUT::LUT2D<int> table;
    if (!table.Resize(3, 2)) { print("FAIL: allocation"); return false; }
    for (size_t y = 0; y < table.Height(); ++y)
        for (size_t x = 0; x < table.Width(); ++x)
            table.SetValue(x, y, static_cast<int>(10 * y + x));
    char message[80];
    snprintf(message, sizeof(message), "3x2 surface: (2,1)=%d; expected=12", table.GetValue(2, 1));
    print(message);
    int sentinel = 99;
    const bool bounds = !table.TryGet(3, 0, sentinel) && sentinel == 99;
    print(bounds ? "Out-of-range read rejected; output preserved: PASS" : "Bounds: FAIL");
    if (!table.Resize(2, 3)) return false;
    snprintf(message, sizeof(message), "Reshape 2x3: flat sample 5=%d; expected=12", table.GetValue(1, 2));
    print(message);
    return bounds && table.GetValue(1, 2) == 12;
}
}}}
