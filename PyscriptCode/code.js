export const editor = CodeMirror.fromTextArea(
    document.getElementById('code-editor'),
    {
        lineNumbers: true,
        mode: 'python',
        theme: 'default',
        indentUnit: 4,
        indentWithTabs: false,
        lineWrapping: true,
        autoCloseBrackets: true
    }
);