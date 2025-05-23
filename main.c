#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include "zlibaux.h"
#include "mmap.h"

typedef struct {
    int       i;
    int       argc;
    char    **argv;
    mmap_t   *mm;
} args_t;

#define ORNULL(s) (s != NULL ? (const char *)s : "NULL")

void print_gz_header(gz_header *gz) {
  printf("gz->text: %d\n", gz->text);
  printf("gz->time: %d\n", gz->time);
  printf("gz->xflags: %d\n", gz->xflags);
  printf("gz->os: %d\n", gz->os);
  printf("gz->extra: %p\n", gz->extra);
  printf("gz->extra_len: %d\n", gz->extra_len);
  printf("gz->extra_max: %d\n", gz->extra_max);
  printf("gz->name: %s\n", ORNULL(gz->name));
  printf("gz->name_max: %d\n", gz->name_max);
  printf("gz->comment: %s\n", ORNULL(gz->comment));
  printf("gz->comm_max: %d\n", gz->comm_max);
  printf("gz->hcrc: %d\n", gz->hcrc);
  printf("gz->done: %d\n", gz->done);
}

void print_z_stream(z_stream *z)
{
  printf("z->next_in: %p\n", z->next_in);
  printf("z->avail_in: %d\n", z->avail_in);
  printf("z->total_in: %d\n", z->total_in);
  printf("z->next_out: %p\n", z->next_out);
  printf("z->avail_out: %d\n", z->avail_out);
  printf("z->total_out: %d\n", z->total_out);
}

size_t fwrite_all(z_const Bytef *ptr, size_t len, FILE *stream)
{
  int i = 0;
  int n = 0;
  while (i = fwrite(&ptr[n], 1, len - n, stream)) {
    n += i;
  } 
  return n;
}

void input_cb(z_const Bytef **bufptr, uInt *lenptr, void *arg)
{
  args_t *args = arg;
  mmap_finish(args->mm);
  mmap_prepare(args->mm, args->argv[args->i]);
  (*bufptr) = (z_const Bytef *)mmap_buf(args->mm);
  (*lenptr) = mmap_len(args->mm);
  (args->i)++;
}

void output_cb(z_const Bytef *buf, uInt len, void *arg)
{
  fwrite_all(buf, len, stdout);
}

int main(int argc, char **argv)
{
  mmap_t                 *mm              = calloc(1, mmap_size());
  args_t                  args            = { 1, argc, argv, mm };
  size_t                  size            = 0;
  zlibaux_inflate_res_t   res             = ZA_OK;
  zlibaux_inflate_args_t  za_inflate_args = { ZA_BLOCK, 15 | 16, input_cb, output_cb, &args };

  if (argc < 2) {
    fprintf(stderr, "usage: %s <file_1> ... <file_n>\n", argv[0]);
    return 0;
  }

  _setmode(_fileno(stdout), O_BINARY);
  res = zlibaux_inflate(&za_inflate_args);
  fflush(stdout);
  mmap_finish(mm);
  free(mm);
  return 0;
}
