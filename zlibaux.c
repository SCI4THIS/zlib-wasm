#include <stdlib.h>
#include <string.h>
#include "zlibaux.h"
#include <stdio.h>

zlibaux_inflate_res_t zlibaux_inflate(zlibaux_inflate_args_t *args)
{
  z_stream  z   = { 0 };
  gz_header gz  = { 0 };

  int      res = Z_OK;

  z.zfree = Z_NULL;
  z.zalloc = Z_NULL;

  args->cb(res, &z, &gz, args->arg);

  res = inflateInit2(&z, args->windowBits);
  res = inflateGetHeader(&z, &gz);

  do {
    res = inflate(&z, args->flush);
    switch (res) {
      case Z_STREAM_ERROR: return ZA_STREAM_ERROR;
      case Z_DATA_ERROR: return ZA_DATA_ERROR;
      case Z_STREAM_END: continue;
      default: break;
    }
    if (z.avail_in == 0 || z.avail_out == 0) {
      args->cb(res, &z, &gz, args->arg);
    }
  } while (res != Z_STREAM_END);
  args->cb(res, &z, &gz, args->arg);

  inflateEnd(&z);
  return ZA_STREAM_END;
}

