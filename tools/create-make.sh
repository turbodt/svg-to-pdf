#!/bin/sh
set -eu

usage() {
    echo "Usage: $0 <SVG_FILE>" >&2
    exit 1
}

error() {
    echo "Error: $1" >&2
    exit 1
}

check_commands() {
    for cmd in pdfunite; do
        command -v "$cmd" >/dev/null 2>&1 || error "'$cmd' command not found. Please install it."
    done
}

check_input() {
    if [ $# -lt 1 ]; then
        usage
    fi

    SVG_FILE=$1

    if [ ! -f "$SVG_FILE" ]; then
        error "File '$SVG_FILE' does not exist."
    fi

    case "$SVG_FILE" in
        *.svg) ;;
        *) error "File must have .svg extension." ;;
    esac
}

prepare_paths() {
    SVG_DIR=$(dirname "$SVG_FILE")
    SVG_BASENAME=$(basename "$SVG_FILE" .svg)
    PAGES_DIR="$SVG_DIR/pages"
    mkdir -p "$PAGES_DIR"
}

split_svg() {
    if [ ! -x "./bin/main" ]; then
        error "./bin/main not found or not executable."
    fi

    echo "Splitting SVG into PDF pages..."
    ./bin/main "$SVG_FILE" "$PAGES_DIR" --pdf "$@"
}

count_pages() {
    PAGE_COUNT=$(ls "$PAGES_DIR"/page-*.pdf 2>/dev/null | wc -l | tr -d ' ')
    if [ "$PAGE_COUNT" -eq 0 ]; then
        error "No page-*.pdf files found in $PAGES_DIR."
    fi
    echo "$PAGE_COUNT"
}

merge_pdfs() {
    OUTPUT_PDF="$SVG_DIR/$SVG_BASENAME.pdf"
    if ls "$PAGES_DIR"/page-*.pdf >/dev/null 2>&1; then
        echo "Merging PDFs into $OUTPUT_PDF..."
        pdfunite "$PAGES_DIR"/page-*.pdf "$OUTPUT_PDF" || error "Failed to merge PDFs."
        echo "Done! Output PDF: $OUTPUT_PDF"
    else
        error "No page-*.pdf files to merge."
    fi
}

### Main execution
check_commands
check_input "$@"
shift;
prepare_paths
split_svg "$@"
PAGE_COUNT=$(count_pages)
merge_pdfs
