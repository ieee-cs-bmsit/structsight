import * as vscode from 'vscode';
import { HoverProvider } from './hoverProvider';
import { WebviewProvider } from './webviewProvider';
import { RefactoringProvider } from './refactoring';

export function activate(context: vscode.ExtensionContext) {
    console.log('StructSight extension is now active');

    // Register hover provider for C/C++ files
    const hoverProvider = new HoverProvider(context);
    context.subscriptions.push(
        vscode.languages.registerHoverProvider(
            ['cpp', 'c'],
            hoverProvider
        )
    );

    // Register webview provider
    const webviewProvider = new WebviewProvider(context);

    // Register commands
    context.subscriptions.push(
        vscode.commands.registerCommand('structsight.showLayout', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active editor');
                return;
            }

            // Get selected text or word at cursor
            const selection = editor.selection;
            const text = editor.document.getText(selection);
            const structName = text || getWordAtPosition(editor.document, selection.active);

            if (!structName) {
                vscode.window.showErrorMessage('No struct/class selected');
                return;
            }

            // Show layout in webview
            await webviewProvider.showLayout(editor.document, structName);
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('structsight.analyzeFile', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active editor');
                return;
            }

            await webviewProvider.showLayout(editor.document, '');
        })
    );

    // Register refactoring provider
    const refactoringProvider = new RefactoringProvider();
    context.subscriptions.push(
        vscode.languages.registerCodeActionsProvider(
            ['cpp', 'c'],
            refactoringProvider,
            {
                providedCodeActionKinds: RefactoringProvider.providedCodeActionKinds
            }
        )
    );
}

function getWordAtPosition(document: vscode.TextDocument, position: vscode.Position): string {
    const range = document.getWordRangeAtPosition(position);
    return range ? document.getText(range) : '';
}

export function deactivate() {
    console.log('StructSight extension deactivated');
}
