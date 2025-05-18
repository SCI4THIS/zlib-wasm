#include <stdint.h>
#include <stdlib.h>
#include "zlibaux.h"

typedef struct {
    int       i;
    uint8_t  *buf;
    size_t    buflen;
    uint8_t  *shared_buf;
} args_t;

extern uInt input_chunk(uint8_t *buf, int buflen);
extern void output_chunk(z_const Bytef *buf, int buflen);
extern uint8_t *wasmalloc(size_t size);

static void input_cb(z_const Bytef **bufptr, uInt *lenptr, void *argv)
{
  args_t *arg = argv;
  uInt len = input_chunk(arg->buf, arg->buflen);
  (*bufptr) = (z_const Bytef *)arg->buf;
  (*lenptr) = len;
}


static void output_cb(z_const Bytef *buf, uInt len, void *argv)
{
  output_chunk(buf, len);
}

int zlibaux_wasm_main()
{
  args_t                  args             = { 0 };
  zlibaux_inflate_res_t   res              = ZA_OK;
  zlibaux_inflate_args_t  za_inflate_args  = { ZA_BLOCK, 15 | 16, input_cb, output_cb, &args };
  size_t                  buflen           = 1024 * 1024;
  uint8_t                *buf              = NULL;

  args.buf = wasmalloc(buflen);
  args.buflen     = buflen;

  res = zlibaux_inflate(&za_inflate_args);

  return 0;
}
