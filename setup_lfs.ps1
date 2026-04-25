# ==========================================================
# Aliyun OSS Git-LFS Setup Script (Corrected Version)
# ==========================================================
$BUCKET = "my-ue5-lfs"
$ENDPOINT = "oss-cn-shenzhen.aliyuncs.com"
$REGION = "oss-cn-shenzhen"

Set-Location $PSScriptRoot

Write-Host "--- Start Aliyun OSS LFS Config ---" -ForegroundColor Cyan

# 1. Download lfs-s3.exe (Updated URL)
if (-not (Test-Path "lfs-s3.exe")) {
    Write-Host "Downloading lfs-s3.exe from nicolas-graves/lfs-s3..."
    $url = "https://github.com/nicolas-graves/lfs-s3/releases/download/0.2.2/lfs-s3-windows.exe"
    try {
        Invoke-WebRequest -Uri $url -OutFile "lfs-s3.exe"
        Write-Host "Download successful." -ForegroundColor Green
    } catch {
        Write-Host "Download failed! Please download manually from: $url" -ForegroundColor Red
        pause
        exit
    }
}

# 2. Git Config
Write-Host "Configuring Git..."
git config lfs.customtransfer.lfs-s3.path "$PSScriptRoot\lfs-s3.exe"
git config lfs.standalonetransferagent lfs-s3

# 3. Credentials
Write-Host ""
Write-Host "Please enter your Aliyun AccessKey:" -ForegroundColor Yellow
$keyId = Read-Host "AccessKey ID"
$keySecret = Read-Host "AccessKey Secret"

if ($keyId -and $keySecret) {
    # 使用该适配器推荐的配置格式
    $args = "--access_key_id=$keyId --secret_access_key=$keySecret --bucket=$BUCKET --endpoint=https://$ENDPOINT --region=$REGION"
    git config lfs.customtransfer.lfs-s3.args "$args"
    
    Write-Host ""
    Write-Host "Success! Configuration complete." -ForegroundColor Green
    Write-Host "Now you can try 'git push origin master'."
} else {
    Write-Host "Error: Key ID and Secret are required." -ForegroundColor Red
}

pause
