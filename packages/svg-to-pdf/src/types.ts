export type PdfNamedSize =
  | "a3-portrait"
  | "a3-landscape"
  | "a4-portrait"
  | "a4-landscape"
  | "a5-portrait"
  | "a5-landscape"
  | "letter-portrait"
  | "letter-landscape"
  | "legal-portrait"
  | "legal-landscape";

type NamedPdfSize = {
  pdfSize: PdfNamedSize;
  pdfWidth?: never;
  pdfHeight?: never;
};

type ExplicitPdfSize = {
  pdfSize?: never;
  pdfWidth: number;
  pdfHeight: number;
};

type DefaultPdfSize = {
  pdfSize?: never;
  pdfWidth?: never;
  pdfHeight?: never;
};

export type PdfSizeOptions = NamedPdfSize | ExplicitPdfSize | DefaultPdfSize;

export type SvgInput = string | Uint8Array | ArrayBuffer | Blob;

export type SvgPageOptions = {
  containerWidth?: number;
  containerHeight?: number;
  includeContainers?: boolean;
  mergeDuplicatedContainers?: boolean;
};

export type SvgToPdfOptions = SvgPageOptions & PdfSizeOptions;

export type WasmSource =
  | { wasmUrl: string; wasmBytes?: never }
  | { wasmBytes: ArrayBuffer | Uint8Array; wasmUrl?: never };

export type SvgToPdfConverter = {
  convertToPdf(svg: SvgInput, options?: SvgToPdfOptions): Promise<Uint8Array>;
  convertToPdfBlob(svg: SvgInput, options?: SvgToPdfOptions): Promise<Blob>;
  convertPageToSvg(svg: SvgInput, pageNumber: number, options?: SvgPageOptions): Promise<Uint8Array>;
  convertPageToSvgBlob(svg: SvgInput, pageNumber: number, options?: SvgPageOptions): Promise<Blob>;
  convertPagesToSvg(svg: SvgInput, options?: SvgPageOptions): Promise<Uint8Array[]>;
  convertPagesToSvgBlob(svg: SvgInput, options?: SvgPageOptions): Promise<Blob[]>;
};
