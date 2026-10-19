#include <DspCore_Analysis.h>
#include "Shared.h"
void PrintLine(const char* text) { Serial.println(text); }
void setup() {
    Serial.begin(115200);
    while(!Serial) {}
    Serial.println("DspCore :: Analysis / Envelope");
    const bool ok=DspCoreExamples::Analysis::Envelope::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}
void loop() {}
