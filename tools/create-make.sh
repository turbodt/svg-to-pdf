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
    for cmd in inkscape pdfunite; do
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

    echo "Splitting SVG into pages..."
    ./bin/main "$SVG_FILE" "$PAGES_DIR" $@
}

count_pages() {
    PAGE_COUNT=$(ls "$PAGES_DIR"/page-*.svg 2>/dev/null | wc -l | tr -d ' ')
    if [ "$PAGE_COUNT" -eq 0 ]; then
        error "No page-*.svg files found in $PAGES_DIR."
    fi
    echo "$PAGE_COUNT"
}

convert_pages() {
    echo "Converting $PAGE_COUNT pages to PDF..."
    i=1
    while [ $i -le $PAGE_COUNT ]; do
        PAGE_SVG="$PAGES_DIR/page-$i.svg"
        PAGE_PDF="$PAGES_DIR/page-$i.pdf"
        if [ -f "$PAGE_SVG" ]; then
            inkscape "$PAGE_SVG" --actions="export-type:pdf;export-filename:$PAGE_PDF;export-do" || {
                echo "Warning: failed to convert $PAGE_SVG"
            }
        else
            echo "Warning: $PAGE_SVG not found, skipping."
        fi
        i=$((i+1))
    done
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
convert_pages
merge_pdfs
