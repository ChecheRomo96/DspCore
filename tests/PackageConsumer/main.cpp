#include <Foundation/Math/Arithmetic.h>
#include <DspCore.h>

#include <cstring>
#include <iostream>

#ifndef DSPCORE_CORE
    #error "The installed DspCore package must export DSPCORE_CORE"
#endif

#ifndef DSPCORE_PROCESSING
    #error "The installed DspCore package must export DSPCORE_PROCESSING"
#endif

int main() {
    // The installed package must bring its transitive Foundation target with
    // it.
    const auto gcd = Foundation::Math::GCD(12, 8);

    std::cout << "DspCore: " << DspCore::Core::Version() << '\n';
    std::cout << "Foundation: " << DspCore::Core::FoundationVersion() << '\n';
    std::cout << "Foundation::Math::GCD(12, 8): " << gcd << '\n';

    DspCore::LUT::LUT1D<float> line;
    DspCore::LUT::LUT2D<int> surface;
    DspCore::LUT::LUT3D<double> volume;
    if (!line.Resize(2) || !surface.Resize(2, 3) || !volume.Resize(2, 3, 4)) return 1;
    line.SetValue(1, 0.5f);
    surface.SetValue(1, 2, 12);
    volume.SetValue(1, 2, 3, 0.25);
    if (line.GetValue(1) != 0.5f || surface.GetValue(1, 2) != 12 ||
        volume.GetValue(1, 2, 3) != 0.25) return 1;
    std::cout << "Installed LUT1D/2D/3D headers and storage: PASS\n";

    double interpolated = 0;
    if (!DspCore::Interpolation::Lookup(line, 0.5, interpolated,
            DspCore::Interpolation::Boundary::Clamp) || interpolated != 0.25) return 1;
    if (DspCore::Interpolation::Trilinear(1.,3.,5.,7.,9.,11.,13.,15.,0.25,0.5,0.75) != 9.5)
        return 1;
    std::cout << "Installed interpolation and LUT lookup: PASS\n";

    double generated[5];
    if (!DspCore::Generators::Envelopes::ExpSegment(generated,0.,1.,1e-9,5) ||
        generated[0] != 0. || generated[4] != 1.) return 1;
    if (!DspCore::Generators::Periodic::Triangle(generated,4) || generated[1] != 1.) return 1;
    if (!DspCore::Generators::Shaping::GammaMorph(volume.GetBuffer(),volume.Size(),1.,2.)) return 1;
    float shaped[3];
    line.SetValue(1,1.f);
    if (!DspCore::Generators::Envelopes::LUTSegment(shaped,0.f,1.f,3,line)) return 1;
    uint32_t seed=1;
    if (!DspCore::Generators::Noise::White(generated,5,seed)) return 1;
    if (!DspCore::Generators::Windows::Hann(generated,5) || generated[2] != 1.) return 1;
    std::cout << "Installed generators and LUT-shaped envelope: PASS\n";


    DspCore::Filters::FIR<double,double> fir;
    const double taps[]={0.5,0.5};
    const double filterInput[]={1.,0.};
    double filterOutput[]={0.,0.};
    if(!fir.Configure(taps,2) || !fir.Process(
            DspCore::Processing::MakeInputBlock(filterInput,2U),
            DspCore::Processing::MakeOutputBlock(filterOutput,2U)) ||
       filterOutput[0]!=0.5 || filterOutput[1]!=0.5) return 1;
    const auto coefficients=DspCore::Filters::Biquad::LowPass(48000.,1000.,0.707);
    DspCore::Filters::IIR<double,double> iir;
    double iirOutput[]={0.,0.};
    if(!iir.Configure(coefficients) || !iir.Process(filterInput,iirOutput,2) || iirOutput[0]<=0.) return 1;
    double filtered=0;
    const double block[]={-3,4,0,0};
    if(!DspCore::Analysis::RMS(block,4,filtered) || filtered!=2.5) return 1;
    DspCore::Analysis::EnvelopeFollower<double> follower;
    if(!follower.Update(-2.,0.01,filtered) || filtered!=2.) return 1;
    std::cout << "Installed typed block processing, FIR/IIR, RMS and follower: PASS\n";

    return (gcd == 4U &&
            std::strcmp(DspCore::Core::Version(), DSPCORE_VERSION) == 0) ? 0 : 1;
}
