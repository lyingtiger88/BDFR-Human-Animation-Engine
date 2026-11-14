$ErrorActionPreference = "Stop"

# Complete legacy Git history + release asset migration.
# Safe by design: all legacy refs are namespaced and will NOT overwrite main.
$Destination = "https://github.com/lyingtiger88/BDFR-Human-Animation-Engine.git"
$Facial = "https://github.com/lyingtiger88/BDFR_FacialAnimation.git"
$Performance = "https://github.com/lyingtiger88/BDFR_PerformanceCapture.git"
$Work = Join-Path $env:TEMP "BDFR-Legacy-Migration"
New-Item -ItemType Directory -Force -Path $Work | Out-Null

function Invoke-Git {
    param([string]$Directory, [string[]]$Arguments)
    & git -C $Directory @Arguments
    if ($LASTEXITCODE -ne 0) { throw "git failed: $($Arguments -join ' ')" }
}
function Invoke-Gh {
    param([string[]]$Arguments)
    & gh @Arguments
    if ($LASTEXITCODE -ne 0) { throw "gh failed: $($Arguments -join ' ')" }
}

# Require authenticated GitHub CLI and Git before making changes.
git --version
if ($LASTEXITCODE -ne 0) { throw "Git is required." }
gh auth status
if ($LASTEXITCODE -ne 0) { throw "Run 'gh auth login' first." }

# Mirror-clone source repositories locally, then push only explicitly mapped refs.
$FacialDir = Join-Path $Work "BDFR_FacialAnimation.git"
$PerformanceDir = Join-Path $Work "BDFR_PerformanceCapture.git"
if (!(Test-Path $FacialDir)) { & git clone --mirror $Facial $FacialDir; if ($LASTEXITCODE -ne 0) { throw "FacialAnimation mirror clone failed." } }
if (!(Test-Path $PerformanceDir)) { & git clone --mirror $Performance $PerformanceDir; if ($LASTEXITCODE -ne 0) { throw "PerformanceCapture mirror clone failed." } }

# Preserve exact original commit objects and ancestry under isolated legacy branch names.
Invoke-Git $FacialDir @("push", $Destination, "refs/heads/main:refs/heads/legacy/facial-animation/main")
Invoke-Git $PerformanceDir @("push", $Destination, "refs/heads/main:refs/heads/legacy/performance-capture/main")
Invoke-Git $PerformanceDir @("push", $Destination, "refs/heads/architecture/unified-human-engine-v1:refs/heads/legacy/performance-capture/architecture/unified-human-engine-v1")

# Preserve the original release tag targets, namespaced to avoid collisions.
Invoke-Git $FacialDir @("push", $Destination, "refs/tags/studio-test-latest:refs/tags/legacy/facial-animation/studio-test-latest")
Invoke-Git $FacialDir @("push", $Destination, "refs/tags/hardware-test-latest:refs/tags/legacy/facial-animation/hardware-test-latest")

# Download and re-upload the three release assets. SHA-256 is checked before upload.
$StudioDir = Join-Path $Work "studio-release"
$HardwareDir = Join-Path $Work "hardware-release"
New-Item -ItemType Directory -Force -Path $StudioDir, $HardwareDir | Out-Null
Invoke-Gh @("release", "download", "studio-test-latest", "--repo", "lyingtiger88/BDFR_FacialAnimation", "--dir", $StudioDir)
Invoke-Gh @("release", "download", "hardware-test-latest", "--repo", "lyingtiger88/BDFR_FacialAnimation", "--dir", $HardwareDir)

$Expected = @{
    "BDFR_Studio_Windows.zip" = "2996fe814603e97123a07d90b42a2d6cd510fa6baa4ac510aac2f1536bab3422"
    "BDFR_Android_Hardware_Test.apk" = "ff86dc80d6ddae2854818e8d4260e4603728ba0d2bcf33f30f1a7ba7002fcdb9"
    "BDFR_Windows_Hardware_Test_Kit.zip" = "076250ea326a63119d75afe9b541edb235f4ab9ca5a045f566c0760f0b1374b6"
}
foreach ($entry in $Expected.GetEnumerator()) {
    $matches = Get-ChildItem -Path $Work -Filter $entry.Key -Recurse -File
    if ($matches.Count -ne 1) { throw "Expected exactly one copy of $($entry.Key); found $($matches.Count)." }
    $actual = (Get-FileHash -Algorithm SHA256 $matches[0].FullName).Hash.ToLowerInvariant()
    if ($actual -ne $entry.Value) { throw "SHA-256 mismatch for $($entry.Key): $actual" }
}

Invoke-Gh @("release", "create", "legacy/facial-animation/studio-test-latest", (Join-Path $StudioDir "BDFR_Studio_Windows.zip"), "--repo", "lyingtiger88/BDFR-Human-Animation-Engine", "--title", "Legacy — BDFR Studio Latest Test Build", "--notes", "Migrated from BDFR_FacialAnimation. Original tag and asset SHA-256 are preserved in migration/LEGACY_GIT_AND_RELEASE_ARCHIVE.json.", "--prerelease")
Invoke-Gh @("release", "create", "legacy/facial-animation/hardware-test-latest", (Join-Path $HardwareDir "BDFR_Android_Hardware_Test.apk"), (Join-Path $HardwareDir "BDFR_Windows_Hardware_Test_Kit.zip"), "--repo", "lyingtiger88/BDFR-Human-Animation-Engine", "--title", "Legacy — BDFR Hardware Test", "--notes", "Migrated from BDFR_FacialAnimation. Original tag and asset SHA-256 are preserved in migration/LEGACY_GIT_AND_RELEASE_ARCHIVE.json.", "--prerelease")

# Final ref and release checks.
git ls-remote $Destination "refs/heads/legacy/*" "refs/tags/legacy/*"
if ($LASTEXITCODE -ne 0) { throw "Destination ref verification failed." }
Invoke-Gh @("release", "view", "legacy/facial-animation/studio-test-latest", "--repo", "lyingtiger88/BDFR-Human-Animation-Engine")
Invoke-Gh @("release", "view", "legacy/facial-animation/hardware-test-latest", "--repo", "lyingtiger88/BDFR-Human-Animation-Engine")

Write-Host "Legacy Git refs and release assets migrated and verified. Keep source repositories until this script completes successfully."
