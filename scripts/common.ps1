. "$PSScriptRoot/romodular-adapter.ps1"
. (Join-Path $script:DspCoreRoModularScripts "common.ps1")

function Invoke-DspCoreCMake {
    param([Parameter(Mandatory = $true)][string[]]$Arguments)

    Invoke-RoModularCMake -Arguments $Arguments
}

function Get-DspCoreBuildDirectory {
    param([Parameter(Mandatory = $true)][string]$Preset)
    return Get-RoModularBuildDirectory -Preset $Preset
}

function Assert-DspCorePreset {
    param([Parameter(Mandatory = $true)][string]$Preset)

    Assert-RoModularPreset -Preset $Preset
}

function Get-DspCoreConfiguration {
    param(
        [Parameter(Mandatory = $true)][string]$Preset,
        [string]$Configuration = "",
        [string]$DefaultConfiguration = "Debug"
    )

    return Get-RoModularConfiguration `
        -Preset $Preset `
        -Configuration $Configuration `
        -DefaultConfiguration $DefaultConfiguration
}

function Assert-DspCoreConfiguration {
    param([Parameter(Mandatory = $true)][string]$Configuration)

    Assert-RoModularConfiguration -Configuration $Configuration
}

function Assert-DspCoreConfigured {
    param([Parameter(Mandatory = $true)][string]$Preset)

    Assert-RoModularConfigured -Preset $Preset
}

function Resolve-DspCorePath {
    param([Parameter(Mandatory = $true)][string]$Path)

    return Resolve-RoModularPath -Path $Path
}

function Assert-DspCoreDistChild {
    param([Parameter(Mandatory = $true)][string]$Path)

    Assert-RoModularDistChild -Path $Path
}
