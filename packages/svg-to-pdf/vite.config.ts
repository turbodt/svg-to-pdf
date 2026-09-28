import { defineConfig } from "vite";
import dts from "vite-plugin-dts";

export default defineConfig({
  plugins: [dts({ include: ["src"], insertTypesEntry: true })],
  build: {
    lib: {
      entry: "src/index.ts",
      name: "SvgToPdf",
      fileName: "svg-to-pdf",
      formats: ["es"]
    },
    sourcemap: true,
    target: "es2022"
  },
  test: {
    environment: "node"
  }
});
