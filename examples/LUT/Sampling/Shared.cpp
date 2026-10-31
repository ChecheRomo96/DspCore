#include "Shared.h"
#include <DspCore_LUT.h>
#include <stdio.h>
namespace DspCoreExamples { namespace LUT { namespace Sampling {
bool Run(void (*print)(const char*)) {
    using namespace DspCore::LUT;
    DspCore::LUT::LUT1D<int> table;
    if (!table.Resize(4)) return false;
    for (size_t i=0;i<4;++i) table.SetValue(i,static_cast<int>(i*10));
    float value = 0;
    bool ok = DspCore::LUT::Sample(table,0.5f,value,DspCore::LUT::Boundary::Clamp) && value == 15.f;
    char message[96];
    snprintf(message,sizeof(message),"Closed coordinate=0.5: value=%d; expected=15",static_cast<int>(value)); print(message);
    ok = DspCore::LUT::Sample(table,0.5f,value,DspCore::LUT::Boundary::Wrap) && value == 20.f && ok;
    snprintf(message,sizeof(message),"Periodic phase=0.5: value=%d; expected=20",static_cast<int>(value)); print(message);
    ok = DspCore::LUT::Sample(table,-0.125f,value,DspCore::LUT::Boundary::Wrap) && value == 15.f && ok;
    snprintf(message,sizeof(message),"Periodic phase=-0.125: seam value=%d; expected=15",static_cast<int>(value)); print(message);
    DspCore::LUT::LUT2D<int> plane;
    DspCore::LUT::LUT3D<int> volume;
    if (!plane.Resize(2,2) || !volume.Resize(2,2,2)) return false;
    for (size_t y=0;y<2;++y) for(size_t x=0;x<2;++x) {
        plane.SetValue(x,y,static_cast<int>(2*x+4*y));
        for(size_t z=0;z<2;++z) volume.SetValue(x,y,z,static_cast<int>(2*x+4*y+8*z));
    }
    ok = DspCore::LUT::Sample(plane,0.5f,0.5f,value,DspCore::LUT::Boundary::Reject) && value == 3.f && ok;
    snprintf(message,sizeof(message),"Bilinear center: value=%d; expected=3",static_cast<int>(value)); print(message);
    ok = DspCore::LUT::Sample(volume,0.5f,0.5f,0.5f,value,DspCore::LUT::Boundary::Reject) && value == 7.f && ok;
    snprintf(message,sizeof(message),"Trilinear center: value=%d; expected=7",static_cast<int>(value)); print(message);
    return ok;
}
}}}
