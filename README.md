## Usage


```sh
mkdir -p "$(dirname "$SVG_FILE")/pages"

./bin/main "$SVG_FILE" "$(dirname "$SVG_FILE")/pages"

cd "$(dirname "$SVG_FILE")"

for i in {1..$(ls -l pages/page-*.svg | wc -l)}; do \
    inkscape "./pages/page-$i.svg" \
    --actions="export-type:pdf;export-filename:./pages/page-$i.pdf;export-do"; \
done;

pdfunite pages/page-*.pdf "./$(basename "$(pwd)").pdf"
```
