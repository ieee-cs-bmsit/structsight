# StructSight 🔍

**C++ Memory Layout Visualizer for VS Code**

StructSight is a powerful VS Code extension that provides real-time visualization of C++ struct and class memory layouts. Get instant insights into memory padding, alignment, vtable layouts, and optimization opportunities - all while you code.

## ✨ Features

### 📊 **Interactive Memory Visualization**
- Hover over struct/class definitions to see memory layout summaries
- Open detailed webview with complete memory map
- Visual representation of members, padding, and alignment
- Color-coded padding regions with explanations

### 🎯 **Cache Line Analysis**
- Visualize 64-byte cache line boundaries
- Detect members spanning multiple cache lines
- Optimize for cache-friendly data structures

### 🚀 **Optimization Suggestions**
- Automatic detection of excessive padding
- Suggested member reordering for size reduction
- "Can save X bytes" insights with confidence scores
- One-click refactoring to apply optimizations

### 🔧 **Multi-Architecture Support**
- Switch between 32-bit (x86) and 64-bit (x64) architectures
- Compiler-specific layout rules (GCC, Clang, MSVC)
- See how your structs behave across different platforms

### 🎨 **Virtual Table Visualization**
- Detect polymorphic classes automatically
- Visualize vtable pointer placement
- List all virtual functions
- Handle multiple inheritance scenarios

## 📸 Screenshots

### Hover Information
![Hover showing struct summary](resources/hover-demo.png)

### Detailed Memory Layout
![Interactive webview with memory visualization](resources/webview-demo.png)

### Optimization Suggestions
![One-click refactoring for better layouts](resources/optimization-demo.png)

## 🚀 Getting Started

### Installation

Install from the VS Code Marketplace:
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "StructSight"
4. Click Install

### Prerequisites

**Windows:**
- LLVM/Clang development libraries (pre-built binaries included)
- Visual Studio 2019 or later (for building from source)

**Linux:**
```bash
sudo apt-get install llvm-14-dev libclang-14-dev clang-14
```

**macOS:**
```bash
brew install llvm
```

### Usage

1. **Hover Analysis**: Hover over any `struct` or `class` keyword to see a quick summary
2. **Detailed View**: Click "Show Detailed Layout" or use command `StructSight: Show Memory Layout`
3. **Apply Optimizations**: Click the "Apply Reordering" button in the webview or use VS Code's Quick Fix (Ctrl+.)

## ⚙️ Configuration

Configure StructSight through VS Code settings:

```json
{
  "structsight.architecture": "x64",          // "x86" or "x64"
  "structsight.compiler": "clang",            // "gcc", "clang", or "msvc"
  "structsight.cacheLineSize": 64,            // Cache line size in bytes
  "structsight.showPaddingBytes": true,       // Highlight padding
  "structsight.showOptimizationHints": true,  // Show optimization suggestions
  "structsight.enableHoverInfo": true         // Enable hover provider
}
```

## 🎯 Examples

### Before Optimization
```cpp
struct BadLayout {
    char a;      // 1 byte
    int b;       // 4 bytes (3 bytes padding before)
    char c;      // 1 byte
    double d;    // 8 bytes (7 bytes padding before)
};
// Total: 24 bytes (10 bytes padding!)
```

### After Optimization (suggested by StructSight)
```cpp
struct GoodLayout {
    double d;    // 8 bytes
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes tail padding
};
// Total: 16 bytes (only 2 bytes padding) - 33% size reduction!
```

## 🏗️ Architecture

StructSight combines the power of Clang's LibTooling with VS Code's extension API:

- **C++ Analysis Engine**: Uses Clang AST to parse C++ code with compiler accuracy
- **Native Node.js Addon**: Exposes C++ analysis to TypeScript via N-API
- **TypeScript Extension**: Provides VS Code integration and UI
- **React Webview**: Interactive visualization of memory layouts

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup

```bash
# Clone the repository
git clone https://github.com/yourusername/structsight.git
cd structsight

# Install dependencies
npm run install:all

# Build native addon
npm run build:native

# Build extension
npm run build:extension

# Run in development mode
code .
# Press F5 to launch extension development host
```

## 📝 License

MIT License - see [LICENSE](LICENSE) for details

## 🙏 Acknowledgments

- Built with [Clang's LibTooling](https://clang.llvm.org/docs/LibTooling.html)
- Inspired by the need for better C++ memory layout understanding
- Thanks to the VS Code extension API team

## 🐛 Issues & Feature Requests

Found a bug or have a feature request? Please [open an issue](https://github.com/yourusername/structsight/issues)!

## 📚 Resources

- [C++ Memory Layout Guide](docs/memory-layout-guide.md)
- [Optimization Best Practices](docs/optimization-guide.md)
- [API Documentation](docs/api.md)

---

**Made with ❤️ for C++ developers who care about performance**
