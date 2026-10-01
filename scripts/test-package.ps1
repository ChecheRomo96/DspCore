param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Preset,

    [string]$FoundationPrefix = "",
    [int]$Parallel = 0,
    [switch]$Fresh
)

. "$PSScriptRoot/common.ps1"

Assert-DspCorePreset -Preset $Preset
if ($Preset -notmatch "^(macos|linux|windows)_") {
    throw "Package consumer tests require a runnable desktop preset"
}

# Without an explicit prefix, DspCore resolves Foundation itself: sibling export,
# GitHub Release package or sources (see cmake/DspCoreFoundation.cmake).
if (-not $FoundationPrefix -and $env:DSPCORE_FOUNDATION_PREFIX) {
    $FoundationPrefix = $env:DSPCORE_FOUNDATION_PREFIX
}
$exportParameters = @{
    Preset = $Preset
}
if ($FoundationPrefix) {
    $FoundationPrefix = Resolve-DspCorePath -Path $FoundationPrefix
    $foundationConfig = Join-Path $FoundationPrefix "lib/cmake/Foundation/FoundationConfig.cmake"
    if (-not (Test-Path -LiteralPath $foundationConfig -PathType Leaf)) {
        throw "Foundation package not found at $FoundationPrefix"
    }
    $exportParameters.CMakeArguments = @("-DDSPCORE_FOUNDATION_PREFIX=$FoundationPrefix")
}
if ($Parallel -gt 0) {
    $exportParameters.Parallel = $Parallel
}
if ($Fresh) {
    $exportParameters.Fresh = $true
}
& "$PSScriptRoot/export.ps1" @exportParameters
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

# The Foundation package DspCore used; empty when Foundation was built from
# sources and installed next to DspCore.
$cachePath = Join-Path (Get-DspCoreBuildDirectory -Preset $Preset) "CMakeCache.txt"
$resolvedFoundationPrefix = ""
$resolvedLine = Select-String -LiteralPath $cachePath `
    -Pattern "^DSPCORE_FOUNDATION_RESOLVED_PREFIX:INTERNAL=(.*)$" | Select-Object -First 1
if ($resolvedLine) {
    $resolvedFoundationPrefix = $resolvedLine.Matches[0].Groups[1].Value
}

$dspcorePrefix = Join-Path $script:DspCoreDistRoot $Preset
$consumerSource = Join-Path $script:DspCoreRoot "tests/PackageConsumer"
$consumerBuild = Join-Path $script:DspCoreBuildRoot "package-consumer/$Preset"

$dspcoreBuildDirectory = Get-DspCoreBuildDirectory -Preset $Preset
$dspcoreCache = Join-Path $dspcoreBuildDirectory "CMakeCache.txt"

function Get-DspCoreCacheValue {
    param([Parameter(Mandatory = $true)][string]$Name)

    $match = Select-String `
        -LiteralPath $dspcoreCache `
        -Pattern "^${Name}:[^=]*=" | `
        Select-Object -First 1

    if (-not $match) {
        return ""
    }

    return ($match.Line -split "=", 2)[1]
}

$generator = Get-DspCoreCacheValue -Name "CMAKE_GENERATOR"
$generatorPlatform = Get-DspCoreCacheValue -Name "CMAKE_GENERATOR_PLATFORM"
$cxxCompiler = Get-DspCoreCacheValue -Name "CMAKE_CXX_COMPILER"
$toolchainFile = Get-DspCoreCacheValue -Name "CMAKE_TOOLCHAIN_FILE"
$osxArchitectures = Get-DspCoreCacheValue -Name "CMAKE_OSX_ARCHITECTURES"
$crossCompiling = Get-DspCoreCacheValue -Name "CMAKE_CROSSCOMPILING"

if ($crossCompiling -eq "TRUE") {
    throw "Package execution requires a native preset"
}
if (-not $generator) {
    throw "Configured preset has no CMake generator"
}

# The package consumer must use the same ABI and compiler family as the
# package. Recreate it so a previous run cannot retain another generator.
if (Test-Path -LiteralPath $consumerBuild) {
    Remove-Item -LiteralPath $consumerBuild -Recurse -Force
}

$configureArguments = @(
    "-S", $consumerSource,
    "-B", $consumerBuild,
    "-G", $generator,
    "-DDspCore_DIR=$(Join-Path $dspcorePrefix 'lib/cmake/DspCore')",
    "-DCMAKE_PREFIX_PATH=$dspcorePrefix;$resolvedFoundationPrefix"
)

if ($generatorPlatform) {
    $configureArguments += @("-A", $generatorPlatform)
}
if ($osxArchitectures) {
    $configureArguments += "-DCMAKE_OSX_ARCHITECTURES=$osxArchitectures"
}
if ($toolchainFile) {
    $configureArguments += "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile"
}
elseif ($cxxCompiler -and $generator -notmatch "^(Visual Studio|Xcode)") {
    $configureArguments += "-DCMAKE_CXX_COMPILER=$cxxCompiler"
}

Invoke-DspCoreCMake -Arguments $configureArguments

$buildArguments = @("--build", $consumerBuild, "--config", "Release")
if ($Parallel -gt 0) {
    $buildArguments += @("--parallel", $Parallel.ToString())
}
Invoke-DspCoreCMake -Arguments $buildArguments

$testArguments = @(
    "--test-dir", $consumerBuild,
    "--output-on-failure",
    "--no-tests=error",
    "--build-config", "Release"
)
if ($Parallel -gt 0) {
    $testArguments += @("--parallel", $Parallel.ToString())
}
& ctest @testArguments
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Verified installed DspCore package and its transitive Foundation dependency"
