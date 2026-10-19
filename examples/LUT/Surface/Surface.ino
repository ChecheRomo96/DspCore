#include <DspCore_LUT.h>
#include "Shared.h"
void PrintLine(const char* message) { Serial.println(message); }
void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("DspCore :: LUT / Surface");
    const bool ok = DspCoreExamples::LUT::Surface::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}
void loop() {}
