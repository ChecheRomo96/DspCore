param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Preset,

    [string]$Prefix = ""
)

. "$PSScriptRoot/romodular-adapter.ps1"

& (Join-Path $script:DspCoreRoModularScripts "install.ps1") `
    -Preset $Preset `
    -Prefix $Prefix
