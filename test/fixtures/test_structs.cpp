// Simple test struct with padding
struct TestStruct {
    char a;      // 1 byte
    int b;       // 4 bytes (3 bytes padding after 'a')
    char c;      // 1 byte
    double d;    // 8 bytes (7 bytes padding after 'c')
};

// Optimized version (what StructSight would suggest)
struct OptimizedStruct {
    double d;    // 8 bytes
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes tail padding for alignment
};

// Polymorphic class with vtable
class BaseClass {
public:
    virtual void foo() {}
    virtual void bar() {}
    
    int x;
    char y;
};

class DerivedClass : public BaseClass {
public:
    void foo() override {}
    
    double z;
};

// Bitfield example
struct BitfieldStruct {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int value : 6;
    int normal_field;
};

// Nested struct
struct OuterStruct {
    int a;
    
    struct InnerStruct {
        char x;
        short y;
    } inner;
    
    double b;
};

// Packed struct (compiler specific)
#pragma pack(push, 1)
struct PackedStruct {
    char a;
    int b;
    char c;
}; // No padding
#pragma pack(pop)

// Empty struct (edge case)
struct EmptyStruct {};

// Large struct for cache line analysis
struct CacheLineTest {
    long long a;  // 8 bytes
    long long b;  // 8 bytes
    long long c;  // 8 bytes
    long long d;  // 8 bytes  
    long long e;  // 8 bytes
    // ... continues up to 64 bytes
    long long f;  // 8 bytes
    long long g;  // 8 bytes
    long long h;  // 8 bytes
    // Total: 64 bytes (exactly 1 cache line)
    char extra;   // This will cross into next cache line
};
