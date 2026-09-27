## Usage


```sh
source ./tools/load-env.sh
./tools/create-make.sh "$SVG_FILE"
```

The script splits the SVG into page PDFs with `./bin/main --pdf` and merges
them with `pdfunite`.
