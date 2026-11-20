@echo off
setlocal enabledelayedexpansion

set "MODE=%~1"
if "%MODE%"=="" set "MODE=build"

set "COMPILER="

where gcc >nul 2>nul
if %errorlevel%==0 set "COMPILER=gcc"

if "%COMPILER%"=="" (
    where clang >nul 2>nul
    if %errorlevel%==0 set "COMPILER=clang"
)

if "%COMPILER%"=="" (
    where cl >nul 2>nul
    if %errorlevel%==0 set "COMPILER=msvc"
)

if "%COMPILER%"=="" (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "!VSWHERE!" (
        for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
            set "VSINSTALL=%%i"
        )
        if defined VSINSTALL (
            if exist "!VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat" (
                call "!VSINSTALL!\VC\Auxiliary\Build\vcvars64.bat" >nul
                set "COMPILER=msvc"
            )
        )
    )
)

if "%COMPILER%"=="" (
    echo [HATA] C derleyicisi bulunamadi ^(gcc, clang veya Visual Studio MSVC gerekli^).
    exit /b 1
)

echo [INFO] Derleyici: %COMPILER%

if /i "%MODE%"=="test" goto :run_tests

echo [INFO] 2048 derleniyor...
if "%COMPILER%"=="msvc" (
    cl /nologo /std:c11 /O2 /W4 /utf-8 /Fe:2048.exe src\main.c src\game.c src\render.c src\input.c src\storage.c
    del /q *.obj 2>nul
) else (
    %COMPILER% -std=c11 -O2 -Wall -Wextra -Wpedantic -o 2048.exe src\main.c src\game.c src\render.c src\input.c src\storage.c
)

if %errorlevel% neq 0 exit /b %errorlevel%
echo [OK] Basariyla derlendi: 2048.exe
exit /b 0

:run_tests
echo [INFO] Birim testleri derleniyor...
if "%COMPILER%"=="msvc" (
    cl /nologo /std:c11 /O2 /W4 /utf-8 /Fe:test_game.exe tests\test_game.c src\game.c src\storage.c
    del /q *.obj 2>nul
) else (
    %COMPILER% -std=c11 -O2 -Wall -Wextra -Wpedantic -o test_game.exe tests\test_game.c src\game.c src\storage.c
)

if %errorlevel% neq 0 exit /b %errorlevel%
echo [INFO] Testler calistiriliyor...
test_game.exe
exit /b %errorlevel%
