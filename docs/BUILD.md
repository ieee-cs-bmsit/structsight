# StructSight - Build Instructions

## Prerequisites

Before building StructSight, ensure you have the following installed:

### All Platforms
- **Node.js** 18.x or later
- **npm** 9.x or later
- **CMake** 3.15 or later
- **Git**

### Windows
- **Visual Studio 2019 or later** (with C++ development tools)
- **LLVM/Clang 14.x or later**
  - Download from: https://releases.llvm.org/download.html
  - Install to default location (e.g., `C:\Program Files\LLVM`)
  - Add `LLVM\bin` to your PATH

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    llvm-14-dev \
    libclang-14-dev \
    clang-14 \
    nodejs \
    npm
```

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install llvm cmake node git

# Add LLVM to PATH
echo 'export PATH="/usr/local/opt/llvm/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

---

## Building the Project

### Step 1: Clone the Repository

```bash
git clone https://github.com/ieee-cs-bmsit/structsight.git
cd structsight
```

### Step 2: Install Dependencies

```bash
# Install dependencies for all packages
npm run install:all
```

This will:
- Install root package dependencies
- Install extension dependencies in `extension/`
- Install native module dependencies in `native/`

### Step 3: Build the Native Module

```bash
# Build the C++ native addon
cd native
npm run build
```

**Expected output:**
```
> cmake-js compile

Building native module...
✓ Clang LibTooling found
✓ Compiling addon.cpp
✓ Compiling analyzer.cpp
✓ Compiling layout_calculator.cpp
✓ Linking native module
✓ Build complete: build/Release/structsight_native.node
```

**Troubleshooting:**
- If CMake can't find LLVM, set `LLVM_DIR` environment variable:
  ```bash
  # Windows
  set LLVM_DIR=C:\Program Files\LLVM\lib\cmake\llvm
  
  # Linux/macOS
  export LLVM_DIR=/usr/lib/llvm-14/lib/cmake/llvm
  ```

### Step 4: Test the Native Module

```bash
# Still in native/ directory
npm test
```

**Expected output:**
```
Testing StructSight Native Module...

Analyzing TestStruct...
✓ Analysis successful!

Struct: TestStruct
Size: 24 bytes
Alignment: 8 bytes

Members:
  + 0 | 1B | a (char)
  + 4 | 4B | b (int)
  + 8 | 1B | c (char)
  +16 | 8B | d (double)

Padding:
  + 1 | 3B | Alignment of next member (b)
  + 9 | 7B | Alignment of next member (d)

Optimizations:
  💡 Reorder members by alignment to reduce padding
     Can save 8 bytes
     Suggested order: d, b, a, c

✓ All tests passed!
```

### Step 5: Build the Extension

```bash
cd ../extension
npm run compile
```

**Expected output:**
```
> tsc -p ./

✓ TypeScript compilation complete
✓ Output: extension/out/
```

### Step 6: Run in Development Mode

```bash
# From project root or extension directory
code .
```

Then in VS Code:
1. Press `F5` to launch Extension Development Host
2. Open a C++ file in the new window
3. Test the extension features

---

## Building for Distribution

### Create VSIX Package

```bash
cd extension

# Install vsce if not already installed
npm install -g @vscode/vsce

# Package the extension
npm run package
```

**Output:** `structsight-0.1.0.vsix`

### Install VSIX Locally

```bash
code --install-extension structsight-0.1.0.vsix
```

---

## Build Configuration

### CMake Configuration

The native module build is configured via [CMakeLists.txt](../native/CMakeLists.txt):

- **C++ Standard**: C++17
- **LLVM Version**: 14.x or compatible
- **Required Clang Libraries**:
  - clangTooling
  - clangFrontend
  - clangDriver
  - clangSerialization
  - clangParse
  - clangSema
  - clangAST
  - clangBasic

### TypeScript Configuration

Extension TypeScript compilation via [tsconfig.json](../extension/tsconfig.json):

- **Target**: ES2022
- **Module**: CommonJS
- **Strict Mode**: Enabled
- **Output**: `extension/out/`

---

## Platform-Specific Notes

### Windows

**Visual Studio Version:**
- Ensure you have C++ build tools installed via Visual Studio Installer
- Required components:
  - MSVC v142 or later
  - Windows 10 SDK
  - C++ CMake tools

**Path Configuration:**
Make sure LLVM is in your PATH:
```powershell
$env:Path += ";C:\Program Files\LLVM\bin"
```

### Linux

**LLVM Version:**
Ubuntu 20.04+: Use LLVM 14
```bash
# If llvm-14 is not default, set alternatives
sudo update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-14 100
```

**Permissions:**
If you encounter permission errors during build:
```bash
sudo chown -R $USER:$USER node_modules
```

### macOS

**Apple Silicon (M1/M2):**
LLVM installed via Homebrew works natively on ARM64.

**Intel Macs:**
Standard x86_64 build process works as documented.

**Xcode Command Line Tools:**
```bash
xcode-select --install
```

---

## Verification

### Verify Native Module

```bash
cd native
node -e "console.log(require('./build/Release/structsight_native.node'))"
```

**Expected:** Should print object with `analyze` function.

### Verify Extension

After installing VSIX:
1. Open VS Code
2. Create a simple C++ file:
   ```cpp
   struct Test {
       char a;
       int b;
   };
   ```
3. Hover over `struct` - you should see layout info
4. Run command "StructSight: Show Memory Layout"

---

## CI/CD Setup (Future)

### GitHub Actions Workflow

```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    
    runs-on: ${{ matrix.os }}
    
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '18'
      
      - name: Install LLVM (Ubuntu)
        if: matrix.os == 'ubuntu-latest'
        run: |
          sudo apt-get install llvm-14-dev libclang-14-dev
      
      - name: Install LLVM (macOS)
        if: matrix.os == 'macos-latest'
        run: brew install llvm
      
      - name: Install LLVM (Windows)
        if: matrix.os == 'windows-latest'
        run: choco install llvm
      
      - name: Build
        run: |
          npm run install:all
          npm run build
      
      - name: Test
        run: cd native && npm test
```

---

## Troubleshooting

### Common Issues

**Issue:** `CMake Error: Could not find LLVM`

**Solution:**
- Ensure LLVM is installed
- Set `LLVM_DIR` environment variable
- Use `llvm-config` to find path: `llvm-config --cmakedir`

---

**Issue:** `Error: Cannot find module 'structsight-native'`

**Solution:**
- Rebuild native module: `cd native && npm rebuild`
- Check that `.node` file exists in `native/build/Release/`

---

**Issue:** Extension not loading in VS Code

**Solution:**
- Check extension output: View → Output → StructSight
- Ensure all dependencies installed: `cd extension && npm install`
- Rebuild extension: `npm run compile`

---

**Issue:** Analysis fails with "Compilation failed"

**Solution:**
- Check C++ syntax is valid
- Ensure file has proper C++ extensions (.cpp, .h)
- Check for required `#include` directives

---

## Development Tips

### Watch Mode

For active development, run TypeScript in watch mode:
```bash
cd extension
npm run watch
```

### Debugging Native Code

1. Build with debug symbols:
   ```bash
   cd native
   cmake-js configure --debug
   cmake-js build
   ```

2. Attach debugger (GDB/LLDB/Visual Studio)

### Clean Build

```bash
# Clean native module
cd native
npm run clean
npm run build

# Clean extension
cd ../extension
rm -rf out
npm run compile
```

---

For more help, see:
- [Contributing Guide](../CONTRIBUTING.md)
- [Quick Start Guide](./QUICKSTART.md)
- [Project Walkthrough](../../../.gemini/antigravity/brain/.../walkthrough.md)
