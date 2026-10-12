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

## WASM and TypeScript

Build the browser WASM module:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk make wasm
```

Build and test the TypeScript package:

```sh
npm install
npm run build --workspace packages/svg-to-pdf
npm run test --workspace packages/svg-to-pdf
```

The package is `@zanny/svg-to-pdf` and exposes PDF conversion plus single/all
SVG page extraction from browser code.

## License

This project is licensed under the GNU Affero General Public
License v3.0 or later (AGPL-3.0-or-later).
See the [LICENSE](LICENSE) file for details.

## Dependencies

This project uses libxml2, libc and libm.

- libxml2 is distributed under the MIT License.
- libc and libm are provided by the target system or toolchain.

Please refer to the respective upstream projects and toolchain
documentation for their licensing terms.
