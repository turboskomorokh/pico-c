#include <stdlib.h>
#include <stdio.h>

#include "xfunc.h"
#include "memfb.h"

memfb_t* mem_fb_new(size_t s)
{
  memfb_t *mfb = xmalloc(sizeof(memfb_t));
  mfb->buf = xcalloc(s, sizeof(char));
  mfb->fp  = xfmemopen(mfb->buf, s, "r+");
  return mfb;
}

void mem_fb_free(memfb_t* mfb)
{
  if(mfb) {
    if(mfb->fp) {
      fflush(mfb->fp);
      fclose(mfb->fp);
    }
    if(mfb->buf)
      xfree(mfb->buf);
    xfree(mfb);
  }
  return;
}
