Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

& .\build.ps1

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "Running build.ps1 failed!"
  Pop-Location
  exit 1
}

Set-Location $build_dir

Write-Output ""
Write-Output ""
Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output ">>>>>>>>>>>>>>>>>>>>>>> Starting debug test run >>> $time_string"

Set-Location Debug

& .\doubly_linked_list_app.exe

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "Debug test run failed!"
  Pop-Location
  exit 1
}

$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output ">>>>>>>>>>>>>>>>>>>>>>> Completed debug test run >>> $time_string"
Write-Output ""
Write-Output ""
Write-Output ""

Pop-Location
Push-Location .
Set-Location $build_dir

Write-Output ""
Write-Output ""
Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output ">>>>>>>>>>>>>>>>>>>>>>> Starting release test run >>> $time_string"

Set-Location Release

& .\doubly_linked_list_app.exe

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "Release test run failed!"
  Pop-Location
  exit 1
}

$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output ">>>>>>>>>>>>>>>>>>>>>>> Completed release test run >>> $time_string"
Write-Output ""
Write-Output ""
Write-Output ""

Pop-Location