const native = require('../../native/build/Release/structsight_native.node');

console.log('Testing StructSight Native Module...\n');

const testCode = `
struct TestStruct {
    char a;
    int b;
    char c;
    double d;
};
`;

const request = {
    sourceCode: testCode,
    filePath: 'test.cpp',
    structName: 'TestStruct',
    architecture: 'x64',
    compiler: 'clang',
    compileFlags: []
};

try {
    console.log('Analyzing TestStruct...');
    const result = native.analyze(request);

    if (result.success) {
        console.log('✓ Analysis successful!\n');

        const layout = result.layouts[0];
        console.log(`Struct: ${layout.name}`);
        console.log(`Size: ${layout.totalSize} bytes`);
        console.log(`Alignment: ${layout.alignment} bytes`);
        console.log(`\nMembers:`);

        layout.members.forEach(m => {
            console.log(`  +${m.offset.toString().padStart(2)} | ${m.size}B | ${m.name} (${m.type})`);
        });

        if (layout.padding.length > 0) {
            console.log(`\nPadding:`);
            layout.padding.forEach(p => {
                console.log(`  +${p.offset.toString().padStart(2)} | ${p.size}B | ${p.reason}`);
            });
        }

        if (layout.optimizations.length > 0) {
            console.log(`\nOptimizations:`);
            layout.optimizations.forEach(opt => {
                console.log(`  💡 ${opt.description}`);
                if (opt.bytesSaved > 0) {
                    console.log(`     Can save ${opt.bytesSaved} bytes`);
                    console.log(`     Suggested order: ${opt.suggestedOrder.join(', ')}`);
                }
            });
        }

        console.log('\n✓ All tests passed!');
    } else {
        console.error('✗ Analysis failed:', result.errorMessage);
        process.exit(1);
    }

} catch (error) {
    console.error('✗ Test error:', error);
    process.exit(1);
}
