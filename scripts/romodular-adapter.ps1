$script:DspCoreRoot = Split-Path -Parent $PSScriptRoot
$script:DspCoreBuildRoot = Join-Path $script:DspCoreRoot "build"
$script:DspCoreDistRoot = Join-Path $script:DspCoreRoot "dist"
$script:DspCoreRoModularScripts = Join-Path `
    $script:DspCoreRoot `
    "tools/RoModularBuild/scripts"

$roModularCommon = Join-Path $script:DspCoreRoModularScripts "common.ps1"
if (-not (Test-Path -LiteralPath $roModularCommon -PathType Leaf)) {
    throw "RoModularBuild is unavailable; initialize tools/RoModularBuild with git submodule update --init --recursive"
}

$env:ROMODULAR_PROJECT_ROOT = $script:DspCoreRoot
$env:ROMODULAR_BUILD_ROOT = $script:DspCoreBuildRoot
$env:ROMODULAR_DIST_ROOT = $script:DspCoreDistRoot
$env:ROMODULAR_PROJECT_LABEL = "DspCore"
$env:ROMODULAR_CONFIGURE_COMMAND = "scripts/configure.ps1"
$env:ROMODULAR_DEFAULT_CONFIGURATION = "Debug"
$env:ROMODULAR_DOCUMENTATION_PRESET = "documentation"
$env:ROMODULAR_DOCUMENTATION_CONFIGURATION = "Release"
$env:ROMODULAR_INSTALL_CONFIGURATION = "Release"
$env:ROMODULAR_TESTING_CACHE_ARGUMENT = "-DDSPCORE_TESTING=ON"
$env:ROMODULAR_EXAMPLES_CACHE_ARGUMENT = "-DDSPCORE_EXAMPLES=ON"
