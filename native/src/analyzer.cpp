#include "analyzer.h"
#include "layout_calculator.h"
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/FixedCompilationDatabase.h>
#include <memory>

namespace structsight {

// AST Visitor to find and analyze record declarations
class StructVisitor : public clang::RecursiveASTVisitor<StructVisitor> {
public:
    StructVisitor(
        clang::ASTContext& ctx,
        const AnalysisRequest& req,
        std::vector<StructLayout>& results
    ) : context_(ctx), request_(req), results_(results) {}
    
    bool VisitRecordDecl(clang::RecordDecl* decl) {
        // Skip incomplete and implicit declarations
        if (!decl->isCompleteDefinition() || decl->isImplicit()) {
            return true;
        }
        
        // Check if we're looking for a specific struct
        if (!request_.struct_name.empty()) {
            std::string name = decl->getNameAsString();
            if (name != request_.struct_name) {
                return true;
            }
        }
        
        // Process this record
        try {
            Analyzer analyzer;
            StructLayout layout = analyzer.ProcessRecord(decl, context_, request_);
            results_.push_back(layout);
        } catch (const std::exception& e) {
            // Log error but continue visiting
        }
        
        return true;
    }
    
private:
    clang::ASTContext& context_;
    const AnalysisRequest& request_;
    std::vector<StructLayout>& results_;
};

// AST Consumer to traverse the AST
class StructConsumer : public clang::ASTConsumer {
public:
    StructConsumer(
        clang::ASTContext& ctx,
        const AnalysisRequest& req,
        std::vector<StructLayout>& results
    ) : visitor_(ctx, req, results) {}
    
    void HandleTranslationUnit(clang::ASTContext& context) override {
        visitor_.TraverseDecl(context.getTranslationUnitDecl());
    }
    
private:
    StructVisitor visitor_;
};

// Frontend Action to create the consumer
class StructAction : public clang::ASTFrontendAction {
public:
    StructAction(const AnalysisRequest& req, std::vector<StructLayout>& results)
        : request_(req), results_(results) {}
    
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& compiler,
        llvm::StringRef file
    ) override {
        return std::make_unique<StructConsumer>(
            compiler.getASTContext(),
            request_,
            results_
        );
    }
    
private:
    const AnalysisRequest& request_;
    std::vector<StructLayout>& results_;
};

// Analyzer implementation

Analyzer::Analyzer() = default;
Analyzer::~Analyzer() = default;

AnalysisResult Analyzer::Analyze(const AnalysisRequest& request) {
    AnalysisResult result;
    result.success = false;
    
    try {
        // Prepare compile flags
        std::vector<std::string> args = {"-std=c++17"};
        
        // Add architecture flag
        if (request.architecture == Architecture::X86) {
            args.push_back("-m32");
        } else {
            args.push_back("-m64");
        }
        
        // Add compiler-specific flags
        switch (request.compiler) {
            case Compiler::GCC:
                args.push_back("-fno-ms-compatibility");
                break;
            case Compiler::MSVC:
                args.push_back("-fms-compatibility");
                args.push_back("-fms-extensions");
                break;
            case Compiler::Clang:
            default:
                break;
        }
        
        // Add user-provided flags
        args.insert(args.end(), request.compile_flags.begin(), request.compile_flags.end());
        
        // Create compilation database
        auto comp_db = clang::tooling::FixedCompilationDatabase::loadFromCommandLine(
            args.size(),
            [&args]() {
                std::vector<const char*> ptrs;
                for (const auto& arg : args) {
                    ptrs.push_back(arg.c_str());
                }
                return ptrs;
            }().data()
        );
        
        // Run the tool
        std::vector<StructLayout> layouts;
        clang::tooling::ClangTool tool(
            *comp_db,
            {request.file_path.empty() ? "input.cpp" : request.file_path}
        );
        
        // Map the source code as a virtual file
        tool.mapVirtualFile(
            request.file_path.empty() ? "input.cpp" : request.file_path,
            request.source_code
        );
        
        // Run analysis
        int ret = tool.run(
            clang::tooling::newFrontendActionFactory<StructAction>(
                [&request, &layouts]() {
                    return std::make_unique<StructAction>(request, layouts);
                }
            ).get()
        );
        
        if (ret == 0) {
            result.success = true;
            result.layouts = std::move(layouts);
        } else {
            result.error_message = "Compilation failed";
        }
        
    } catch (const std::exception& e) {
        result.error_message = std::string("Analysis error: ") + e.what();
    }
    
    return result;
}

StructLayout Analyzer::ProcessRecord(
    const clang::RecordDecl* record,
    clang::ASTContext& context,
    const AnalysisRequest& request
) {
    StructLayout layout;
    
    // Basic information
    layout.name = record->getNameAsString();
    layout.qualified_name = record->getQualifiedNameAsString();
    
    // Get the layout from Clang's analysis
    const clang::ASTRecordLayout& ast_layout = context.getASTRecordLayout(record);
    
    layout.total_size = ast_layout.getSize().getQuantity();
    layout.alignment = ast_layout.getAlignment().getQuantity();
    
    // Check properties
    if (const auto* cxx_record = llvm::dyn_cast<clang::CXXRecordDecl>(record)) {
        layout.is_polymorphic = cxx_record->isPolymorphic();
        layout.is_standard_layout = cxx_record->isStandardLayout();
        
        // Process virtual functions if polymorphic
        if (layout.is_polymorphic) {
            layout.vtable.pointer_offset = 0; // Usually at offset 0
            layout.vtable.has_virtual_base = cxx_record->getNumVBases() > 0;
            
            for (auto method : cxx_record->methods()) {
                if (method->isVirtual()) {
                    layout.vtable.virtual_functions.push_back(
                        method->getNameAsString()
                    );
                }
            }
        }
    } else {
        layout.is_polymorphic = false;
        layout.is_standard_layout = true;
    }
    
    // Extract member information
    ExtractBasicLayout(layout, record, context);
    
    // Calculate padding using LayoutCalculator
    LayoutCalculator calculator(request.compiler, request.architecture);
    calculator.CalculatePadding(layout, context, record);
    calculator.GenerateOptimizations(layout);
    
    return layout;
}

void Analyzer::ExtractBasicLayout(
    StructLayout& layout,
    const clang::RecordDecl* record,
    const clang::ASTContext& context
) {
    const clang::ASTRecordLayout& ast_layout = context.getASTRecordLayout(record);
    
    uint32_t field_index = 0;
    for (const auto* field : record->fields()) {
        MemberInfo member;
        
        member.name = field->getNameAsString();
        member.type = field->getType().getAsString();
        
        // Get offset
        uint64_t offset_bits = ast_layout.getFieldOffset(field_index);
        member.offset = offset_bits / 8;
        
        // Get size and alignment
        clang::QualType type = field->getType();
        auto type_info = context.getTypeInfo(type);
        member.size = type_info.Width / 8;
        member.alignment = type_info.Align / 8;
        
        // Check for bitfield
        if (field->isBitField()) {
            member.is_bitfield = true;
            member.bitfield_width = field->getBitWidthValue(context);
            member.bitfield_offset = offset_bits % 8;
        } else {
            member.is_bitfield = false;
            member.bitfield_width = 0;
            member.bitfield_offset = 0;
        }
        
        layout.members.push_back(member);
        field_index++;
    }
    
    // Calculate useful size (without tail padding)
    if (!layout.members.empty()) {
        const auto& last_member = layout.members.back();
        layout.useful_size = last_member.offset + last_member.size;
    } else {
        layout.useful_size = 0;
    }
}

} // namespace structsight
