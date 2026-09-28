## Usage


```sh
source ./tools/load-env.sh
./tools/create-make.sh "$SVG_FILE"
```

The script splits the SVG into page PDFs with `./bin/main --pdf` and merges
them with `pdfunite`.

To detect 2400x3400 SVG page containers and output A4 PDF pages:

```sh
./tools/create-make.sh "$SVG_FILE" \
    --container-width 2400 \
    --container-height 3400 \
    --pdf-size a4-portrait
```

PDF page size can also be set explicitly with `--pdf-width` and
`--pdf-height`. Supported named sizes are `a3`, `a4`, `a5`, `letter`, and
`legal`, each with `-portrait` or `-landscape`.
