# StructSight - Changelog

All notable changes to the StructSight extension will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial release of StructSight
- Interactive memory layout visualization for C++ structs and classes
- Hover provider showing quick memory layout summaries
- Detailed webview with complete memory map
- Padding detection and highlighting
- Cache line boundary visualization (64-byte cache lines)
- Virtual table (vtable) analysis for polymorphic classes
- Optimization suggestions with member reordering
- One-click refactoring to apply suggested optimizations
- Support for x86 (32-bit) and x64 (64-bit) architectures
- Compiler-specific layout rules (GCC, Clang, MSVC)
- Native C++ analysis engine using Clang LibTooling
- Comprehensive configuration options
- Code actions for automatic refactoring

### Known Issues
- Template specializations may not be fully analyzed in some cases
- Packed structures (#pragma pack) support is limited
- Some edge cases with bit fields may not be visualized correctly

## [0.1.0] - 2026-01-09

### Added
- Initial development release
- Core functionality for struct/class analysis
- Basic visualization and optimization suggestions

[Unreleased]: https://github.com/yourusername/structsight/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/yourusername/structsight/releases/tag/v0.1.0
