#include <stdlib.h>
#include <string.h>
#include "zlibaux.h"
#include <stdio.h>

typedef voidpf (*alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void   (*free_func)(voidpf opaque, voidpf address);
extern uint8_t *wasmalloc(size_t size);

voidpf zalloc(voidpf opaque, uInt items, uInt size)
{
  return wasmalloc(items * size);
}

void zfree(voidpf opaque, voidpf address)
{
}

static void loop_cb(int res, z_stream *z, gz_header *gz, uint8_t *buf, size_t buflen, zlibaux_inflate_args_t *args)
{
  if (z->avail_in == 0 && res != Z_STREAM_END) {
    args->input_cb(&z->next_in, &z->avail_in, args->arg);
  }
  if (z->avail_out == 0 || res == Z_STREAM_END) {
    if (z->total_out > 0) {
      args->output_cb(buf, buflen - z->avail_out, args->arg);
    }
    z->next_out = buf;
    z->avail_out = buflen;
  }
}

zlibaux_inflate_res_t zlibaux_inflate(zlibaux_inflate_args_t *args)
{
  z_stream   z                = { 0 };
  gz_header  gz               = { 0 };
  const int  buflen           = 1024 * 1024;
  uint8_t   *buf              = NULL;
  int        res              = Z_OK;

  z.zfree = zfree;
  z.zalloc = zalloc;
  buf = wasmalloc(buflen);

  loop_cb(res, &z, &gz, buf, buflen, args);

  res = inflateInit2(&z, args->windowBits);
  res = inflateGetHeader(&z, &gz);

  do {
    res = inflate(&z, args->flush);
    if (res == Z_STREAM_END) { continue; }
    if (res == Z_OK) {
      if (z.avail_in == 0 || z.avail_out == 0) {
        loop_cb(res, &z, &gz, buf, buflen, args);
      }
      continue;
    }
    return res;
  } while (res != Z_STREAM_END);
  loop_cb(res, &z, &gz, buf, buflen, args);
  inflateEnd(&z);
  return ZA_STREAM_END;
}

