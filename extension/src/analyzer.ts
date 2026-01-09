import * as vscode from 'vscode';

// Type definitions matching the native module
export interface MemberInfo {
    name: string;
    type: string;
    offset: number;
    size: number;
    alignment: number;
    isBitfield: boolean;
    bitfieldWidth: number;
    bitfieldOffset: number;
}

export interface PaddingInfo {
    offset: number;
    size: number;
    reason: string;
}

export interface VTableInfo {
    pointerOffset: number;
    virtualFunctions: string[];
    hasVirtualBase: boolean;
}

export interface Optimization {
    description: string;
    bytesSaved: number;
    suggestedOrder: string[];
    confidence: number;
}

export interface StructLayout {
    name: string;
    qualifiedName: string;
    totalSize: number;
    alignment: number;
    usefulSize: number;
    isPolymorphic: boolean;
    isStandardLayout: boolean;
    members: MemberInfo[];
    padding: PaddingInfo[];
    vtable: VTableInfo;
    optimizations: Optimization[];
}

export interface AnalysisResult {
    success: boolean;
    errorMessage: string;
    layouts: StructLayout[];
}

interface NativeModule {
    analyze(request: {
        sourceCode: string;
        filePath: string;
        structName?: string;
        architecture: string;
        compiler: string;
        compileFlags?: string[];
    }): AnalysisResult;
}

export class Analyzer {
    private native: NativeModule | null = null;
    private cache: Map<string, AnalysisResult> = new Map();

    constructor() {
        try {
            // Load native module
            this.native = require('structsight-native');
        } catch (error) {
            vscode.window.showErrorMessage(
                'StructSight: Failed to load native module. Please ensure LLVM/Clang is installed.'
            );
            console.error('Native module load error:', error);
        }
    }

    async analyze(
        document: vscode.TextDocument,
        structName: string = ''
    ): Promise<AnalysisResult> {
        if (!this.native) {
            return {
                success: false,
                errorMessage: 'Native module not loaded',
                layouts: []
            };
        }

        // Get configuration
        const config = vscode.workspace.getConfiguration('structsight');
        const architecture = config.get<string>('architecture', 'x64');
        const compiler = config.get<string>('compiler', 'clang');

        // Create cache key
        const cacheKey = `${document.uri.toString()}-${structName}-${architecture}-${compiler}`;

        // Check cache
        if (this.cache.has(cacheKey)) {
            return this.cache.get(cacheKey)!;
        }

        // Prepare analysis request
        const request = {
            sourceCode: document.getText(),
            filePath: document.uri.fsPath,
            structName,
            architecture,
            compiler,
            compileFlags: this.getCompileFlags(document)
        };

        try {
            const result = this.native.analyze(request);

            // Cache successful results
            if (result.success) {
                this.cache.set(cacheKey, result);

                // Clear cache after 30 seconds
                setTimeout(() => this.cache.delete(cacheKey), 30000);
            }

            return result;

        } catch (error) {
            const errorMsg = error instanceof Error ? error.message : String(error);
            return {
                success: false,
                errorMessage: `Analysis failed: ${errorMsg}`,
                layouts: []
            };
        }
    }

    private getCompileFlags(document: vscode.TextDocument): string[] {
        const flags: string[] = [];

        // Try to read from compile_commands.json if available
        // For now, use basic flags
        flags.push('-x', 'c++');

        // Check C++ standard from file extension or content
        const text = document.getText();
        if (text.includes('std::') || text.includes('::')) {
            flags.push('-std=c++17');
        }

        return flags;
    }

    clearCache(): void {
        this.cache.clear();
    }
}
