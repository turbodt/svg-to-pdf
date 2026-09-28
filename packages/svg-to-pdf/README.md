# @zanny/svg-to-pdf

Browser-first SVG to PDF conversion backed by the WASM build from this repo.

## Usage

```ts
import { createSvgToPdfConverter } from "@zanny/svg-to-pdf";

const converter = await createSvgToPdfConverter({
  wasmUrl: "/svg-to-pdf.wasm",
});

const pdf = await converter.convertToPdf(svgText, {
  containerWidth: 2400,
  containerHeight: 3400,
  includeContainers: false,
  pdfSize: "a4-portrait",
});
```

You can also pass bytes directly:

```ts
const converter = await createSvgToPdfConverter({ wasmBytes });
```

The package includes the compiled WASM at the export path
`@zanny/svg-to-pdf/svg-to-pdf.wasm` for bundlers that support asset URLs.

## SVG Pages

Single page extraction uses 1-based page numbers:

```ts
const firstPage = await converter.convertPageToSvg(svgText, 1);
```

All pages are returned as a normal 0-based JavaScript array:

```ts
const pages = await converter.convertPagesToSvg(svgText);
```

## Build

Build the WASM first, then the package:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk make wasm
npm run build --workspace packages/svg-to-pdf
```
