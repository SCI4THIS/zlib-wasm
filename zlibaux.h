#ifndef ZLIBAUX_H
#define ZLIBAUX_H 1

#include <stdint.h>
#include "zlib.h"

typedef void (*zlibaux_inflate_cb)(int res, z_stream *z, gz_header *gz, void *arg);

typedef struct zlibaux_st zlibaux_t;

typedef enum {
  ZA_NO_FLUSH   = Z_NO_FLUSH,
  ZA_SYNC_FLUSH = Z_SYNC_FLUSH,
  ZA_FINISH     = Z_FINISH,
  ZA_BLOCK      = Z_BLOCK,
  ZA_TREES      = Z_TREES,
} zlibaux_flush_t;

typedef enum {
  ZA_OK           = Z_OK,
  ZA_STREAM_ERROR = Z_STREAM_ERROR,
  ZA_DATA_ERROR   = Z_DATA_ERROR,
  ZA_STREAM_END   = Z_STREAM_END,
} zlibaux_inflate_res_t;

typedef struct {
  zlibaux_inflate_cb  cb;
  zlibaux_flush_t     flush;
  int                 windowBits;
  void               *arg;
} zlibaux_inflate_args_t;

zlibaux_inflate_res_t zlibaux_inflate(zlibaux_inflate_args_t *args);

#endif
