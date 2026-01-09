#include "layout_calculator.h"
#include <algorithm>
#include <map>

namespace structsight {

LayoutCalculator::LayoutCalculator(Compiler compiler, Architecture arch)
    : compiler_(compiler), arch_(arch) {}

uint64_t LayoutCalculator::GetPointerSize() const {
    return (arch_ == Architecture::X86) ? 4 : 8;
}

void LayoutCalculator::CalculatePadding(
    StructLayout& layout,
    const clang::ASTContext& context,
    const clang::RecordDecl* record
) {
    layout.padding.clear();
    
    if (layout.members.empty()) {
        return;
    }
    
    // Find padding between members
    for (size_t i = 0; i < layout.members.size() - 1; i++) {
        const auto& current = layout.members[i];
        const auto& next = layout.members[i + 1];
        
        uint64_t current_end = current.offset + current.size;
        if (next.offset > current_end) {
            PaddingInfo padding;
            padding.offset = current_end;
            padding.size = next.offset - current_end;
            padding.reason = "Alignment of next member (" + next.name + ")";
            layout.padding.push_back(padding);
        }
    }
    
    // Check for tail padding
    if (!layout.members.empty()) {
        const auto& last = layout.members.back();
        uint64_t last_end = last.offset + last.size;
        
        if (layout.total_size > last_end) {
            PaddingInfo padding;
            padding.offset = last_end;
            padding.size = layout.total_size - last_end;
            padding.reason = "Tail padding for struct alignment";
            layout.padding.push_back(padding);
        }
    }
}

std::vector<std::string> LayoutCalculator::CalculateOptimalOrder(const StructLayout& layout) {
    // Create a copy of members with their info
    std::vector<std::pair<std::string, const MemberInfo*>> members;
    for (const auto& member : layout.members) {
        members.push_back({member.name, &member});
    }
    
    // Sort by alignment (descending), then by size (descending)
    // This typically minimizes padding
    std::sort(members.begin(), members.end(),
        [](const auto& a, const auto& b) {
            if (a.second->alignment != b.second->alignment) {
                return a.second->alignment > b.second->alignment;
            }
            return a.second->size > b.second->size;
        }
    );
    
    std::vector<std::string> order;
    for (const auto& m : members) {
        order.push_back(m.first);
    }
    
    return order;
}

uint64_t LayoutCalculator::CalculateSizeWithOrder(
    const StructLayout& layout,
    const std::vector<std::string>& order
) {
    // Create a map for quick lookup
    std::map<std::string, const MemberInfo*> member_map;
    for (const auto& member : layout.members) {
        member_map[member.name] = &member;
    }
    
    uint64_t current_offset = 0;
    uint64_t max_alignment = 1;
    
    // Account for vtable pointer if polymorphic
    if (layout.is_polymorphic) {
        uint64_t ptr_size = GetPointerSize();
        current_offset = ptr_size;
        max_alignment = ptr_size;
    }
    
    // Calculate new layout
    for (const auto& name : order) {
        auto it = member_map.find(name);
        if (it == member_map.end()) {
            continue;
        }
        
        const MemberInfo* member = it->second;
        
        // Align current offset to member's alignment
        uint64_t alignment = member->alignment;
        if (alignment > 0) {
            current_offset = (current_offset + alignment - 1) / alignment * alignment;
        }
        
        // Place member
        current_offset += member->size;
        max_alignment = std::max(max_alignment, alignment);
    }
    
    // Add tail padding to align to struct alignment
    if (max_alignment > 0) {
        current_offset = (current_offset + max_alignment - 1) / max_alignment * max_alignment;
    }
    
    return current_offset;
}

void LayoutCalculator::GenerateOptimizations(StructLayout& layout) {
    layout.optimizations.clear();
    
    // Don't optimize empty structs or single-member structs
    if (layout.members.size() < 2) {
        return;
    }
    
    // Calculate optimal ordering
    std::vector<std::string> optimal_order = CalculateOptimalOrder(layout);
    
    // Check if current order matches optimal
    bool is_optimal = true;
    for (size_t i = 0; i < layout.members.size(); i++) {
        if (layout.members[i].name != optimal_order[i]) {
            is_optimal = false;
            break;
        }
    }
    
    if (!is_optimal) {
        uint64_t optimized_size = CalculateSizeWithOrder(layout, optimal_order);
        
        if (optimized_size < layout.total_size) {
            StructLayout::Optimization opt;
            opt.description = "Reorder members by alignment to reduce padding";
            opt.bytes_saved = layout.total_size - optimized_size;
            opt.suggested_order = optimal_order;
            opt.confidence = 0.95; // High confidence for standard struct optimization
            
            layout.optimizations.push_back(opt);
        }
    }
    
    // Check for cache line splitting (if members are large)
    const uint64_t cache_line_size = 64;
    for (const auto& member : layout.members) {
        uint64_t member_end = member.offset + member.size;
        uint64_t start_line = member.offset / cache_line_size;
        uint64_t end_line = (member_end - 1) / cache_line_size;
        
        if (start_line != end_line && member.size < cache_line_size) {
            StructLayout::Optimization opt;
            opt.description = "Member '" + member.name + "' spans multiple cache lines";
            opt.bytes_saved = 0; // Informational
            opt.confidence = 0.8;
            opt.suggested_order = {}; // No specific reordering suggested
            
            layout.optimizations.push_back(opt);
        }
    }
}

} // namespace structsight
