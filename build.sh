mkdir -p build

for FILE in adler32.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c zutil.c
do
  gcc -Izlib -c zlib/${FILE} -o build/${FILE}.o
done

gcc -Izlib -c zlibaux.c -o build/zlibaux.o
gcc -c mmap.c -o build/mmap.o
gcc -Izlib -c main.c -o build/main.o
gcc -o main build/*.o
