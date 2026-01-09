#ifndef STRUCTSIGHT_ANALYZER_H
#define STRUCTSIGHT_ANALYZER_H

#include "types.h"
#include <clang/Tooling/Tooling.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecordLayout.h>

namespace structsight {

class Analyzer {
public:
    Analyzer();
    ~Analyzer();
    
    // Main analysis entry point
    AnalysisResult Analyze(const AnalysisRequest& request);
    
private:
    // Helper to create compilation database
    std::unique_ptr<clang::tooling::CompilationDatabase> 
        CreateCompilationDatabase(const AnalysisRequest& request);
    
    // Process a single record (struct/class)
    StructLayout ProcessRecord(
        const clang::RecordDecl* record,
        clang::ASTContext& context,
        const AnalysisRequest& request
    );
    
    // Extract basic layout information
    void ExtractBasicLayout(
        StructLayout& layout,
        const clang::RecordDecl* record,
        const clang::ASTContext& context
    );
};

} // namespace structsight

#endif // STRUCTSIGHT_ANALYZER_H
