# Contributing to StructSight

Thank you for your interest in contributing to StructSight! This document provides guidelines and instructions for contributing.

## Development Setup

### Prerequisites

1. **Node.js** 18.x or later
2. **LLVM/Clang** development libraries (14.x or later)
   - Windows: Download from llvm.org
   - Linux: `sudo apt-get install llvm-14-dev libclang-14-dev`
   - macOS: `brew install llvm`
3. **CMake** 3.15 or later
4. **C++ Compiler** with C++17 support
5. **VS Code** 1.85.0 or later

### Building from Source

```bash
# Clone the repository
git clone https://github.com/ieee-cs-bmsit/structsight.git
cd structsight

# Install all dependencies
npm run install:all

# Build the native addon
npm run build:native

# Build the extension
npm run build:extension
```

### Running Tests

```bash
# Test native module
cd native
npm test

# Test extension
cd extension
npm test
```

### Development Workflow

1. Open the project in VS Code
2. Press F5 to launch Extension Development Host
3. Make changes to TypeScript or C++ code
4. For TypeScript: Restart debug session to see changes
5. For C++: Rebuild native module (`npm run build:native`) and restart

## Code Style

### TypeScript
- Use ESLint configuration provided
- 4 spaces for indentation
- Prefer `const` over `let`
- Use meaningful variable names

### C++
- Follow Clang format style
- Use descriptive names
- Comment complex algorithms
- Prefer RAII and smart pointers

## Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for your changes
5. Ensure all tests pass
6. Commit with descriptive messages
7. Push to your fork
8. Open a Pull Request

### PR Guidelines

- Provide clear description of changes
- Reference any related issues
- Include screenshots for UI changes
- Ensure CI passes
- Get at least one review approval

## Reporting Bugs

Use GitHub Issues with the following information:

- **Title**: Clear, concise description
- **Environment**: OS, VS Code version, extension version
- **Steps to reproduce**: Detailed steps
- **Expected behavior**: What should happen
- **Actual behavior**: What actually happens
- **Code sample**: Minimal C++ code that triggers the issue

## Feature Requests

We welcome feature requests! Please:

- Check if the feature already exists
- Provide use case and rationale
- Describe expected behavior
- Consider implementation complexity

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

Thank you for making StructSight better! 🎉
