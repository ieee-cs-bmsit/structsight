import * as vscode from 'vscode';
import { Analyzer, StructLayout } from './analyzer';
import * as path from 'path';

export class WebviewProvider {
    private panel: vscode.WebviewPanel | undefined;
    private analyzer: Analyzer;

    constructor(private context: vscode.ExtensionContext) {
        this.analyzer = new Analyzer();
    }

    async showLayout(document: vscode.TextDocument, structName: string): Promise<void> {
        // Analyze the document
        const result = await this.analyzer.analyze(document, structName);

        if (!result.success) {
            vscode.window.showErrorMessage(`Analysis failed: ${result.errorMessage}`);
            return;
        }

        if (result.layouts.length === 0) {
            vscode.window.showInformationMessage('No structs/classes found');
            return;
        }

        // Create or reveal webview
        if (this.panel) {
            this.panel.reveal(vscode.ViewColumn.Beside);
        } else {
            this.panel = vscode.window.createWebviewPanel(
                'structsightLayout',
                'Memory Layout',
                vscode.ViewColumn.Beside,
                {
                    enableScripts: true,
                    retainContextWhenHidden: true
                }
            );

            this.panel.onDidDispose(() => {
                this.panel = undefined;
            });

            // Handle messages from webview
            this.panel.webview.onDidReceiveMessage(
                async message => {
                    switch (message.command) {
                        case 'applyOptimization':
                            await this.applyOptimization(document, message.layout, message.optimization);
                            break;
                    }
                }
            );
        }

        // Update webview content
        this.panel.webview.html = this.getWebviewContent(result.layouts);
    }

    private async applyOptimization(
        document: vscode.TextDocument,
        layout: StructLayout,
        optimization: any
    ): Promise<void> {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document !== document) {
            vscode.window.showErrorMessage('Document not active');
            return;
        }

        // For now, show a message about the optimization
        // Full implementation would create a WorkspaceEdit
        const message = `Would reorder members to: ${optimization.suggestedOrder.join(', ')}`;
        vscode.window.showInformationMessage(message);
    }

    private getWebviewContent(layouts: StructLayout[]): string {
        const config = vscode.workspace.getConfiguration('structsight');
        const architecture = config.get<string>('architecture', 'x64');
        const compiler = config.get<string>('compiler', 'clang');
        const cacheLineSize = config.get<number>('cacheLineSize', 64);

        return `<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Memory Layout</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            padding: 20px;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        .header {
            margin-bottom: 30px;
        }
        
        .config {
            display: flex;
            gap: 20px;
            margin-bottom: 20px;
            padding: 10px;
            background: var(--vscode-editor-inactiveSelectionBackground);
            border-radius: 4px;
        }
        
        .struct-section {
            margin-bottom: 40px;
            padding: 20px;
            background: var(--vscode-editor-inactiveSelectionBackground);
            border-radius: 8px;
        }
        
        .struct-header {
            margin-bottom: 20px;
        }
        
        .struct-name {
            font-size: 24px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        
        .struct-stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .stat {
            padding: 10px;
            background: var(--vscode-input-background);
            border-radius: 4px;
        }
        
        .stat-label {
            font-size: 11px;
            color: var(--vscode-descriptionForeground);
            text-transform: uppercase;
            margin-bottom: 4px;
        }
        
        .stat-value {
            font-size: 18px;
            font-weight: bold;
        }
        
        .layout-viz {
            margin: 20px 0;
        }
        
        .memory-block {
            display: flex;
            flex-direction: column;
            border: 1px solid var(--vscode-panel-border);
            border-radius: 4px;
            overflow: hidden;
        }
        
        .member-row, .padding-row {
            display: flex;
            align-items: center;
            padding: 8px 12px;
            border-bottom: 1px solid var(--vscode-panel-border);
            min-height: 40px;
        }
        
        .member-row {
            background: var(--vscode-editor-background);
        }
        
        .padding-row {
            background: repeating-linear-gradient(
                45deg,
                #ff000020,
                #ff000020 10px,
                #ff000010 10px,
                #ff000010 20px
            );
        }
        
        .offset {
            width: 80px;
            font-family: 'Courier New', monospace;
            color: var(--vscode-descriptionForeground);
            font-size: 11px;
        }
        
        .size-indicator {
            width: 60px;
            text-align: center;
            font-size: 11px;
            color: var(--vscode-descriptionForeground);
        }
        
        .member-info {
            flex: 1;
            display: flex;
            gap: 15px;
        }
        
        .member-name {
            font-weight: bold;
            min-width: 150px;
        }
        
        .member-type {
            color: var(--vscode-descriptionForeground);
        }
        
        .optimizations {
            margin-top: 20px;
        }
        
        .optimization-card {
            padding: 15px;
            background: var(--vscode-input-background);
            border-left: 3px solid #4CAF50;
            border-radius: 4px;
            margin-bottom: 10px;
        }
        
        .optimization-card.warning {
            border-left-color: #FF9800;
        }
        
        .opt-description {
            margin-bottom: 10px;
        }
        
        .opt-savings {
            font-weight: bold;
            color: #4CAF50;
            margin-bottom: 10px;
        }
        
        .apply-btn {
            padding: 6px 12px;
            background: var(--vscode-button-background);
            color: var(--vscode-button-foreground);
            border: none;
            border-radius: 3px;
            cursor: pointer;
        }
        
        .apply-btn:hover {
            background: var(--vscode-button-hoverBackground);
        }
        
        .vtable-info {
            margin-top: 15px;
            padding: 10px;
            background: var(--vscode-input-background);
            border-radius: 4px;
        }
        
        .cache-line-marker {
            height: 2px;
            background: #2196F3;
            position: relative;
            margin: 2px 0;
        }
        
        .cache-line-label {
            position: absolute;
            right: 10px;
            top: -8px;
            font-size: 10px;
            color: #2196F3;
            background: var(--vscode-editor-background);
            padding: 0 4px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔍 StructSight - Memory Layout Analysis</h1>
            <div class="config">
                <div><strong>Architecture:</strong> ${architecture}</div>
                <div><strong>Compiler:</strong> ${compiler}</div>
                <div><strong>Cache Line:</strong> ${cacheLineSize} bytes</div>
            </div>
        </div>
        
        ${layouts.map(layout => this.renderLayout(layout, cacheLineSize)).join('')}
    </div>
    
    <script>
        const vscode = acquireVsCodeApi();
        
        function applyOptimization(layoutIndex, optIndex) {
            const layout = ${JSON.stringify(layouts)}[layoutIndex];
            const optimization = layout.optimizations[optIndex];
            
            vscode.postMessage({
                command: 'applyOptimization',
                layout: layout,
                optimization: optimization
            });
        }
    </script>
</body>
</html>`;
    }

    private renderLayout(layout: StructLayout, cacheLineSize: number): string {
        const paddingBytes = layout.padding.reduce((sum, p) => p.size + sum, 0);
        const paddingPercent = ((paddingBytes / layout.totalSize) * 100).toFixed(1);

        return `
        <div class="struct-section">
            <div class="struct-header">
                <div class="struct-name">${layout.name}</div>
                <div class="struct-stats">
                    <div class="stat">
                        <div class="stat-label">Total Size</div>
                        <div class="stat-value">${layout.totalSize} bytes</div>
                    </div>
                    <div class="stat">
                        <div class="stat-label">Alignment</div>
                        <div class="stat-value">${layout.alignment} bytes</div>
                    </div>
                    <div class="stat">
                        <div class="stat-label">Padding</div>
                        <div class="stat-value">${paddingBytes} bytes (${paddingPercent}%)</div>
                    </div>
                    <div class="stat">
                        <div class="stat-label">Useful Size</div>
                        <div class="stat-value">${layout.usefulSize} bytes</div>
                    </div>
                </div>
            </div>
            
            <div class="layout-viz">
                <h3>Memory Layout</h3>
                <div class="memory-block">
                    ${this.renderMemoryMap(layout, cacheLineSize)}
                </div>
            </div>
            
            ${layout.isPolymorphic ? this.renderVTable(layout) : ''}
            
            ${layout.optimizations.length > 0 ? this.renderOptimizations(layout) : ''}
        </div>
        `;
    }

    private renderMemoryMap(layout: StructLayout, cacheLineSize: number): string {
        const items: Array<{ type: 'member' | 'padding', offset: number, data: any }> = [];

        // Add members
        layout.members.forEach(m => {
            items.push({ type: 'member', offset: m.offset, data: m });
        });

        // Add padding
        layout.padding.forEach(p => {
            items.push({ type: 'padding', offset: p.offset, data: p });
        });

        // Sort by offset
        items.sort((a, b) => a.offset - b.offset);

        let html = '';
        let lastOffset = 0;

        items.forEach(item => {
            // Check for cache line boundaries
            const crossesCacheLine = Math.floor(lastOffset / cacheLineSize) !== Math.floor(item.offset / cacheLineSize);
            if (crossesCacheLine) {
                const lineNum = Math.floor(item.offset / cacheLineSize);
                html += `<div class="cache-line-marker">
                    <span class="cache-line-label">Cache Line ${lineNum}</span>
                </div>`;
            }

            if (item.type === 'member') {
                const m = item.data;
                html += `<div class="member-row">
                    <div class="offset">+${m.offset}</div>
                    <div class="size-indicator">${m.size}B</div>
                    <div class="member-info">
                        <div class="member-name">${m.name}</div>
                        <div class="member-type">${m.type}</div>
                    </div>
                </div>`;
                lastOffset = m.offset + m.size;
            } else {
                const p = item.data;
                html += `<div class="padding-row">
                    <div class="offset">+${p.offset}</div>
                    <div class="size-indicator">${p.size}B</div>
                    <div class="member-info">
                        <div class="member-name">⚠️ Padding</div>
                        <div class="member-type">${p.reason}</div>
                    </div>
                </div>`;
                lastOffset = p.offset + p.size;
            }
        });

        return html;
    }

    private renderVTable(layout: StructLayout): string {
        return `
        <div class="vtable-info">
            <h4>🔗 Virtual Table</h4>
            <p><strong>VTable Pointer Offset:</strong> ${layout.vtable.pointerOffset}</p>
            <p><strong>Virtual Functions (${layout.vtable.virtualFunctions.length}):</strong></p>
            <ul>
                ${layout.vtable.virtualFunctions.map(f => `<li>${f}</li>`).join('')}
            </ul>
        </div>
        `;
    }

    private renderOptimizations(layout: StructLayout): string {
        return `
        <div class="optimizations">
            <h3>💡 Optimization Suggestions</h3>
            ${layout.optimizations.map((opt, idx) => `
                <div class="optimization-card ${opt.bytesSaved === 0 ? 'warning' : ''}">
                    <div class="opt-description">${opt.description}</div>
                    ${opt.bytesSaved > 0 ?
                `<div class="opt-savings">💾 Can save ${opt.bytesSaved} bytes</div>
                         <button class="apply-btn" onclick="applyOptimization(0, ${idx})">
                            Apply Reordering
                         </button>`
                : ''}
                    ${opt.confidence < 1.0 ?
                `<div style="font-size: 11px; color: var(--vscode-descriptionForeground);">
                            Confidence: ${(opt.confidence * 100).toFixed0()}%
                         </div>`
                : ''}
                </div>
            `).join('')}
        </div>
        `;
    }
}
