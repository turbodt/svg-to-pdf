## Usage


```sh
source ./tools/load-env.sh
./bin/main --pdf "$SVG_FILE" output.pdf
```

`./bin/main` writes a single multi-page PDF directly. In PDF mode, the output
argument is the final PDF filename.

To detect 2400x3400 SVG page containers and output A4 PDF pages:

```sh
./bin/main --pdf "$SVG_FILE" output.pdf \
    --container-width 2400 \
    --container-height 3400 \
    --pdf-size a4-portrait
```

To split an SVG into page SVG files instead, omit `--pdf`. In SVG mode, the
output argument is a directory for `page-*.svg` files:

```sh
./bin/main "$SVG_FILE" pages \
    --container-width 2400 \
    --container-height 3400
```

PDF page size can also be set explicitly with `--pdf-width` and
`--pdf-height`. Supported named sizes are `a3`, `a4`, `a5`, `letter`, and
`legal`, each with `-portrait` or `-landscape`.
