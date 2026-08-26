#!/usr/bin/env python3
"""
AudioForge Manual PDF Converter (Python/WeasyPrint)
Converts Markdown manuals to professional PDF format without LaTeX
"""

import os
import glob
from datetime import datetime
import markdown
from weasyprint import HTML, CSS
from weasyprint.text.fonts import FontConfiguration

# CSS styling for professional PDF output
CSS_STYLE = """
@page {
    size: Letter;
    margin: 1in;

    @top-center {
        content: "AudioForge User Manual";
        font-family: system-ui, -apple-system, sans-serif;
        font-size: 9pt;
        color: #666;
    }

    @bottom-center {
        content: "Page " counter(page) " of " counter(pages);
        font-family: system-ui, -apple-system, sans-serif;
        font-size: 9pt;
        color: #666;
    }
}

body {
    font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
    font-size: 11pt;
    line-height: 1.6;
    color: #333;
}

h1 {
    font-size: 24pt;
    font-weight: 700;
    color: #1a1a1a;
    border-bottom: 3px solid #0066cc;
    padding-bottom: 8pt;
    margin-top: 24pt;
    margin-bottom: 16pt;
    page-break-after: avoid;
}

h2 {
    font-size: 18pt;
    font-weight: 600;
    color: #2c2c2c;
    border-bottom: 2px solid #0088ff;
    padding-bottom: 6pt;
    margin-top: 20pt;
    margin-bottom: 12pt;
    page-break-after: avoid;
}

h3 {
    font-size: 14pt;
    font-weight: 600;
    color: #404040;
    margin-top: 16pt;
    margin-bottom: 10pt;
    page-break-after: avoid;
}

h4 {
    font-size: 12pt;
    font-weight: 600;
    color: #505050;
    margin-top: 12pt;
    margin-bottom: 8pt;
    page-break-after: avoid;
}

p {
    margin-bottom: 10pt;
    text-align: justify;
}

code {
    font-family: 'SF Mono', 'Monaco', 'Courier New', monospace;
    font-size: 10pt;
    background-color: #f5f5f5;
    padding: 2pt 4pt;
    border-radius: 3pt;
    color: #d14;
}

pre {
    background-color: #f8f8f8;
    border: 1px solid #ddd;
    border-radius: 4pt;
    padding: 10pt;
    margin: 10pt 0;
    overflow-x: auto;
    page-break-inside: avoid;
}

pre code {
    background-color: transparent;
    padding: 0;
    color: #333;
}

ul, ol {
    margin: 10pt 0;
    padding-left: 24pt;
}

li {
    margin-bottom: 6pt;
}

table {
    width: 100%;
    border-collapse: collapse;
    margin: 12pt 0;
    page-break-inside: avoid;
}

th, td {
    border: 1px solid #ddd;
    padding: 8pt;
    text-align: left;
}

th {
    background-color: #f0f0f0;
    font-weight: 600;
}

blockquote {
    border-left: 4pt solid #0066cc;
    margin: 12pt 0;
    padding-left: 12pt;
    color: #555;
    font-style: italic;
}

a {
    color: #0066cc;
    text-decoration: none;
}

strong {
    font-weight: 600;
}

em {
    font-style: italic;
}

hr {
    border: none;
    border-top: 1px solid #ccc;
    margin: 20pt 0;
}

.toc {
    background-color: #f8f8f8;
    border: 1px solid #ddd;
    border-radius: 4pt;
    padding: 12pt;
    margin: 16pt 0;
}

.toc h2 {
    margin-top: 0;
    font-size: 16pt;
}

/* Prevent widows and orphans */
h1, h2, h3, h4, h5, h6 {
    page-break-after: avoid;
}

p {
    orphans: 3;
    widows: 3;
}
"""

def convert_markdown_to_pdf(md_file):
    """Convert a single Markdown file to PDF"""

    # Read Markdown content
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()

    # Convert Markdown to HTML
    md = markdown.Markdown(extensions=[
        'extra',           # Tables, fenced code blocks, etc.
        'codehilite',      # Syntax highlighting
        'toc',             # Table of contents
        'nl2br',           # New line to <br>
        'sane_lists',      # Better list handling
    ])
    html_body = md.convert(md_content)

    # Extract plugin name from filename
    plugin_name = os.path.basename(md_file).replace('-Manual.md', '')

    # Create full HTML document
    html_content = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <title>AudioForge {plugin_name} User Manual</title>
    </head>
    <body>
        {html_body}
    </body>
    </html>
    """

    # Generate PDF filename
    pdf_file = md_file.replace('.md', '.pdf')

    # Create PDF
    font_config = FontConfiguration()
    html = HTML(string=html_content)
    css = CSS(string=CSS_STYLE, font_config=font_config)
    html.write_pdf(pdf_file, stylesheets=[css], font_config=font_config)

    return pdf_file

def main():
    """Convert all manual files to PDF"""

    print("\033[0;34m" + "=" * 40 + "\033[0m")
    print("\033[0;34mAudioForge Manual PDF Converter\033[0m")
    print("\033[0;34m(Python/WeasyPrint)\033[0m")
    print("\033[0;34m" + "=" * 40 + "\033[0m")
    print()

    # Find all manual files
    manual_files = sorted(glob.glob('*-Manual.md'))

    if not manual_files:
        print("\033[0;31mError: No manual files found (*-Manual.md)\033[0m")
        return 1

    print(f"\033[0;34mFound {len(manual_files)} manuals to convert\033[0m")
    print()

    success_count = 0
    fail_count = 0
    pdf_files = []

    for md_file in manual_files:
        print(f"\033[1;33m→ Converting {md_file}...\033[0m")

        try:
            pdf_file = convert_markdown_to_pdf(md_file)
            pdf_size = os.path.getsize(pdf_file)
            pdf_size_kb = pdf_size / 1024

            print(f"\033[0;32m✓ Created {os.path.basename(pdf_file)} ({pdf_size_kb:.1f} KB)\033[0m")
            pdf_files.append(pdf_file)
            success_count += 1

        except Exception as e:
            print(f"\033[0;31m✗ Failed to convert {md_file}: {e}\033[0m")
            fail_count += 1

    print()
    print("\033[0;34m" + "=" * 40 + "\033[0m")
    print("\033[0;32mConversion complete!\033[0m")
    print("\033[0;34m" + "=" * 40 + "\033[0m")
    print()
    print(f"\033[0;32mSuccess: {success_count} / {len(manual_files)}\033[0m")

    if fail_count > 0:
        print(f"\033[0;31mFailed:  {fail_count} / {len(manual_files)}\033[0m")

    print()
    print(f"PDF files created in: {os.getcwd()}")
    print()

    if pdf_files:
        print("\033[0;34mCreated PDFs:\033[0m")
        total_size = 0
        for pdf_file in pdf_files:
            size = os.path.getsize(pdf_file)
            total_size += size
            size_kb = size / 1024
            print(f"  {os.path.basename(pdf_file)} ({size_kb:.1f} KB)")

        total_size_mb = total_size / (1024 * 1024)
        print()
        print(f"\033[0;32mTotal size: {total_size_mb:.1f} MB\033[0m")

    print()
    print("\033[0;34mDone!\033[0m")

    return 0 if fail_count == 0 else 1

if __name__ == '__main__':
    exit(main())
