#ifndef DSPCORE_EXAMPLES_DESKTOP_SIGNAL_CSV_H
#define DSPCORE_EXAMPLES_DESKTOP_SIGNAL_CSV_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>

namespace DspCoreExamples { namespace Desktop {

struct SignalColumn {
    const char* name;
    const float* samples;
};

inline std::filesystem::path DefaultOutputDirectory() {
    const char* home=std::getenv("HOME");
#if defined(_WIN32)
    if(!home || home[0]=='\0') home=std::getenv("USERPROFILE");
#endif
    if(home && home[0]!='\0') return std::filesystem::path(home)/"Documents"/"DspCoreExampleOutput";
    std::error_code error;
    const std::filesystem::path current=std::filesystem::current_path(error);
    return error ? std::filesystem::path("DspCoreExampleOutput") : current/"DspCoreExampleOutput";
}

inline bool ParseOutputDirectory(int argc,char* argv[],std::filesystem::path& directory) {
    directory=DefaultOutputDirectory();
    if(argc==1) return true;
    if(argc==3 && std::strcmp(argv[1],"--output")==0 && argv[2][0]!='\0') {
        directory=argv[2];
        return true;
    }
    std::fprintf(stderr,"Usage: %s [--output <directory>]\n",argv[0]);
    return false;
}

inline bool WriteSignalCsv(const std::filesystem::path& directory,const char* fileName,
                           const char* title,const char* description,float sampleRate,
                           const SignalColumn* columns,size_t columnCount,size_t sampleCount,
                           std::filesystem::path& outputPath) {
    if(directory.empty() || !fileName || !title || !description || sampleRate<=0.f ||
       !columns || columnCount==0 || sampleCount==0) return false;
    for(size_t column=0;column<columnCount;++column)
        if(!columns[column].name || !columns[column].samples) return false;

    std::error_code error;
    std::filesystem::create_directories(directory,error);
    if(error) return false;

    outputPath=directory/fileName;
    std::ofstream file(outputPath);
    if(!file.is_open()) return false;
    file << "# title=" << title << '\n';
    file << "# description=" << description << '\n';
    file << "# sample_rate_hz=" << sampleRate << '\n';
    file << "sample,time_seconds";
    for(size_t column=0;column<columnCount;++column) file << ',' << columns[column].name;
    file << '\n' << std::setprecision(9);
    for(size_t sample=0;sample<sampleCount;++sample) {
        file << sample << ',' << static_cast<double>(sample)/static_cast<double>(sampleRate);
        for(size_t column=0;column<columnCount;++column) file << ',' << columns[column].samples[sample];
        file << '\n';
    }
    return file.good();
}

inline void PrintPlotHint(const std::filesystem::path& outputPath,const char* executable) {
    std::error_code error;
    const std::filesystem::path executablePath=std::filesystem::absolute(executable,error);
    const std::filesystem::path toolDirectory=error ? std::filesystem::path() : executablePath.parent_path();
    const std::string path=std::filesystem::absolute(outputPath).string();
#if defined(_WIN32)
    const std::string renderTool=(toolDirectory/"render_signal.ps1").string();
#else
    const std::string renderTool=(toolDirectory/"render_signal.sh").string();
#endif
    const std::string matlabToolDirectory=toolDirectory.string();
    std::printf("Wrote signal data: %s\n",path.c_str());
#if defined(_WIN32)
    std::printf("PNG (PowerShell): & \"%s\" \"%s\"\n",renderTool.c_str(),path.c_str());
#else
    std::printf("PNG: \"%s\" \"%s\"\n",renderTool.c_str(),path.c_str());
#endif
    std::printf("MATLAB: addpath(\"%s\"); plot_signal(\"%s\")\n",matlabToolDirectory.c_str(),path.c_str());
}

}}

#endif
