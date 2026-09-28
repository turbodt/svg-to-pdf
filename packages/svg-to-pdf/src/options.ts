import type { PdfNamedSize, SvgPageOptions, SvgToPdfOptions } from "./types";

export const PDF_SIZES: Record<PdfNamedSize, readonly [number, number]> = {
  "a3-portrait": [841.89, 1190.551],
  "a3-landscape": [1190.551, 841.89],
  "a4-portrait": [595.276, 841.89],
  "a4-landscape": [841.89, 595.276],
  "a5-portrait": [419.528, 595.276],
  "a5-landscape": [595.276, 419.528],
  "letter-portrait": [612, 792],
  "letter-landscape": [792, 612],
  "legal-portrait": [612, 1008],
  "legal-landscape": [1008, 612]
};

export type WasmOptions = {
  containerWidth: number;
  containerHeight: number;
  includeContainers: boolean;
  mergeDuplicatedContainers: boolean;
  pdfWidth: number;
  pdfHeight: number;
  hasPdfSize: boolean;
};

export const normalizeOptions = (options: SvgToPdfOptions | SvgPageOptions = {}): WasmOptions => {
  let pdfWidth = 0;
  let pdfHeight = 0;
  let hasPdfSize = false;

  if ("pdfSize" in options && options.pdfSize) {
    [pdfWidth, pdfHeight] = PDF_SIZES[options.pdfSize];
    hasPdfSize = true;
  } else if ("pdfWidth" in options && "pdfHeight" in options && options.pdfWidth && options.pdfHeight) {
    pdfWidth = options.pdfWidth;
    pdfHeight = options.pdfHeight;
    hasPdfSize = true;
  }

  return {
    containerWidth: options.containerWidth ?? 2400,
    containerHeight: options.containerHeight ?? 3400,
    includeContainers: options.includeContainers ?? true,
    mergeDuplicatedContainers: options.mergeDuplicatedContainers ?? true,
    pdfWidth,
    pdfHeight,
    hasPdfSize
  };
};
