#!/bin/bash
# Fanatec Wheel Environment Setup Script for Linux/macOS
# This script sets up the complete development environment for Zephyr RTOS
# Run with sudo for automatic installation: sudo ./env.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="${SCRIPT_DIR}/.venv"
ZEPHYR_BASE="${SCRIPT_DIR}/3rd_party/zephyr/zephyr"

echo "=========================================="
echo "Fanatec Wheel Environment Setup"
echo "=========================================="
echo ""

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Detect OS type
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS_TYPE="linux"
    # Detect if ARM64
    if [[ $(uname -m) == "aarch64" ]]; then
        IS_ARM64=true
    else
        IS_ARM64=false
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS_TYPE="macos"
    IS_ARM64=false
else
    OS_TYPE="unknown"
    IS_ARM64=false
fi

# Check and install dependencies on Linux
if [ "$OS_TYPE" == "linux" ]; then
    echo "[1/8] Checking system dependencies..."

    MISSING_DEPS=""
    command_exists git || MISSING_DEPS="$MISSING_DEPS git"
    command_exists cmake || MISSING_DEPS="$MISSING_DEPS cmake"
    command_exists ninja || MISSING_DEPS="$MISSING_DEPS ninja-build"
    command_exists gperf || MISSING_DEPS="$MISSING_DEPS gperf"
    command_exists ccache || MISSING_DEPS="$MISSING_DEPS ccache"
    command_exists dfu-util || MISSING_DEPS="$MISSING_DEPS dfu-util"
    command_exists dtc || MISSING_DEPS="$MISSING_DEPS device-tree-compiler"
    command_exists wget || MISSING_DEPS="$MISSING_DEPS wget"
    command_exists file || MISSING_DEPS="$MISSING_DEPS file"
    command_exists make || MISSING_DEPS="$MISSING_DEPS make"

    if [ -n "$MISSING_DEPS" ]; then
        echo "Missing dependencies:$MISSING_DEPS"

        if [ "$EUID" -eq 0 ] || command_exists sudo; then
            echo "Installing missing dependencies..."

            if [ "$IS_ARM64" = true ]; then
                # ARM64 systems - omit gcc-multilib and g++-multilib
                INSTALL_CMD="apt install --no-install-recommends -y git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk xz-utils file make gcc libsdl2-dev libmagic1 cryptography requests"
            else
                # x86_64 systems - include multilib
                INSTALL_CMD="apt install --no-install-recommends -y git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 cryptography requests"
            fi

            if [ "$EUID" -eq 0 ]; then
                apt update && $INSTALL_CMD
            else
                sudo apt update && sudo $INSTALL_CMD
            fi

            echo "[OK] Dependencies installed"
        else
            echo "ERROR: Missing dependencies and cannot install automatically."
            echo "Please run with sudo or install manually:"
            if [ "$IS_ARM64" = true ]; then
                echo "  sudo apt install --no-install-recommends git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk xz-utils file make gcc libsdl2-dev libmagic1"
            else
                echo "  sudo apt install --no-install-recommends git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1"
            fi
            exit 1
        fi
    else
        echo "[OK] All system dependencies are installed"
    fi
elif [ "$OS_TYPE" == "macos" ]; then
    echo "[1/8] Checking macOS dependencies..."
    if ! command_exists brew; then
        echo "ERROR: Homebrew is not installed."
        echo "Please install Homebrew from https://brew.sh/"
        echo "Then install dependencies: brew install cmake ninja gperf ccache dfu-util dtc wget python3"
        exit 1
    fi

    MISSING_DEPS=""
    command_exists cmake || MISSING_DEPS="$MISSING_DEPS cmake"
    command_exists ninja || MISSING_DEPS="$MISSING_DEPS ninja"
    command_exists gperf || MISSING_DEPS="$MISSING_DEPS gperf"
    command_exists ccache || MISSING_DEPS="$MISSING_DEPS ccache"
    command_exists dfu-util || MISSING_DEPS="$MISSING_DEPS dfu-util"
    command_exists dtc || MISSING_DEPS="$MISSING_DEPS dtc"
    command_exists wget || MISSING_DEPS="$MISSING_DEPS wget"

    if [ -n "$MISSING_DEPS" ]; then
        echo "Installing missing dependencies via Homebrew:$MISSING_DEPS"
        brew install $MISSING_DEPS
        echo "[OK] Dependencies installed"
    else
        echo "[OK] All dependencies are installed"
    fi
else
    echo "[1/8] Unknown OS - skipping dependency check"
fi

# Check Python 3
echo ""
echo "[2/8] Checking Python installation..."
if ! command_exists python3; then
    if [ "$OS_TYPE" == "linux" ]; then
        echo "Installing Python 3..."
        if [ "$EUID" -eq 0 ]; then
            apt install -y python3 python3-dev python3-venv python3-tk
        else
            sudo apt install -y python3 python3-dev python3-venv python3-tk
        fi
    else
        echo "ERROR: Python 3 is not installed."
        echo "Please install Python 3.10 to 3.13"
        exit 1
    fi
fi

PYTHON_VERSION=$(python3 --version | awk '{print $2}')
echo "[OK] Found Python ${PYTHON_VERSION}"

# Verify versions
echo ""
echo "[3/8] Verifying tool versions..."
if command_exists cmake; then
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    echo "[OK] CMake ${CMAKE_VERSION}"
fi

if command_exists dtc; then
    DTC_VERSION=$(dtc --version 2>&1 | head -n1 | awk '{print $NF}')
    echo "[OK] Device Tree Compiler ${DTC_VERSION}"
fi

if command_exists ninja; then
    NINJA_VERSION=$(ninja --version)
    echo "[OK] Ninja ${NINJA_VERSION}"
fi

# Check Rust toolchain (optional, for Rust integration)
echo ""
echo "[5/10] Checking Rust toolchain (optional)..."
if ! command_exists rustc; then
    echo "Rust is not installed."
    echo "Installing Rust via rustup..."
    if curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y; then
        source "$HOME/.cargo/env"
        echo "[OK] Rust installed"
    else
        echo "WARNING: Failed to install Rust automatically."
        echo "You can install it manually from: https://rustup.rs/"
    fi
else
    RUST_VERSION=$(rustc --version | awk '{print $2}')
    echo "[OK] Found Rust ${RUST_VERSION}"
fi

# Check if ARM target is installed
if command_exists rustup; then
    if ! rustup target list --installed 2>/dev/null | grep -q "thumbv7em-none-eabi"; then
        echo "Installing ARM Cortex-M target for Rust..."
        rustup target add thumbv7em-none-eabi
        echo "[OK] ARM target installed"
    else
        echo "[OK] ARM target (thumbv7em-none-eabi) already installed"
    fi
fi

# Check LLVM/Clang (required for Rust bindgen)
echo ""
echo "[6/10] Checking LLVM/Clang (required for Rust bindgen)..."
if ! command_exists clang; then
    echo "LLVM/Clang is not installed."
    if [ "$OS_TYPE" == "linux" ]; then
        echo "Installing LLVM/Clang..."
        if [ "$EUID" -eq 0 ]; then
            apt install -y clang llvm
        else
            sudo apt install -y clang llvm
        fi
        echo "[OK] LLVM/Clang installed"
    elif [ "$OS_TYPE" == "macos" ]; then
        echo "Installing LLVM via Homebrew..."
        brew install llvm
        echo "[OK] LLVM installed"
    else
        echo "WARNING: LLVM/Clang is not installed (required for Rust bindgen)."
        echo "Please install LLVM manually."
    fi
else
    CLANG_VERSION=$(clang --version | head -n1 | awk '{print $3}')
    echo "[OK] Found LLVM/Clang ${CLANG_VERSION}"
fi

# Check Zephyr SDK
echo ""
echo "[7/10] Checking Zephyr SDK..."
if [ -z "${ZEPHYR_SDK_INSTALL_DIR}" ]; then
    echo "WARNING: ZEPHYR_SDK_INSTALL_DIR environment variable is not set."
    echo ""
    echo "Please download and install the Zephyr SDK:"
    echo "  https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html"
    echo ""
    echo "Recommended installation:"
    echo "  1. Download: wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.4/zephyr-sdk-0.17.4_linux-x86_64.tar.xz"
    echo "  2. Extract: tar xvf zephyr-sdk-0.17.4_linux-x86_64.tar.xz"
    echo "  3. Install: cd zephyr-sdk-0.17.4 && ./setup.sh"
    echo "  4. Set environment: export ZEPHYR_SDK_INSTALL_DIR=/path/to/zephyr-sdk-0.17.4"
    echo ""
    echo "For macOS (ARM): Use zephyr-sdk-0.17.4_macos-aarch64.tar.xz"
    echo "For macOS (Intel): Use zephyr-sdk-0.17.4_macos-x86_64.tar.xz"
    echo ""
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
else
    if [ -d "${ZEPHYR_SDK_INSTALL_DIR}" ]; then
        echo "[OK] Found Zephyr SDK at: ${ZEPHYR_SDK_INSTALL_DIR}"
    else
        echo "ERROR: ZEPHYR_SDK_INSTALL_DIR points to non-existent directory: ${ZEPHYR_SDK_INSTALL_DIR}"
        exit 1
    fi
fi

# Create/Update Python virtual environment
echo ""
echo "[8/10] Setting up Python virtual environment..."
if [ -d "${VENV_DIR}" ]; then
    echo "Existing virtual environment found. Removing for clean setup..."
    rm -rf "${VENV_DIR}"
fi
echo "Creating virtual environment..."
python3 -m venv "${VENV_DIR}"
echo "[OK] Virtual environment created at: ${VENV_DIR}"

# Activate virtual environment
echo "Activating virtual environment..."
source "${VENV_DIR}/bin/activate"

# Upgrade pip and install base tools
echo "Upgrading pip, setuptools, pymodbus and wheel..."
python -m pip install --upgrade pip setuptools pymodbus wheel -q

# Install Zephyr requirements
echo ""
echo "[9/10] Installing Zephyr Python dependencies..."

if [ -f "${ZEPHYR_BASE}/scripts/requirements-base.txt" ]; then
    echo "Installing base requirements..."
    pip install -r "${ZEPHYR_BASE}/scripts/requirements-base.txt"
    echo "[OK] Base requirements installed"

    if [ -f "${ZEPHYR_BASE}/scripts/requirements-west.txt" ]; then
        echo "Installing west requirements..."
        pip install -r "${ZEPHYR_BASE}/scripts/requirements-west.txt"
        echo "[OK] West requirements installed"
    fi

    if [ -f "${ZEPHYR_BASE}/scripts/requirements-build-test.txt" ]; then
        echo "Installing build test requirements..."
        pip install -r "${ZEPHYR_BASE}/scripts/requirements-build-test.txt"
        echo "[OK] Build test requirements installed"
    fi

    if [ -f "${ZEPHYR_BASE}/scripts/requirements-run-test.txt" ]; then
        echo "Installing run test requirements..."
        pip install -r "${ZEPHYR_BASE}/scripts/requirements-run-test.txt"
        echo "[OK] Run test requirements installed"
    fi

    if [ -f "${ZEPHYR_BASE}/scripts/requirements-extras.txt" ]; then
        echo "Installing extra requirements..."
        pip install -r "${ZEPHYR_BASE}/scripts/requirements-extras.txt"
        echo "[OK] Extra requirements installed"
    fi

    if [ -f "${ZEPHYR_BASE}/scripts/requirements-compliance.txt" ]; then
        echo "Installing compliance requirements..."
        pip install -r "${ZEPHYR_BASE}/scripts/requirements-compliance.txt"
        echo "[OK] Compliance requirements installed"
    fi

    echo ""
    echo "Ensuring critical core packages are installed..."
    pip install west pyelftools pyyaml pykwalify jsonschema packaging progress canopen gitlint cbor2 intelhex ply pyserial click
    echo "[OK] Core packages verified"

    echo ""
    echo "Installing memory report tools (puncover, anytree, plotly)..."
    pip install puncover anytree plotly
    echo "[OK] Memory report tools installed"

    echo ""
    echo "[OK] All Zephyr requirements installed successfully"
else
    echo "WARNING: Could not find Zephyr requirements files."
    echo "Installing west and core packages manually..."
    pip install west pyelftools pyyaml pykwalify jsonschema packaging progress canopen gitlint cbor2 intelhex ply pyserial click
fi

# Set Zephyr environment variables
echo ""
echo "[10/10] Setting up Zephyr environment variables..."
export ZEPHYR_BASE
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr

# Source Zephyr environment if available
if [ -f "${ZEPHYR_BASE}/zephyr-env.sh" ]; then
    source "${ZEPHYR_BASE}/zephyr-env.sh"
    echo "[OK] Sourced Zephyr environment"
fi

# Run west update to fetch all dependencies
echo ""
echo "Running 'west update' to fetch all Zephyr dependencies..."
echo "This may take several minutes on first run..."
if west update; then
    echo "[OK] West update completed successfully"
else
    echo "WARNING: West update encountered issues. You may need to run it manually."
fi

echo ""
echo "=========================================="
echo "[OK] Environment setup complete!"
echo "=========================================="
echo ""
echo "Environment variables set:"
echo "  ZEPHYR_BASE=${ZEPHYR_BASE}"
echo "  ZEPHYR_TOOLCHAIN_VARIANT=${ZEPHYR_TOOLCHAIN_VARIANT}"
if [ -n "${ZEPHYR_SDK_INSTALL_DIR}" ]; then
    echo "  ZEPHYR_SDK_INSTALL_DIR=${ZEPHYR_SDK_INSTALL_DIR}"
fi
echo ""
echo "Virtual environment activated at: ${VENV_DIR}"
echo "West dependencies fetched."
echo ""
echo "Next steps:"
echo "  1. Build your application:"
echo "     west build -p -b fk723m1_zgt6 app"
echo "  2. Flash to device:"
echo "     west flash"
echo ""
echo "Memory Report Tools:"
echo "  Generate RAM/ROM report:"
echo "     west build -d build -t ram_report"
echo "     west build -d build -t rom_report"
echo ""
echo "  Interactive memory analysis with Puncover:"
echo "     puncover --elf_file build/zephyr/zephyr.elf --gcc_tools_base \${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi/bin/arm-zephyr-eabi- --build_dir build"
echo "     Then open: http://localhost:5000"
echo ""

echo "To reactivate this environment later, run:"
echo "  source ${SCRIPT_DIR}/env.sh"
echo ""
