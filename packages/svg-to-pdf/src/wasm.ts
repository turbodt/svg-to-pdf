import { pdfBytesToBlob, svgBytesToBlob } from "./blob";
import { normalizeOptions, type WasmOptions } from "./options";
import type { SvgInput, SvgPageOptions, SvgToPdfConverter, SvgToPdfOptions, WasmSource } from "./types";
import { createWasiShim } from "./wasi";

type WasmExports = {
  memory: WebAssembly.Memory;
  svg_to_pdf_alloc(len: number): number;
  svg_to_pdf_free(ptr: number): void;
  svg_to_pdf_from_memory(svgPtr: number, svgLen: number, optionsPtr: number, outLenPtr: number): number;
  svg_to_pdf_page_svg_from_memory(svgPtr: number, svgLen: number, pageNumber: number, optionsPtr: number, outLenPtr: number): number;
  svg_to_pdf_page_svgs_from_memory(svgPtr: number, svgLen: number, optionsPtr: number, outLenPtr: number): number;
};

const textEncoder = new TextEncoder();

const inputToBytes = async (input: SvgInput): Promise<Uint8Array> => {
  if (typeof input === "string") return textEncoder.encode(input);
  if (ArrayBuffer.isView(input)) return new Uint8Array(input.buffer, input.byteOffset, input.byteLength);
  if (input instanceof ArrayBuffer) return new Uint8Array(input);
  return new Uint8Array(await input.arrayBuffer());
};

const toArrayBuffer = (bytes: ArrayBuffer | Uint8Array): ArrayBuffer => {
  if (bytes instanceof ArrayBuffer) return bytes;
  return bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength) as ArrayBuffer;
};

const getBytes = async (source: WasmSource): Promise<ArrayBuffer> => {
  if (source.wasmBytes) return toArrayBuffer(source.wasmBytes);
  const response = await fetch(source.wasmUrl);
  if (!response.ok) {
    throw new Error(`@zanny/svg-to-pdf: failed to fetch WASM from ${source.wasmUrl}.`);
  }
  return response.arrayBuffer();
};

const readU32 = (bytes: Uint8Array, offset: number): number =>
  bytes[offset]
  | (bytes[offset + 1] << 8)
  | (bytes[offset + 2] << 16)
  | (bytes[offset + 3] << 24);

export const createSvgToPdfConverter = async (source: WasmSource): Promise<SvgToPdfConverter> => {
  const ref: { instance?: WebAssembly.Instance } = {};
  const module = await WebAssembly.compile(await getBytes(source));
  const instance = await WebAssembly.instantiate(module, {
    wasi_snapshot_preview1: createWasiShim(ref)
  });
  ref.instance = instance;
  const api = instance.exports as unknown as WasmExports;

  const writeBytes = (bytes: Uint8Array): number => {
    const ptr = api.svg_to_pdf_alloc(bytes.length);
    new Uint8Array(api.memory.buffer).set(bytes, ptr);
    return ptr;
  };

  const writeOptions = (options: WasmOptions): number => {
    const ptr = api.svg_to_pdf_alloc(48);
    const view = new DataView(api.memory.buffer);
    view.setFloat64(ptr, options.containerWidth, true);
    view.setFloat64(ptr + 8, options.containerHeight, true);
    view.setInt32(ptr + 16, options.includeContainers ? 1 : 0, true);
    view.setInt32(ptr + 20, options.mergeDuplicatedContainers ? 1 : 0, true);
    view.setInt32(ptr + 24, options.hasPdfSize ? 1 : 0, true);
    view.setFloat64(ptr + 32, options.pdfWidth, true);
    view.setFloat64(ptr + 40, options.pdfHeight, true);
    return ptr;
  };

  const callWithSvg = async (
    svg: SvgInput,
    options: SvgToPdfOptions | SvgPageOptions | undefined,
    fn: (svgPtr: number, svgLen: number, optionsPtr: number, outLenPtr: number) => number
  ): Promise<Uint8Array> => {
    const svgBytes = await inputToBytes(svg);
    const svgPtr = writeBytes(svgBytes);
    const optionsPtr = writeOptions(normalizeOptions(options));
    const outLenPtr = api.svg_to_pdf_alloc(4);
    const outPtr = fn(svgPtr, svgBytes.length, optionsPtr, outLenPtr);
    const outLen = new DataView(api.memory.buffer).getUint32(outLenPtr, true);
    const out = outPtr ? new Uint8Array(api.memory.buffer, outPtr, outLen).slice() : new Uint8Array();
    api.svg_to_pdf_free(svgPtr);
    api.svg_to_pdf_free(optionsPtr);
    api.svg_to_pdf_free(outLenPtr);
    if (outPtr) api.svg_to_pdf_free(outPtr);
    return out;
  };

  const convertToPdf = (svg: SvgInput, options?: SvgToPdfOptions): Promise<Uint8Array> =>
    callWithSvg(svg, options, (svgPtr, svgLen, optionsPtr, outLenPtr) =>
      api.svg_to_pdf_from_memory(svgPtr, svgLen, optionsPtr, outLenPtr)
    );

  const convertPageToSvg = (svg: SvgInput, pageNumber: number, options?: SvgPageOptions): Promise<Uint8Array> => {
    if (!Number.isInteger(pageNumber) || pageNumber < 1) {
      throw new Error("@zanny/svg-to-pdf: pageNumber must be a 1-based positive integer.");
    }
    return callWithSvg(svg, options, (svgPtr, svgLen, optionsPtr, outLenPtr) =>
      api.svg_to_pdf_page_svg_from_memory(svgPtr, svgLen, pageNumber, optionsPtr, outLenPtr)
    );
  };

  const convertPagesToSvg = async (svg: SvgInput, options?: SvgPageOptions): Promise<Uint8Array[]> => {
    const packed = await callWithSvg(svg, options, (svgPtr, svgLen, optionsPtr, outLenPtr) =>
      api.svg_to_pdf_page_svgs_from_memory(svgPtr, svgLen, optionsPtr, outLenPtr)
    );
    if (packed.length < 4) return [];
    const pageCount = readU32(packed, 0);
    const pages: Uint8Array[] = [];
    let offset = 4;
    for (let i = 0; i < pageCount; i++) {
      const len = readU32(packed, offset);
      offset += 4;
      pages.push(packed.slice(offset, offset + len));
      offset += len;
    }
    return pages;
  };

  return {
    convertToPdf,
    convertToPdfBlob: async (svg, options) => pdfBytesToBlob(await convertToPdf(svg, options)),
    convertPageToSvg,
    convertPageToSvgBlob: async (svg, pageNumber, options) => svgBytesToBlob(await convertPageToSvg(svg, pageNumber, options)),
    convertPagesToSvg,
    convertPagesToSvgBlob: async (svg, options) => (await convertPagesToSvg(svg, options)).map(svgBytesToBlob)
  };
};
