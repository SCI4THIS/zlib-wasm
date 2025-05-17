mkdir -p build

rm build/*

INC_EMCC=/home/A/emsdk/upstream/emscripten/cache/sysroot/include
WASM_LD=/home/A/emsdk/upstream/bin/wasm-ld.exe
WASM2WAT=/home/A/wabt/bin/wasm2wat.exe
WAT2WASM=/home/A/wabt/bin/wat2wasm.exe

for FILE in adler32.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c zutil.c
do
  clang -o build/${FILE}.ll -I${INC_EMCC} --target=wasm32 -emit-llvm -S zlib/${FILE}
done

for FILE in zlibaux.c
do
  clang -o build/${FILE}.ll -I${INC_EMCC} -Izlib --target=wasm32 -emit-llvm -S ${FILE}
done

for FILE in `ls build/*\.ll`
do
  llc -o ${FILE}.o -march=wasm32 -filetype=obj ${FILE}
done

FINAL_NAME=zlibaux

$WASM_LD --no-entry --import-undefined --export-all -o build/${FINAL_NAME}-temp.wasm `ls build/*\.ll\.o`
$WASM2WAT build/${FINAL_NAME}-temp.wasm > build/${FINAL_NAME}.wat
$WAT2WASM -o build/${FINAL_NAME}.wasm build/${FINAL_NAME}.wat
