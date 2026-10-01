// The Arduino builder only discovers libraries included from the sketch.
#include <Foundation.h>
#include <DspCore.h>

#include "Shared.h"

void setup() {
    Serial.begin(115200);
    while(!Serial) {}

    Serial.println("DspCore :: Core / Version");
    Serial.print("DspCore ....... ");
    Serial.println(DspCoreExamples::Core::Version::DspCoreVersion());
    Serial.print("Foundation .... ");
    Serial.println(DspCoreExamples::Core::Version::FoundationVersion());
}

void loop() {}
