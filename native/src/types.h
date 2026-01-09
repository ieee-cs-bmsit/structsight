#ifndef STRUCTSIGHT_TYPES_H
#define STRUCTSIGHT_TYPES_H

#include <string>
#include <vector>
#include <cstdint>

namespace structsight {

// Architecture type
enum class Architecture {
    X86,    // 32-bit
    X64     // 64-bit
};

// Compiler type (affects layout rules)
enum class Compiler {
    GCC,
    Clang,
    MSVC
};

// Member information
struct MemberInfo {
    std::string name;
    std::string type;
    uint64_t offset;        // Byte offset from start of struct
    uint64_t size;          // Size in bytes
    uint64_t alignment;     // Alignment requirement
    bool is_bitfield;       // Is this a bit field?
    uint32_t bitfield_width; // Width in bits (if bitfield)
    uint32_t bitfield_offset; // Bit offset within byte
};

// Padding region
struct PaddingInfo {
    uint64_t offset;        // Where padding starts
    uint64_t size;          // How many bytes of padding
    std::string reason;     // Why padding exists (alignment, end-padding, etc.)
};

// Virtual table information
struct VTableInfo {
    uint64_t pointer_offset; // Offset of vptr in object
    std::vector<std::string> virtual_functions; // List of virtual functions
    bool has_virtual_base;   // Has virtual base classes
};

// Complete struct layout analysis
struct StructLayout {
    std::string name;
    std::string qualified_name;      // Fully qualified name
    uint64_t total_size;              // Total size in bytes
    uint64_t alignment;               // Alignment requirement
    std::vector<MemberInfo> members;  // All members in order
    std::vector<PaddingInfo> padding; // All padding regions
    VTableInfo vtable;                // Virtual table info (if polymorphic)
    bool is_polymorphic;              // Has virtual functions
    bool is_standard_layout;          // Is standard layout type
    uint64_t useful_size;             // Size without tail padding
    
    // Optimization suggestions
    struct Optimization {
        std::string description;
        uint64_t bytes_saved;
        std::vector<std::string> suggested_order; // Suggested member order
        double confidence;            // 0.0 to 1.0
    };
    std::vector<Optimization> optimizations;
};

// Analysis request
struct AnalysisRequest {
    std::string source_code;
    std::string file_path;
    std::string struct_name;         // Which struct to analyze (empty = all)
    Architecture architecture;
    Compiler compiler;
    std::vector<std::string> compile_flags; // Additional compiler flags
};

// Analysis result
struct AnalysisResult {
    bool success;
    std::string error_message;
    std::vector<StructLayout> layouts; // All analyzed structs
};

} // namespace structsight

#endif // STRUCTSIGHT_TYPES_H
