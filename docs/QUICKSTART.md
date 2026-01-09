# Quick Start Guide

## Installation

### Option 1: Install from Marketplace (Coming Soon)
Once published, you'll be able to install directly from the VS Code Marketplace.

### Option 2: Build from Source

#### Step 1: Install Prerequisites

**Windows:**
```powershell
# Install LLVM (choose pre-built binaries)
# Download from: https://releases.llvm.org/download.html
# Add LLVM bin directory to PATH

# Install Visual Studio Build Tools
# Download from: https://visualstudio.microsoft.com/downloads/
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install llvm-14-dev libclang-14-dev clang-14 cmake git
```

**macOS:**
```bash
brew install llvm cmake git
export PATH="/usr/local/opt/llvm/bin:$PATH"
```

#### Step 2: Build the Extension

```bash
# Clone repository
git clone https://github.com/ieee-cs-bmsit/structsight.git
cd structsight

# Install dependencies
npm run install:all

# Build native module
cd native
npm run build

# Build extension
cd ../extension
npm run compile
```

#### Step 3: Install in VS Code

```bash
# From extension directory
vsce package
code --install-extension structsight-0.1.0.vsix
```

## Basic Usage

### 1. Hover Analysis

Simply hover over a `struct` or `class` keyword:

```cpp
struct MyStruct {  // <- Hover here
    int a;
    char b;
    double c;
};
```

You'll see a popup with:
- Total size
- Alignment
- Padding percentage
- Quick optimization hints

### 2. Detailed Visualization

Click "Show Detailed Layout" in the hover tooltip, or:
1. Place cursor on struct name
2. Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (macOS)
3. Type "StructSight: Show Memory Layout"
4. Press Enter

This opens an interactive webview showing:
- Complete memory map with all members
- Padding regions highlighted in red
- Cache line boundaries marked in blue
- Optimization suggestions
- VTable information (for polymorphic classes)

### 3. Apply Optimizations

When StructSight detects inefficient layouts:

**Method 1: Quick Fix**
1. Place cursor on struct name
2. Press `Ctrl+.` (Windows/Linux) or `Cmd+.` (macOS)
3. Select "Reorder members to save X bytes"
4. Confirm the refactoring

**Method 2: From Webview**
1. Open detailed visualization
2. Scroll to "Optimization Suggestions"
3. Click "Apply Reordering" button
4. VS Code will show a diff preview
5. Confirm or reject changes

## Configuration

Access settings: `File > Preferences > Settings > StructSight`

### Key Settings:

**Architecture** (`structsight.architecture`)
- `x64` (default): 64-bit analysis
- `x86`: 32-bit analysis

**Compiler** (`structsight.compiler`)
- `clang` (default)
- `gcc`
- `msvc`

**Cache Line Size** (`structsight.cacheLineSize`)
- Default: `64` bytes
- Modify for specific CPU architectures

**Show Padding** (`structsight.showPaddingBytes`)
- `true` (default): Highlight padding in red
- `false`: Hide padding visualization

## Examples

### Example 1: Basic Struct Optimization

**Before:**
```cpp
struct Person {
    char initial;     // 1 byte
    int age;          // 4 bytes
    char gender;      // 1 byte
    double salary;    // 8 bytes
};
// Size: 24 bytes
```

**StructSight Analysis:**
- 10 bytes of padding detected
- Suggested reordering can save 8 bytes

**After (optimized):**
```cpp
struct Person {
    double salary;    // 8 bytes
    int age;          // 4 bytes
    char initial;     // 1 byte
    char gender;      // 1 byte
    // 2 bytes tail padding
};
// Size: 16 bytes (33% smaller!)
```

### Example 2: Cache Line Awareness

```cpp
struct HotData {
    long long values[8];  // 64 bytes (exactly 1 cache line)
};

struct ColdData {
    long long values[9];  // 72 bytes (spans 2 cache lines!)
    // StructSight will warn about cache line splitting
};
```

## Troubleshooting

### Native Module Not Loading

**Error:** "Failed to load native module"

**Solution:**
1. Ensure LLVM/Clang is installed
2. Check that LLVM is in your PATH
3. Try rebuilding: `cd native && npm rebuild`

### Analysis Fails

**Error:** "Compilation failed"

**Possible causes:**
- Invalid C++ syntax
- Missing headers
- Unsupported language features

**Solution:**
- Check C++ code compiles with your compiler
-Add necessary include paths in compile flags

### No Hover Information

**Check:**
1. Is `structsight.enableHoverInfo` set to `true`?
2. Are you hovering over `struct` or `class` keyword?
3. Is the definition complete (not forward declaration)?

## Tips & Tricks

1. **Use with Multiple Architectures**: Switch between x86 and x64 to see how your structs behave on different platforms

2. **Compiler Comparison**: Try different compiler settings to see layout differences

3. **Cache-Friendly Design**: Keep frequently accessed members within first 64 bytes

4. **Alignment Control**: Use `alignas()` keyword for custom alignment requirements

5. **Padding Reduction**: Place larger members first, smaller members last

## Next Steps

- Read the [Memory Layout Guide](../docs/memory-layout-guide.md)
- Check out more [examples](../test/fixtures/)
- Learn about [optimization best practices](../docs/optimization-guide.md)

## Getting Help

- **Issues**: https://github.com/yourusername/structsight/issues
- **Discussions**: https://github.com/yourusername/structsight/discussions
- **Documentation**: https://github.com/yourusername/structsight/wiki
