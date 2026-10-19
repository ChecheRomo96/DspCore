#include <DspCore_Generators.h>
#include "Shared.h"
void PrintLine(const char* text) { Serial.println(text); }
void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("DspCore :: Generators / Windows");
    const bool ok=DspCoreExamples::Generators::Windows::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}
void loop() {}
