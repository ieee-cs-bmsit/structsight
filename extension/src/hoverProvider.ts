import * as vscode from 'vscode';
import { Analyzer } from './analyzer';

export class HoverProvider implements vscode.HoverProvider {
    private analyzer: Analyzer;

    constructor(context: vscode.ExtensionContext) {
        this.analyzer = new Analyzer();
    }

    async provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Hover | undefined> {
        // Check if hover is enabled
        const config = vscode.workspace.getConfiguration('structsight');
        if (!config.get<boolean>('enableHoverInfo', true)) {
            return undefined;
        }

        // Get the word at the cursor position
        const range = document.getWordRangeAtPosition(position);
        if (!range) {
            return undefined;
        }

        const word = document.getText(range);

        // Check if this looks like a struct/class definition or usage
        const line = document.lineAt(position.line).text;
        const isDefinition = /\b(struct|class)\s+/.test(line);

        if (!isDefinition) {
            return undefined; // Only show on definitions for now
        }

        try {
            // Analyze the struct
            const result = await this.analyzer.analyze(document, word);

            if (!result.success || result.layouts.length === 0) {
                return undefined;
            }

            const layout = result.layouts[0];

            // Create hover markdown
            const markdown = new vscode.MarkdownString();
            markdown.isTrusted = true;

            markdown.appendMarkdown(`### 🔍 ${layout.name}\n\n`);
            markdown.appendMarkdown(`**Size:** ${layout.totalSize} bytes  \n`);
            markdown.appendMarkdown(`**Alignment:** ${layout.alignment} bytes  \n`);

            if (layout.padding.length > 0) {
                const totalPadding = layout.padding.reduce((sum, p) => sum + p.size, 0);
                markdown.appendMarkdown(`**Padding:** ${totalPadding} bytes (${((totalPadding / layout.totalSize) * 100).toFixed(1)}%)  \n`);
            }

            if (layout.isPolymorphic) {
                markdown.appendMarkdown(`**Polymorphic:** Yes (has vtable)  \n`);
            }

            // Show optimization hints
            if (layout.optimizations.length > 0) {
                const opt = layout.optimizations[0];
                if (opt.bytesSaved > 0) {
                    markdown.appendMarkdown(`\n💡 Can save ${opt.bytesSaved} bytes by reordering members  \n`);
                }
            }

            markdown.appendMarkdown(`\n[Show Detailed Layout](command:structsight.showLayout)`);

            return new vscode.Hover(markdown, range);

        } catch (error) {
            console.error('Hover error:', error);
            return undefined;
        }
    }
}
