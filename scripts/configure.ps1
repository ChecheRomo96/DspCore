param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Preset,

    [switch]$Fresh,

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$CMakeArguments
)

. "$PSScriptRoot/romodular-adapter.ps1"

$parameters = @{
    Preset = $Preset
}
if ($Fresh) {
    $parameters.Fresh = $true
}
if ($CMakeArguments) {
    $parameters.CMakeArguments = $CMakeArguments
}

& (Join-Path $script:DspCoreRoModularScripts "configure.ps1") @parameters
