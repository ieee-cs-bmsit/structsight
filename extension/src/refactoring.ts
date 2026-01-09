import * as vscode from 'vscode';
import { Analyzer, StructLayout, Optimization } from './analyzer';

export class RefactoringProvider implements vscode.CodeActionProvider {
    public static readonly providedCodeActionKinds = [
        vscode.CodeActionKind.RefactorRewrite
    ];

    private analyzer: Analyzer;

    constructor() {
        this.analyzer = new Analyzer();
    }

    async provideCodeActions(
        document: vscode.TextDocument,
        range: vscode.Range | vscode.Selection,
        context: vscode.CodeActionContext,
        token: vscode.CancellationToken
    ): Promise<vscode.CodeAction[]> {
        const actions: vscode.CodeAction[] = [];

        // Get word at cursor
        const wordRange = document.getWordRangeAtPosition(range.start);
        if (!wordRange) {
            return actions;
        }

        const word = document.getText(wordRange);

        // Check if this is a struct/class
        const line = document.lineAt(range.start.line).text;
        if (!/\b(struct|class)\s+/.test(line)) {
            return actions;
        }

        try {
            const result = await this.analyzer.analyze(document, word);

            if (!result.success || result.layouts.length === 0) {
                return actions;
            }

            const layout = result.layouts[0];

            // Create code actions for each optimization
            for (const opt of layout.optimizations) {
                if (opt.suggestedOrder.length > 0 && opt.bytesSaved > 0) {
                    const action = new vscode.CodeAction(
                        `Reorder members to save ${opt.bytesSaved} bytes`,
                        vscode.CodeActionKind.RefactorRewrite
                    );

                    action.edit = await this.createReorderEdit(document, layout, opt);
                    action.isPreferred = opt.confidence > 0.9;

                    actions.push(action);
                }
            }

        } catch (error) {
            console.error('Code action error:', error);
        }

        return actions;
    }

    private async createReorderEdit(
        document: vscode.TextDocument,
        layout: StructLayout,
        optimization: Optimization
    ): Promise<vscode.WorkspaceEdit> {
        const edit = new vscode.WorkspaceEdit();

        // Find the struct definition
        const text = document.getText();
        const structRegex = new RegExp(
            `(struct|class)\\s+${layout.name}\\s*\\{([^}]+)\\}`,
            'gs'
        );

        const match = structRegex.exec(text);
        if (!match) {
            return edit;
        }

        const bodyText = match[2];
        const bodyStart = match.index + match[1].length + layout.name.length + 2;

        // Extract member declarations
        const memberLines = new Map<string, string>();
        const memberRegex = /^\s*(.+?\s+)(\w+)\s*;/gm;

        let memberMatch;
        while ((memberMatch = memberRegex.exec(bodyText)) !== null) {
            const memberName = memberMatch[2];
            const fullLine = memberMatch[0];
            memberLines.set(memberName, fullLine);
        }

        // Build new body with reordered members
        const reorderedLines: string[] = [];
        for (const memberName of optimization.suggestedOrder) {
            const line = memberLines.get(memberName);
            if (line) {
                reorderedLines.push(line);
            }
        }

        // Find the range of member declarations
        const startPos = document.positionAt(bodyStart);
        const endPos = document.positionAt(bodyStart + bodyText.length);

        // Create the edit
        const newBody = '\n' + reorderedLines.join('\n') + '\n';
        edit.replace(
            document.uri,
            new vscode.Range(startPos, endPos),
            newBody
        );

        return edit;
    }
}
