#ifndef MMAP_H
#define MMAP_H 1

#include <stdint.h>

typedef struct mmap_st mmap_t;

size_t         mmap_size();
size_t         mmap_prepare(mmap_t *mm, const char *fn);
void           mmap_finish(mmap_t *mm);
size_t         mmap_len(mmap_t *mm);
const uint8_t *mmap_buf(mmap_t *mm);

#endif
