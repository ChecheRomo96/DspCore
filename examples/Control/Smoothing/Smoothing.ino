#include <DspCore_Control.h>
#include "Shared.h"
void PrintLine(const char* text) { Serial.println(text); }
void setup() {
    Serial.begin(115200);
    while(!Serial) {}
    Serial.println("DspCore :: Control / Smoothing");
    const bool ok=DspCoreExamples::Control::Smoothing::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}
void loop() {}
