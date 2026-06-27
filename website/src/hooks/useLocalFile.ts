export function downloadFile(name: string, content: string, type = 'text/plain') { const blob = new Blob([content], { type }); const url = URL.createObjectURL(blob); const a = document.createElement('a'); a.href = url; a.download = name; a.click(); URL.revokeObjectURL(url); }
export function readMarkdownFile(file: File): Promise<string> { return file.text(); }
