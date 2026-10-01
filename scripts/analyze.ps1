param(
    [Parameter(Position = 0, Mandatory = $true)]
    [string]$Preset,

    [switch]$Fresh
)

$ErrorActionPreference = "Stop"

. "$PSScriptRoot/common.ps1"

# Runs clang-tidy (.clang-tidy) over the DspCore sources of a Ninja preset.
# Set CLANG_TIDY to choose the executable.
Assert-DspCorePreset -Preset $Preset
$clangTidy = if ($env:CLANG_TIDY) { $env:CLANG_TIDY } else { "clang-tidy" }
if (-not (Get-Command $clangTidy -ErrorAction SilentlyContinue)) {
    throw "$clangTidy not found"
}

$configureParameters = @{ Preset = $Preset }
if ($Fresh) {
    $configureParameters.Fresh = $true
}
& "$PSScriptRoot/configure.ps1" @configureParameters
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$buildDirectory = Get-DspCoreBuildDirectory -Preset $Preset
if (-not (Test-Path (Join-Path $buildDirectory "compile_commands.json"))) {
    throw "$buildDirectory/compile_commands.json not found; use a Ninja preset"
}

$root = Split-Path -Parent $PSScriptRoot
$failed = $false
foreach ($source in Get-ChildItem -Path (Join-Path $root "src") -Recurse -Filter *.cpp | Sort-Object FullName) {
    Write-Host "clang-tidy $($source.FullName.Substring($root.Length + 1))"
    & $clangTidy -p $buildDirectory --quiet $source.FullName
    if ($LASTEXITCODE -ne 0) {
        $failed = $true
    }
}

if ($failed) {
    throw "clang-tidy reported findings"
}
Write-Host "clang-tidy found no issues in the DspCore sources"
