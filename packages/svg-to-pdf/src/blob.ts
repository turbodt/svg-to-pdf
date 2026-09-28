export const svgBytesToBlob = (svg: Uint8Array): Blob =>
  new Blob([new Uint8Array(svg).buffer], { type: "image/svg+xml;charset=utf-8" });

export const svgTextToBlob = (svg: string): Blob =>
  new Blob([svg], { type: "image/svg+xml;charset=utf-8" });

export const pdfBytesToBlob = (pdf: Uint8Array): Blob =>
  new Blob([new Uint8Array(pdf).buffer], { type: "application/pdf" });

export const blobToBytes = async (blob: Blob): Promise<Uint8Array> =>
  new Uint8Array(await blob.arrayBuffer());
