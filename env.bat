@echo off
REM Fanatec Wheel Environment Setup Script for Windows
REM This script sets up the complete development environment for Zephyr RTOS
REM Run with administrator privileges for automatic installation

REM Use setlocal only for the setup phase, then endlocal and set variables globally
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "VENV_DIR=%SCRIPT_DIR%\.venv"
set "ZEPHYR_BASE=%SCRIPT_DIR%\3rd_party\zephyr\zephyr"

echo ==========================================
echo Fanatec Wheel Environment Setup
echo ==========================================
echo.

REM Check winget availability
where winget >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo WARNING: winget is not available. Automatic installation will not work.
    echo Please install App Installer from Microsoft Store or install prerequisites manually.
    set WINGET_AVAILABLE=0
) else (
    set WINGET_AVAILABLE=1
)

REM Check Python
echo [1/7] Checking Python installation...
where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Python is not installed.
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing Python 3.14 via winget...
        winget install --id Python.Python.3.14 --silent --accept-package-agreements --accept-source-agreements
        if !ERRORLEVEL! equ 0 (
            echo [OK] Python installed. Please close and reopen this terminal, then run this script again.
        ) else (
            echo ERROR: Failed to install Python automatically.
            echo Please install Python 3.14 manually from https://www.python.org/
            pause
            exit /b 1
        )
    ) else (
        echo ERROR: Python is not installed.
        echo Please install Python 3.10 to 3.14 from https://www.python.org/
        echo Make sure to check "Add Python to PATH" during installation.
        pause
        exit /b 1
    )
)

for /f "tokens=2" %%i in ('python --version 2^>^&1') do set PYTHON_VERSION=%%i
echo [OK] Found Python %PYTHON_VERSION%

REM Check CMake
echo.
echo [2/7] Checking CMake installation...
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo CMake is not installed.
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing CMake via winget...
        winget install --id Kitware.CMake --silent --accept-package-agreements --accept-source-agreements
        if !ERRORLEVEL! equ 0 (
            echo [OK] CMake installed. Please close and reopen this terminal, then run this script again.
        ) else (
            echo ERROR: Failed to install CMake automatically.
            echo Please install CMake manually from https://cmake.org/download/
            pause
            exit /b 1
        )
    ) else (
        echo ERROR: CMake is not installed.
        echo Please install CMake 3.20 or later from https://cmake.org/download/
        pause
        exit /b 1
    )
)

for /f "tokens=3" %%i in ('cmake --version 2^>^&1 ^| findstr /C:"cmake version"') do set CMAKE_VERSION=%%i
echo [OK] Found CMake %CMAKE_VERSION%

REM Check Ninja
echo.
echo [3/7] Checking Ninja build system...
where ninja >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Ninja is not installed.
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing Ninja via winget...
        winget install --id Ninja-build.Ninja --silent --accept-package-agreements --accept-source-agreements
        if !ERRORLEVEL! equ 0 (
            echo [OK] Ninja installed. Please close and reopen this terminal, then run this script again.
        ) else (
            echo WARNING: Failed to install Ninja automatically ^(optional but recommended^).
            echo You can install it manually from: https://github.com/ninja-build/ninja/releases
        )
    ) else (
        echo WARNING: Ninja is not installed ^(optional but recommended^).
        echo Download from: https://github.com/ninja-build/ninja/releases
    )
) else (
    for /f %%i in ('ninja --version 2^>^&1') do set NINJA_VERSION=%%i
    echo [OK] Found Ninja !NINJA_VERSION!
)

REM Verify versions
echo.
echo [4/8] Verifying tool versions...
echo [OK] Python %PYTHON_VERSION%
echo [OK] CMake %CMAKE_VERSION%
if defined NINJA_VERSION (
    echo [OK] Ninja !NINJA_VERSION!
)

REM Check additional tools (gperf, dtc, wget, 7zip, git)
echo.
echo [5/8] Checking additional tools...
set "MISSING_TOOLS="

where gperf >nul 2>nul
if %ERRORLEVEL% neq 0 (
    set "MISSING_TOOLS=!MISSING_TOOLS! gperf"
)

where dtc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    set "MISSING_TOOLS=!MISSING_TOOLS! dtc"
)

where wget >nul 2>nul
if %ERRORLEVEL% neq 0 (
    set "MISSING_TOOLS=!MISSING_TOOLS! wget"
)

where 7z >nul 2>nul
if %ERRORLEVEL% neq 0 (
    set "MISSING_TOOLS=!MISSING_TOOLS! 7zip"
)

where git >nul 2>nul
if %ERRORLEVEL% neq 0 (
    set "MISSING_TOOLS=!MISSING_TOOLS! git"
)

if not "!MISSING_TOOLS!"=="" (
    echo Missing tools:!MISSING_TOOLS!
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing missing tools via winget...
        echo This may take a few minutes...

        echo !MISSING_TOOLS! | findstr "gperf" >nul && (
            echo Installing gperf...
            winget install --id oss-winget.gperf --silent --accept-package-agreements --accept-source-agreements
        )

        echo !MISSING_TOOLS! | findstr "dtc" >nul && (
            echo Installing dtc...
            winget install --id oss-winget.dtc --silent --accept-package-agreements --accept-source-agreements
        )

        echo !MISSING_TOOLS! | findstr "wget" >nul && (
            echo Installing wget...
            winget install --id wget --silent --accept-package-agreements --accept-source-agreements
        )

        echo !MISSING_TOOLS! | findstr "7zip" >nul && (
            echo Installing 7zip...
            winget install --id 7zip.7zip --silent --accept-package-agreements --accept-source-agreements
        )

        echo !MISSING_TOOLS! | findstr "git" >nul && (
            echo Installing git...
            winget install --id Git.Git --silent --accept-package-agreements --accept-source-agreements
        )

        echo [OK] Additional tools installed. Please close and reopen this terminal, then run this script again.
    ) else (
        echo WARNING: Some tools are missing. They may be needed for full functionality.
        echo Consider installing: gperf, dtc, wget, 7zip, git
    )
) else (
    echo [OK] All additional tools are available
)

REM Check Rust toolchain (optional, for Rust integration)
echo.
echo [6/10] Checking Rust toolchain ^(optional^)...
where rustc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Rust is not installed.
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing Rust via winget...
        winget install --id Rustlang.Rustup --silent --accept-package-agreements --accept-source-agreements
        if !ERRORLEVEL! equ 0 (
            echo [OK] Rust installed. Please close and reopen this terminal, then run this script again.
            echo After reopening, run: rustup target add thumbv7em-none-eabi
        ) else (
            echo WARNING: Failed to install Rust automatically.
            echo You can install it manually from: https://rustup.rs/
        )
    ) else (
        echo WARNING: Rust is not installed ^(optional for Rust integration^).
        echo Download from: https://rustup.rs/
    )
) else (
    for /f "tokens=2" %%i in ('rustc --version 2^>^&1') do set RUST_VERSION=%%i
    echo [OK] Found Rust !RUST_VERSION!

    REM Check if ARM target is installed
    rustup target list --installed 2>nul | findstr "thumbv7em-none-eabi" >nul
    if !ERRORLEVEL! neq 0 (
        echo Installing ARM Cortex-M target for Rust...
        rustup target add thumbv7em-none-eabi
        echo [OK] ARM target installed
    ) else (
        echo [OK] ARM target ^(thumbv7em-none-eabi^) already installed
    )
)

REM Check LLVM/Clang (required for Rust bindgen)
echo.
echo [7/10] Checking LLVM/Clang ^(required for Rust bindgen^)...
where clang >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo LLVM/Clang is not installed.
    if !WINGET_AVAILABLE! equ 1 (
        echo Installing LLVM via winget...
        winget install --id LLVM.LLVM --source winget --silent --accept-package-agreements --accept-source-agreements
        if !ERRORLEVEL! equ 0 (
            echo [OK] LLVM installed. Please close and reopen this terminal, then run this script again.
        ) else (
            echo WARNING: Failed to install LLVM automatically.
            echo You can install it manually from: https://releases.llvm.org/
        )
    ) else (
        echo WARNING: LLVM/Clang is not installed ^(required for Rust bindgen^).
        echo Download from: https://releases.llvm.org/
    )
) else (
    for /f "tokens=3" %%i in ('clang --version 2^>^&1 ^| findstr /C:"clang version"') do set CLANG_VERSION=%%i
    echo [OK] Found LLVM/Clang !CLANG_VERSION!
)

REM Check Zephyr SDK
echo.
echo [8/10] Checking Zephyr SDK...
if not defined ZEPHYR_SDK_INSTALL_DIR (
    echo WARNING: ZEPHYR_SDK_INSTALL_DIR environment variable is not set.
    echo.
    echo Please download and install the Zephyr SDK:
    echo   https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html
    echo.
    echo Recommended installation:
    echo   1. Download: https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.4/zephyr-sdk-0.17.4_windows-x86_64.7z
    echo   2. Extract to: C:\zephyr-sdk-0.17.4
    echo   3. Run setup: cd C:\zephyr-sdk-0.17.4 ^&^& setup.cmd
    echo   4. Set user environment variable:
    echo      setx ZEPHYR_SDK_INSTALL_DIR "C:\zephyr-sdk-0.17.4"
    echo.
    echo After setting ZEPHYR_SDK_INSTALL_DIR, close and reopen this terminal.
    echo.
    set /p CONTINUE="Continue anyway? (y/N): "
    if /i not "!CONTINUE!"=="y" exit /b 1
) else (
    if exist "%ZEPHYR_SDK_INSTALL_DIR%" (
        echo [OK] Found Zephyr SDK at: %ZEPHYR_SDK_INSTALL_DIR%
    ) else (
        echo ERROR: ZEPHYR_SDK_INSTALL_DIR points to non-existent directory: %ZEPHYR_SDK_INSTALL_DIR%
        pause
        exit /b 1
    )
)

REM Create/Update Python virtual environment
echo.
echo [9/10] Setting up Python virtual environment...
if exist "%VENV_DIR%" (
    echo Existing virtual environment found. Removing for clean setup...
    rd /s /q "%VENV_DIR%" 2>nul
)
echo Creating virtual environment...
python -m venv "%VENV_DIR%"
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to create virtual environment.
    pause
    exit /b 1
)
echo [OK] Virtual environment created at: %VENV_DIR%

REM Activate virtual environment
echo Activating virtual environment...
call "%VENV_DIR%\Scripts\activate.bat"
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to activate virtual environment.
    pause
    exit /b 1
)

REM Upgrade pip and install base tools
echo Upgrading pip, setuptools, pymodbus and wheel...
python -m pip install --upgrade pip setuptools pymodbus wheel --quiet

REM Install Zephyr requirements
echo.
echo [10/10] Installing Zephyr Python dependencies...

if exist "%ZEPHYR_BASE%\scripts\requirements-base.txt" (
    echo Installing base requirements...
    pip install -r "%ZEPHYR_BASE%\scripts\requirements-base.txt"
    echo [OK] Base requirements installed

    if exist "%ZEPHYR_BASE%\scripts\requirements-west.txt" (
        echo Installing west requirements...
        pip install -r "%ZEPHYR_BASE%\scripts\requirements-west.txt"
        echo [OK] West requirements installed
    )

    if exist "%ZEPHYR_BASE%\scripts\requirements-build-test.txt" (
        echo Installing build test requirements...
        pip install -r "%ZEPHYR_BASE%\scripts\requirements-build-test.txt"
        echo [OK] Build test requirements installed
    )

    if exist "%ZEPHYR_BASE%\scripts\requirements-run-test.txt" (
        echo Installing run test requirements...
        pip install -r "%ZEPHYR_BASE%\scripts\requirements-run-test.txt"
        echo [OK] Run test requirements installed
    )

    if exist "%ZEPHYR_BASE%\scripts\requirements-extras.txt" (
        echo Installing extra requirements...
        pip install -r "%ZEPHYR_BASE%\scripts\requirements-extras.txt"
        echo [OK] Extra requirements installed
    )

    if exist "%ZEPHYR_BASE%\scripts\requirements-compliance.txt" (
        echo Installing compliance requirements...
        pip install -r "%ZEPHYR_BASE%\scripts\requirements-compliance.txt"
        echo [OK] Compliance requirements installed
    )

    echo.
    echo Ensuring critical core packages are installed...
    pip install west pyelftools pyyaml pykwalify jsonschema packaging progress canopen gitlint cbor2 intelhex ply pyserial cryptography requests click
    echo [OK] Core packages verified

    echo.
    echo Installing memory report tools ^(puncover, anytree, plotly^)...
    pip install puncover anytree plotly
    echo [OK] Memory report tools installed

    echo.
    echo [OK] All Zephyr requirements installed successfully
) else (
    echo WARNING: Could not find Zephyr requirements files.
    echo Installing west and core packages manually...
    pip install west pyelftools pyyaml pykwalify jsonschema packaging progress canopen gitlint cbor2 intelhex ply pyserial cryptography requests click
)

REM Prepare environment variables to export
echo.
echo Setting up Zephyr environment variables...

REM Source Zephyr environment if available
if exist "%ZEPHYR_BASE%\zephyr-env.cmd" (
    call "%ZEPHYR_BASE%\zephyr-env.cmd"
    echo [OK] Sourced Zephyr environment
)

REM Run west update to fetch all dependencies
echo.
echo Running 'west update' to fetch all Zephyr dependencies...
echo This may take several minutes on first run...
"%VENV_DIR%\Scripts\west.exe" update
if %ERRORLEVEL% equ 0 (
    echo [OK] West update completed successfully
) else (
    echo WARNING: West update encountered issues. You may need to run it manually.
)

REM End the local scope but preserve important variables
endlocal & (
    set "ZEPHYR_BASE=%ZEPHYR_BASE%"
    set "ZEPHYR_TOOLCHAIN_VARIANT=zephyr"
    set "PATH=%VENV_DIR%\Scripts;%PATH%"
    set "VIRTUAL_ENV=%VENV_DIR%"
)

echo.
echo ==========================================
echo [OK] Environment setup complete!
echo ==========================================
echo.
echo Python packages installed successfully.
echo Virtual environment created at: %VENV_DIR%
echo West dependencies fetched.
echo.
echo Environment variables that will be set:
echo   ZEPHYR_BASE=%ZEPHYR_BASE%
echo   ZEPHYR_TOOLCHAIN_VARIANT=%ZEPHYR_TOOLCHAIN_VARIANT%
if defined ZEPHYR_SDK_INSTALL_DIR (
    echo   ZEPHYR_SDK_INSTALL_DIR=%ZEPHYR_SDK_INSTALL_DIR%
)
echo.
echo ============================================================
echo IMPORTANT: ACTIVATE THE VIRTUAL ENVIRONMENT WITH THE FOLLOWING COMMANDS:
echo ============================================================
echo.
echo   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force; .\.venv\Scripts\Activate.ps1
echo.
echo After activation, your prompt will show: ^(.venv^) PS ...
echo.
echo   1. Build your application:
echo      west build -p -b fk723m1_zgt6 app
echo   2. Flash to device:
echo      west flash
echo.
echo ============================================================
echo MEMORY REPORT TOOLS
echo ============================================================
echo.
echo   Generate RAM/ROM report:
echo      west build -d build -t ram_report
echo      west build -d build -t rom_report
echo.
echo   Interactive memory analysis with Puncover:
echo      PowerShell: puncover --elf_file build\zephyr\zephyr.elf --gcc_tools_base "$env:ZEPHYR_SDK_INSTALL_DIR/arm-zephyr-eabi/bin/arm-zephyr-eabi-" --build_dir build
echo      CMD:        puncover --elf_file build\zephyr\zephyr.elf --gcc_tools_base %%ZEPHYR_SDK_INSTALL_DIR%%/arm-zephyr-eabi/bin/arm-zephyr-eabi- --build_dir build
echo      Then open: http://localhost:5000
echo.

