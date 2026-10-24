#include <DspCore_Filters.h>

using namespace DspCore::Filters::Biquad;

void setup() {
    Serial.begin(115200);

    ParameterInfo parameter;
    if (ParameterAt(Type::Peak, 3U, parameter)) {
        Serial.println(static_cast<unsigned int>(parameter.parameter));
    }
}

void loop() {}
