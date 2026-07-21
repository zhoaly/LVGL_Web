param(
    [string]$Server = "http://100.87.225.67:8000",
    [string]$ProjectName = "LVGL_Web",
    [ValidateRange(120, 4096)][int]$Width = 240,
    [ValidateRange(120, 4096)][int]$Height = 320,
    [switch]$NoWait
)

$ErrorActionPreference = "Stop"
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$zipPath = Join-Path $env:TEMP "LVGL_Web_remote_build.zip"

function Invoke-CurlJson {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments,
        [Parameter(Mandatory = $true)]
        [string]$Operation
    )

    $output = & curl.exe @Arguments 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "$Operation failed (curl exit $LASTEXITCODE): $($output -join [Environment]::NewLine)"
    }

    try {
        return ($output -join [Environment]::NewLine) | ConvertFrom-Json
    }
    catch {
        throw "$Operation returned invalid JSON: $($output -join [Environment]::NewLine)"
    }
}

if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}

Push-Location $projectRoot
try {
    tar.exe -a -cf $zipPath `
        --exclude='./build' `
        --exclude='./build_web' `
        --exclude='./build_tests*' `
        --exclude='./.git' `
        .
}
finally {
    Pop-Location
}

$upload = Invoke-CurlJson -Operation "LVGL project upload" -Arguments @(
    "-sS", "--fail-with-body", "--connect-timeout", "10", "--max-time", "120",
    "-F", "file=@$zipPath",
    "-F", "project_name=$ProjectName",
    "-F", "width=$Width",
    "-F", "height=$Height",
    "$Server/api/lvgl/build/upload"
)

if (-not $upload.job_id) {
    throw "LVGL project upload did not return a job_id."
}
Write-Host "LVGL remote build started: $($upload.job_id)"

if ($NoWait) {
    $upload
    exit 0
}

do {
    Start-Sleep -Seconds 3
    $job = Invoke-CurlJson -Operation "LVGL job status request" -Arguments @(
        "-sS", "--fail-with-body", "--connect-timeout", "10", "--max-time", "30",
        "$Server/api/lvgl/jobs/$($upload.job_id)"
    )
    Write-Host "[$($job.status)] $($job.message)"
} while ($job.status -notin @("success", "failed"))

if ($job.status -eq "failed") {
    Write-Host "`nRemote build log:"
    & curl.exe -sS --connect-timeout 10 --max-time 30 `
        "$Server/api/lvgl/logs/$($upload.job_id)"
    exit 1
}

$job
Write-Host "Preview : $Server$($job.preview_url)"
Write-Host "Artifact: $Server$($job.artifact_url)"
Write-Host "Log     : $Server$($job.log_url)"
