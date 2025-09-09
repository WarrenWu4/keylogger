@echo off
setlocal

:: CONFIGURATION
set "APP_NAME=Keydisplay"
set "APP_EXE=keylogger.exe"
set "SOURCE_DIR=%~dp0"
set "INSTALL_DIR=%ProgramFiles%\%APP_NAME%"
set "RESOURCES_DIR=resources"
set "APP_RES=app.res"
set "START_MENU=%AppData%\Microsoft\Windows\Start Menu\Programs"

echo Installing "%APP_NAME%" from "%SOURCE_DIR%"...

:: Create Installation Directory
if not exist "%INSTALL_DIR%" (
    echo Creating installation directory...
    mkdir "%INSTALL_DIR%"
) else (
    echo Existing installation found. Overwriting...
)

:: Copy executable
echo Copying executable...
copy "%SOURCE_DIR%build\%APP_EXE%" "%INSTALL_DIR%\%APP_EXE%" >nul 

:: Copy resources
echo Copying resources...
copy "%SOURCE_DIR%build\%APP_RES%" "%INSTALL_DIR%\%APP_RES%" >nul
xcopy "%SOURCE_DIR%build\%RESOURCES_DIR%" "%INSTALL_DIR%\%RESOURCES_DIR%" /E /I /Y >nul

:: Create uninstall script
echo Creating uninstall script...
(
    echo @echo off
    echo echo Uninstalling "%APP_NAME%" ...
    echo rmdir /S /Q "%INSTALL_DIR%"
    del "%START_MENU%\%APP_NAME%.lnk" >nul 2>&1
    echo echo Uninstallation complete.
    echo pause
) > "%INSTALL_DIR%\uninstall.bat"

:: Create start menu shortcut
powershell -command ^
    "$s=(New-Object -COM WScript.Shell).CreateShortcut('%START_MENU%\%APP_NAME%.lnk');" ^
    "$s.TargetPath='%INSTALL_DIR%\%APP_EXE%';" ^
    "$s.IconLocation='%INSTALL_DIR%\%APP_EXE%';" ^
    "$s.Save()"

echo Installation complete.
echo You can find the application in "%INSTALL_DIR%".
echo You can uninstall by running uninstall.bat in the installation directory
pause
endlocal