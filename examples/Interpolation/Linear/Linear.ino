#include <DspCore_Interpolation.h>
#include "Shared.h"
void PrintLine(const char* message) { Serial.println(message); }
void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("DspCore :: Interpolation / Linear");
    const bool ok = DspCoreExamples::Interpolation::Linear::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}
void loop() {}
