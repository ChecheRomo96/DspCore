param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Preset,

    [string]$Configuration = "",
    [string]$Target = "",
    [int]$Parallel = 0,
    [switch]$CleanFirst,
    [switch]$Fresh,
    [Alias("examples-on")]
    [switch]$ExamplesOn
)

. "$PSScriptRoot/romodular-adapter.ps1"

$parameters = @{
    Preset = $Preset
    Configuration = $Configuration
    Target = $Target
    Parallel = $Parallel
    CleanFirst = $CleanFirst
    Fresh = $Fresh
    ExamplesOn = $ExamplesOn
}

& (Join-Path $script:DspCoreRoModularScripts "build.ps1") @parameters
