#include "mmap.h"
#include <windows.h>

struct mmap_st {
  HANDLE f;
  HANDLE m;
  const uint8_t *buf;
  size_t len;
};

size_t mmap_size()
{
  return mmap_prepare(NULL, NULL);
}

size_t mmap_prepare(mmap_t *mm, const char *fn)
{
  size_t size = sizeof(struct mmap_st);

  if (mm == NULL) {
    return size;
  }

  memset(mm, 0, size);
  mm->f      = CreateFile(fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (mm->f == INVALID_HANDLE_VALUE) {
    goto err;
  }

  mm->len = GetFileSize(mm->f, NULL);
  if (mm->len == INVALID_FILE_SIZE) {
    goto err2;
  }

  mm->m = CreateFileMapping(mm->f, NULL, PAGE_READONLY, 0, 0, NULL);
  if (mm->m == NULL) {
    goto err3;
  }

  mm->buf = (const unsigned char *)MapViewOfFile(mm->m, FILE_MAP_READ, 0, 0, 0);
  if (mm->buf == NULL) {
    goto err4;
  }

  return size;

err4:
  CloseHandle(mm->m);
err3:
  mm->len = 0;
err2:
  CloseHandle(mm->f);
  mm->f = NULL;
err:
  free(mm);
  return 0;
}

size_t mmap_len(mmap_t *mm)
{
  if (mm == NULL) {
    return 0;
  }
  return mm->len;
}

const uint8_t *mmap_buf(mmap_t *mm)
{
  if (mm == NULL) {
    return NULL;
  }
  return mm->buf;
}

void mmap_finish(mmap_t *mm) {
  if (mm == NULL) {
    return;
  }
  if (mm->m != NULL) {
    CloseHandle(mm->m);
    mm->m = NULL;
  }
  if (mm->f != NULL) {
    CloseHandle(mm->f);
    mm->f = NULL;
  }
  mm->len = 0;
  mm->buf = NULL;
}
