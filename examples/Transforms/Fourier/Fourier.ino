#include <DspCore_Transforms.h>
#include "Shared.h"

void PrintLine(const char* text) { Serial.println(text); }

void setup() {
    Serial.begin(115200);
    while(!Serial) {}
    Serial.println("DspCore :: Transforms / Fourier");
    const bool ok=DspCoreExamples::Transforms::Fourier::Run(PrintLine);
    Serial.println(ok ? "Overall: PASS" : "Overall: FAIL");
}

void loop() {}
