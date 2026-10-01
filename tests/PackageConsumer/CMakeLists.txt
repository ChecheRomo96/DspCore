cmake_minimum_required(VERSION 3.25)

project(DspCorePackageConsumer LANGUAGES CXX)

find_package(DspCore 0.1 CONFIG REQUIRED)

add_executable(DspCorePackageConsumer main.cpp)
target_link_libraries(DspCorePackageConsumer PRIVATE DspCore::DspCore)

enable_testing()
add_test(NAME DspCorePackageConsumer COMMAND DspCorePackageConsumer)
