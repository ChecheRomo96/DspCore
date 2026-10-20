[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$CsvPath,

    [Parameter(Position = 1)]
    [string]$PngPath
)

if ([string]::IsNullOrWhiteSpace($PngPath)) {
    $PngPath = [System.IO.Path]::ChangeExtension($CsvPath, ".png")
}

$python = Get-Command python3 -ErrorAction SilentlyContinue
if ($null -eq $python) {
    $python = Get-Command python -ErrorAction SilentlyContinue
}
if ($null -eq $python) {
    throw "Python 3 is required to render DspCore signal plots."
}

$plotter = Join-Path $PSScriptRoot "plot_signal.py"
& $python.Source $plotter $CsvPath --save $PngPath
exit $LASTEXITCODE
