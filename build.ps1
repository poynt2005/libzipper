Write-Host "[Builder][Info] ***** zipper library builder *****" -ForegroundColor DarkGreen
Write-Host "[Builder][Info] Make sure you have nodejs(with npm) 20.x, Microsoft C++ Build Tools(msbuild >= v143) and python installed on this machine to complete the building"  -ForegroundColor DarkGreen


Write-Host "[Builder][Info] Build core lib" -ForegroundColor DarkGreen
Push-Location .\\libzipper
& .\\build.ps1
Pop-Location

Write-Host "[Builder][Info] Build zipperjs" -ForegroundColor DarkGreen
Push-Location .\\zipperjs
& npm i
& npm run build
Pop-Location

Write-Host "[Builder][Info] Build done" -ForegroundColor DarkGreen