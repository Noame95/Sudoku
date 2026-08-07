@echo off
setlocal

set "PIO=%USERPROFILE%\.platformio\penv\Scripts\platformio.exe"
set "PROJECT=%~dp0"
set "CLION_EXE=C:\Program Files\JetBrains\CLion 2026.2.0.1\bin\clion64.exe"

echo Refreshing PlatformIO project...
"%PIO%" project init --ide clion

echo Building project...
"%PIO%" run

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

echo Restarting CLion...

start "" powershell.exe -NoProfile -WindowStyle Hidden -Command ^
"Start-Sleep -Seconds 1; Stop-Process -Name clion64 -Force -ErrorAction SilentlyContinue; Start-Sleep -Seconds 2; Start-Process -FilePath '%CLION_EXE%' -ArgumentList '%PROJECT%'"

endlocal