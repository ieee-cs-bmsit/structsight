#include <napi.h>
#include "types.h"
#include "analyzer.h"
#include <memory>

namespace structsight {

// Convert JS object to AnalysisRequest
AnalysisRequest ParseRequest(const Napi::Object& obj) {
    AnalysisRequest req;
    
    req.source_code = obj.Get("sourceCode").As<Napi::String>().Utf8Value();
    req.file_path = obj.Get("filePath").As<Napi::String>().Utf8Value();
    
    if (obj.Has("structName")) {
        req.struct_name = obj.Get("structName").As<Napi::String>().Utf8Value();
    }
    
    // Parse architecture
    std::string arch = obj.Get("architecture").As<Napi::String>().Utf8Value();
    req.architecture = (arch == "x86") ? Architecture::X86 : Architecture::X64;
    
    // Parse compiler
    std::string compiler = obj.Get("compiler").As<Napi::String>().Utf8Value();
    if (compiler == "gcc") {
        req.compiler = Compiler::GCC;
    } else if (compiler == "msvc") {
        req.compiler = Compiler::MSVC;
    } else {
        req.compiler = Compiler::Clang;
    }
    
    // Parse compile flags
    if (obj.Has("compileFlags")) {
        Napi::Array flags_array = obj.Get("compileFlags").As<Napi::Array>();
        for (uint32_t i = 0; i < flags_array.Length(); i++) {
            req.compile_flags.push_back(
                flags_array.Get(i).As<Napi::String>().Utf8Value()
            );
        }
    }
    
    return req;
}

// Convert MemberInfo to JS object
Napi::Object MemberToJS(const Napi::Env& env, const MemberInfo& member) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("name", member.name);
    obj.Set("type", member.type);
    obj.Set("offset", Napi::Number::New(env, member.offset));
    obj.Set("size", Napi::Number::New(env, member.size));
    obj.Set("alignment", Napi::Number::New(env, member.alignment));
    obj.Set("isBitfield", Napi::Boolean::New(env, member.is_bitfield));
    obj.Set("bitfieldWidth", Napi::Number::New(env, member.bitfield_width));
    obj.Set("bitfieldOffset", Napi::Number::New(env, member.bitfield_offset));
    return obj;
}

// Convert PaddingInfo to JS object
Napi::Object PaddingToJS(const Napi::Env& env, const PaddingInfo& padding) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("offset", Napi::Number::New(env, padding.offset));
    obj.Set("size", Napi::Number::New(env, padding.size));
    obj.Set("reason", padding.reason);
    return obj;
}

// Convert VTableInfo to JS object
Napi::Object VTableToJS(const Napi::Env& env, const VTableInfo& vtable) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("pointerOffset", Napi::Number::New(env, vtable.pointer_offset));
    obj.Set("hasVirtualBase", Napi::Boolean::New(env, vtable.has_virtual_base));
    
    Napi::Array funcs = Napi::Array::New(env, vtable.virtual_functions.size());
    for (size_t i = 0; i < vtable.virtual_functions.size(); i++) {
        funcs.Set(i, vtable.virtual_functions[i]);
    }
    obj.Set("virtualFunctions", funcs);
    
    return obj;
}

// Convert Optimization to JS object
Napi::Object OptimizationToJS(const Napi::Env& env, const StructLayout::Optimization& opt) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("description", opt.description);
    obj.Set("bytesSaved", Napi::Number::New(env, opt.bytes_saved));
    obj.Set("confidence", Napi::Number::New(env, opt.confidence));
    
    Napi::Array order = Napi::Array::New(env, opt.suggested_order.size());
    for (size_t i = 0; i < opt.suggested_order.size(); i++) {
        order.Set(i, opt.suggested_order[i]);
    }
    obj.Set("suggestedOrder", order);
    
    return obj;
}

// Convert StructLayout to JS object
Napi::Object LayoutToJS(const Napi::Env& env, const StructLayout& layout) {
    Napi::Object obj = Napi::Object::New(env);
    
    obj.Set("name", layout.name);
    obj.Set("qualifiedName", layout.qualified_name);
    obj.Set("totalSize", Napi::Number::New(env, layout.total_size));
    obj.Set("alignment", Napi::Number::New(env, layout.alignment));
    obj.Set("usefulSize", Napi::Number::New(env, layout.useful_size));
    obj.Set("isPolymorphic", Napi::Boolean::New(env, layout.is_polymorphic));
    obj.Set("isStandardLayout", Napi::Boolean::New(env, layout.is_standard_layout));
    
    // Members
    Napi::Array members = Napi::Array::New(env, layout.members.size());
    for (size_t i = 0; i < layout.members.size(); i++) {
        members.Set(i, MemberToJS(env, layout.members[i]));
    }
    obj.Set("members", members);
    
    // Padding
    Napi::Array padding = Napi::Array::New(env, layout.padding.size());
    for (size_t i = 0; i < layout.padding.size(); i++) {
        padding.Set(i, PaddingToJS(env, layout.padding[i]));
    }
    obj.Set("padding", padding);
    
    // VTable
    obj.Set("vtable", VTableToJS(env, layout.vtable));
    
    // Optimizations
    Napi::Array opts = Napi::Array::New(env, layout.optimizations.size());
    for (size_t i = 0; i < layout.optimizations.size(); i++) {
        opts.Set(i, OptimizationToJS(env, layout.optimizations[i]));
    }
    obj.Set("optimizations", opts);
    
    return obj;
}

// Main analysis function exposed to JavaScript
Napi::Value Analyze(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object argument")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    
    try {
        // Parse request
        AnalysisRequest request = ParseRequest(info[0].As<Napi::Object>());
        
        // Create analyzer and perform analysis
        Analyzer analyzer;
        AnalysisResult result = analyzer.Analyze(request);
        
        // Convert result to JS
        Napi::Object js_result = Napi::Object::New(env);
        js_result.Set("success", Napi::Boolean::New(env, result.success));
        js_result.Set("errorMessage", result.error_message);
        
        Napi::Array layouts = Napi::Array::New(env, result.layouts.size());
        for (size_t i = 0; i < result.layouts.size(); i++) {
            layouts.Set(i, LayoutToJS(env, result.layouts[i]));
        }
        js_result.Set("layouts", layouts);
        
        return js_result;
        
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Null();
    }
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("analyze", Napi::Function::New(env, Analyze));
    return exports;
}

} // namespace structsight

NODE_API_MODULE(structsight_native, structsight::Init)
