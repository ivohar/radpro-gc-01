# PowerShell script to vendor CMSIS headers from STMicroelectronics GitHub repositories
# This downloads the required CMSIS headers to remove PlatformIO dependency

$ErrorActionPreference = "Stop"

$VENDOR_DIR = "vendor/cmsis"
$REPO_BASE_URL = "https://github.com/STMicroelectronics"

# CMSIS repositories and their tags/versions to download
$CMSIS_REPOS = @{
    # CMSIS Core (ARM Cortex-M core definitions)
    "cmsis-core" = @{
        "tag" = "v5.6.0_cm0"
        "paths" = @("CMSIS/Core/Include")
        "dest" = "core"
    }
    # STM32F0 family
    "cmsis_device_f0" = @{
        "tag" = "v2.3.7"
        "paths" = @("Include", "Source/Templates")
        "dest" = "stm32f0"
    }
    # STM32F1 family
    "cmsis_device_f1" = @{
        "tag" = "v4.3.4"
        "paths" = @("Include", "Source/Templates")
        "dest" = "stm32f1"
    }
    # STM32F2 family
    "cmsis_device_f2" = @{
        "tag" = "v2.2.6"
        "paths" = @("Include", "Source/Templates")
        "dest" = "stm32f2"
    }
    # STM32G0 family
    "cmsis_device_g0" = @{
        "tag" = "v1.4.5"
        "paths" = @("Include", "Source/Templates")
        "dest" = "stm32g0"
    }
    # STM32L4 family
    "cmsis_device_l4" = @{
        "tag" = "v1.7.3"
        "paths" = @("Include", "Source/Templates")
        "dest" = "stm32l4"
    }
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Vendoring CMSIS Headers" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Create vendor directory
Write-Host "Creating vendor directory: $VENDOR_DIR" -ForegroundColor Green
New-Item -ItemType Directory -Force -Path $VENDOR_DIR | Out-Null

foreach ($repo in $CMSIS_REPOS.Keys) {
    $config = $CMSIS_REPOS[$repo]
    $tag = $config["tag"]
    $dest = "$VENDOR_DIR/$($config['dest'])"
    
    Write-Host ""
    Write-Host "Processing $repo ($tag)..." -ForegroundColor Yellow
    
    # Create destination directory
    New-Item -ItemType Directory -Force -Path $dest | Out-Null
    
    foreach ($path in $config["paths"]) {
        $url = "$REPO_BASE_URL/$repo/archive/refs/tags/$tag.zip"
        $zipFile = "$env:TEMP\$repo-$tag.zip"
        $extractDir = "$env:TEMP\$repo-extract"
        
        Write-Host "  Downloading $path..." -ForegroundColor Gray
        
        # Download the zip file
        try {
            Invoke-WebRequest -Uri $url -OutFile $zipFile -ErrorAction Stop
        } catch {
            Write-Host "  ERROR: Failed to download $url" -ForegroundColor Red
            Write-Host "  $_" -ForegroundColor Red
            continue
        }
        
        # Extract the zip file
        if (Test-Path $extractDir) {
            Remove-Item $extractDir -Recurse -Force
        }
        Expand-Archive -Path $zipFile -DestinationPath $extractDir -Force
        
        # Find the extracted folder (it will be named like "repo-tag")
        $extractedFolder = Get-ChildItem $extractDir | Select-Object -First 1
        $sourcePath = Join-Path $extractedFolder.FullName $path
        
        if (Test-Path $sourcePath) {
            # Copy the contents
            $destPath = Join-Path $dest (Split-Path $path -Leaf)
            Write-Host "  Copying to $destPath..." -ForegroundColor Gray
            Copy-Item -Path $sourcePath -Destination $dest -Recurse -Force
        } else {
            Write-Host "  WARNING: Path $path not found in archive" -ForegroundColor Yellow
        }
        
        # Cleanup
        Remove-Item $zipFile -Force -ErrorAction SilentlyContinue
        Remove-Item $extractDir -Recurse -Force -ErrorAction SilentlyContinue
    }
    
    Write-Host "  Completed $repo" -ForegroundColor Green
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  CMSIS Headers Vendored Successfully!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Review the vendored headers in $VENDOR_DIR" -ForegroundColor White
Write-Host "  2. Update CMakeLists.txt to use vendored headers" -ForegroundColor White
Write-Host "  3. Remove PLATFORMIO_PACKAGES_DIR references" -ForegroundColor White
Write-Host ""
