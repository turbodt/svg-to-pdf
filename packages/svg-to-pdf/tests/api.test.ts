import { readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { describe, expect, it } from "vitest";
import {
  createSvgToPdfConverter,
  pdfBytesToBlob,
  svgTextToBlob,
  type SvgToPdfOptions
} from "../src";

const __dirname = dirname(fileURLToPath(import.meta.url));
const wasmPath = resolve(__dirname, "../../../wasm/dist/svg-to-pdf.wasm");

describe("@zanny/svg-to-pdf", () => {
  it("converts a simple SVG to PDF bytes", async () => {
    const converter = await createSvgToPdfConverter({ wasmBytes: await readFile(wasmPath) });
    const pdf = await converter.convertToPdf(
      '<svg xmlns="http://www.w3.org/2000/svg" width="100" height="100"><rect x="10" y="10" width="80" height="80" fill="red"/></svg>',
      { pdfSize: "a4-portrait" }
    );

    expect(new TextDecoder().decode(pdf.slice(0, 8))).toBe("%PDF-1.4");
    expect(pdf.length).toBeGreaterThan(100);
  });

  it("returns a 1-based SVG page and all SVG pages", async () => {
    const converter = await createSvgToPdfConverter({ wasmBytes: await readFile(wasmPath) });
    const svg = await readFile(resolve(__dirname, "../../../examples/input/2025-07-22.svg"));

    const firstPage = await converter.convertPageToSvg(svg, 1, {
      containerWidth: 2400,
      containerHeight: 3400,
      includeContainers: false
    });
    const pages = await converter.convertPagesToSvg(svg, {
      containerWidth: 2400,
      containerHeight: 3400,
      includeContainers: false
    });

    expect(new TextDecoder().decode(firstPage.slice(0, 5))).toContain("<?xml");
    expect(pages).toHaveLength(7);
    expect(pages[0].length).toBe(firstPage.length);
  });

  it("provides browser Blob helpers", async () => {
    expect(await svgTextToBlob("<svg />").text()).toBe("<svg />");
    expect(pdfBytesToBlob(new Uint8Array([1, 2, 3])).type).toBe("application/pdf");
  });

  it("types pdfSize as mutually exclusive with explicit dimensions", () => {
    const named: SvgToPdfOptions = { pdfSize: "a4-portrait" };
    const explicit: SvgToPdfOptions = { pdfWidth: 595, pdfHeight: 842 };

    expect(named.pdfSize).toBe("a4-portrait");
    expect(explicit.pdfWidth).toBe(595);
  });
});
