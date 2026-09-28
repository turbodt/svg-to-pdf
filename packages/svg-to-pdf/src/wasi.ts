type WasmInstanceRef = { instance?: WebAssembly.Instance };

const ERRNO_BADF = 8;

export const createWasiShim = (ref: WasmInstanceRef): WebAssembly.Imports["wasi_snapshot_preview1"] => {
  const memory = () => {
    const exported = ref.instance?.exports.memory;
    if (!(exported instanceof WebAssembly.Memory)) {
      throw new Error("@zanny/svg-to-pdf: WASM memory is not available.");
    }
    return exported.buffer;
  };

  const view = () => new DataView(memory());

  return {
    environ_get: () => 0,
    environ_sizes_get: (countPtr: number, sizePtr: number) => {
      const data = view();
      data.setUint32(countPtr, 0, true);
      data.setUint32(sizePtr, 0, true);
      return 0;
    },
    clock_time_get: (_id: number, _precision: bigint, timePtr: number) => {
      view().setBigUint64(timePtr, BigInt(Date.now()) * 1000000n, true);
      return 0;
    },
    fd_close: () => 0,
    fd_fdstat_get: () => ERRNO_BADF,
    fd_prestat_get: () => ERRNO_BADF,
    fd_prestat_dir_name: () => ERRNO_BADF,
    fd_read: (_fd: number, _iovs: number, _iovsLen: number, nreadPtr: number) => {
      view().setUint32(nreadPtr, 0, true);
      return 0;
    },
    fd_seek: (_fd: number, _offset: bigint, _whence: number, newOffsetPtr: number) => {
      view().setBigUint64(newOffsetPtr, 0n, true);
      return 0;
    },
    fd_write: (_fd: number, iovs: number, iovsLen: number, nwrittenPtr: number) => {
      const data = view();
      let written = 0;
      for (let i = 0; i < iovsLen; i++) {
        written += data.getUint32(iovs + i * 8 + 4, true);
      }
      data.setUint32(nwrittenPtr, written, true);
      return 0;
    },
    path_filestat_get: () => ERRNO_BADF,
    path_open: () => ERRNO_BADF,
    proc_exit: (code: number) => {
      throw new Error(`WASI proc_exit(${code})`);
    },
    random_get: (ptr: number, len: number) => {
      const bytes = new Uint8Array(memory(), ptr, len);
      if (globalThis.crypto?.getRandomValues) {
        globalThis.crypto.getRandomValues(bytes);
      } else {
        for (let i = 0; i < len; i++) bytes[i] = (i * 73 + 19) & 255;
      }
      return 0;
    }
  };
};
