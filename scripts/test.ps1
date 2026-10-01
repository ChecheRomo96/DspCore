param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Preset,

    [string]$Configuration = "",
    [int]$Parallel = 0,
    [string]$Filter = "",
    [string]$JUnit = "",
    [switch]$Fresh,
    [switch]$AllowNoTests
)

. "$PSScriptRoot/romodular-adapter.ps1"

$parameters = @{
    Preset = $Preset
    Configuration = $Configuration
    Parallel = $Parallel
    Filter = $Filter
    JUnit = $JUnit
    Fresh = $Fresh
    AllowNoTests = $AllowNoTests
}

& (Join-Path $script:DspCoreRoModularScripts "test.ps1") @parameters
