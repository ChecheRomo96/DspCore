param(
    [Parameter(Position = 0)]
    [string]$Preset = "documentation",

    [int]$Parallel = 0,
    [switch]$Fresh
)

. "$PSScriptRoot/common.ps1"

$buildParameters = @{
    Preset = $Preset
    Target = "docs"
}
if ($Parallel -gt 0) {
    $buildParameters.Parallel = $Parallel
}
if ($Fresh) {
    $buildParameters.Fresh = $true
}

& "$PSScriptRoot/build.ps1" @buildParameters
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Documentation: $(Join-Path $script:DspCoreBuildRoot "$Preset/docs/html/index.html")"
