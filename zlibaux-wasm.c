#include <stdint.h>
#include <stdlib.h>
#include "zlibaux.h"

typedef struct {
    int       i;
    uint8_t  *buf;
    size_t    buflen;
} args_t;

extern uInt input_chunk(uint8_t *buf, int buflen);
extern void output_chunk(z_const Bytef *buf, int buflen);

static void input_cb(z_const Bytef **bufptr, uInt *lenptr, void *arg)
{
  args_t *args = arg;
  uInt len = input_chunk(args->buf, args->buflen);
  (*bufptr) = (z_const Bytef *)args->buf;
  (*lenptr) = len;
}

static void output_cb(z_const Bytef *buf, uInt len, void *argv)
{
  output_chunk(buf, len);
}

int zlibaux_wasm_main()
{
  args_t                  args            = { 0 };
  zlibaux_inflate_res_t   res             = ZA_OK;
  zlibaux_inflate_args_t  za_inflate_args = { ZA_BLOCK, 15 | 16, input_cb, output_cb, &args };

  args.buflen = 1024 * 1024;
  args.buf = calloc(1, args.buflen);

  res = zlibaux_inflate(&za_inflate_args);

  free(args.buf);
  return 0;
}
