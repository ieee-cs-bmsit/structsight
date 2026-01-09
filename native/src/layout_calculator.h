#ifndef STRUCTSIGHT_LAYOUT_CALCULATOR_H
#define STRUCTSIGHT_LAYOUT_CALCULATOR_H

#include "types.h"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>

namespace structsight {

class LayoutCalculator {
public:
    LayoutCalculator(Compiler compiler, Architecture arch);
    
    // Calculate padding regions
    void CalculatePadding(
        StructLayout& layout,
        const clang::ASTContext& context,
        const clang::RecordDecl* record
    );
    
    // Generate optimization suggestions
    void GenerateOptimizations(StructLayout& layout);
    
private:
    Compiler compiler_;
    Architecture arch_;
    
    // Calculate optimal member ordering
    std::vector<std::string> CalculateOptimalOrder(const StructLayout& layout);
    
    // Calculate size with given member order
    uint64_t CalculateSizeWithOrder(
        const StructLayout& layout,
        const std::vector<std::string>& order
    );
    
    // Get pointer size for architecture
    uint64_t GetPointerSize() const;
};

} // namespace structsight

#endif // STRUCTSIGHT_LAYOUT_CALCULATOR_H
