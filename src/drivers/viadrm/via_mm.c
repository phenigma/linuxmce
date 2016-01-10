/*
 * Copyright 1998-2005 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2005 S3 Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <linux/module.h>
#include "via.h"
#include "drmP.h"
#include "via_drm.h"
#include "via_drv.h"
#include "via_ds.h"
#include "via_mm.h"

#define MAX_CONTEXT 100

unsigned int VIA_DEBUG = 1;

typedef struct {
  int used;
  int context;
  set_t *sets[2]; /* 0 for frame buffer, 1 for AGP , 2 for System*/
} via_context_t;

static via_context_t global_ppriv[MAX_CONTEXT];

static int add_alloc_set(int context, int type, ITEM_TYPE val)
{
  int i, retval = 0;
  
  for (i = 0; i < MAX_CONTEXT; i++)
 {
    if (global_ppriv[i].used && global_ppriv[i].context == context) {
	retval = setAdd(global_ppriv[i].sets[type], val);
        break;
    }
  }
  
  return retval;
}

static int del_alloc_set(int context, int type, ITEM_TYPE val)
{  
  int i, retval = 0;
  
  for (i = 0; i < MAX_CONTEXT; i++)
    if (global_ppriv[i].used && global_ppriv[i].context == context) {
	retval = setDel(global_ppriv[i].sets[type], val);
        break;
    }
  
  return retval;
}

/* agp memory management */ 
static memHeap_t *AgpHeap = NULL;

int via_agp_init(struct inode *inode, struct file *filp, unsigned int cmd,
		  unsigned long arg)
{
  drm_via_agp_t agp;
  
  if (copy_from_user(&agp, (drm_via_agp_t *)arg, sizeof(agp)))
	  return -EFAULT;

  AgpHeap = mmInit(agp.offset, agp.size);

  DRM_DEBUG("offset = %u, size = %u", agp.offset, agp.size);
  
  return 0;
}

/* fb memory management */ 
static memHeap_t *FBHeap = NULL;

int via_fb_init(struct inode *inode, struct file *filp, unsigned int cmd,
		  unsigned long arg)
{
  drm_via_fb_t fb;

   
  if  (copy_from_user(&fb, (drm_via_fb_t *)arg, sizeof(fb)))
	  return -EFAULT;

  FBHeap = mmInit(fb.offset, fb.size);

  DRM_INFO("offset = %u, size = %u\n", fb.offset, fb.size);
  DRM_INFO("FBHeap = %u\n", FBHeap);

  return 0;
}

int via_init_context(int context)
{
    int i;
    
    for (i = 0; i < MAX_CONTEXT ; i++)
	if (global_ppriv[i].used && (global_ppriv[i].context == context))
	    break;
    
    if (i >= MAX_CONTEXT) {
    	for (i = 0; i < MAX_CONTEXT ; i++) {
	    if (!global_ppriv[i].used) {
		global_ppriv[i].context = context;
	    	global_ppriv[i].used = 1;
	    	global_ppriv[i].sets[0] = setInit();
	    	global_ppriv[i].sets[1] = setInit();
	    	DRM_DEBUG("init allocation set, socket=%d, context = %d\n", 
	                 i, context);
		break;
	    }
	}
	
	if ((i >= MAX_CONTEXT) || (global_ppriv[i].sets[0] == NULL) ||
	     (global_ppriv[i].sets[1] == NULL)) {
	     return 0;
	}
    }
    
    return 1;
}

int via_final_context(int context)
{
    int i;
    for (i=0; i<MAX_CONTEXT; i++)
	if (global_ppriv[i].used && (global_ppriv[i].context == context))
	    break;
    
    if (i < MAX_CONTEXT) {
	set_t *set;
/*=* John Sheng [2004.6.3] For AMD64 *=*/
#ifndef __x86_64__
	unsigned int item;
#else
	unsigned long item;
#endif
	int retval;
	  
  	DRM_DEBUG("find socket %d, context = %d\n", i, context);

	/* Video Memory */
	set = global_ppriv[i].sets[0];
	retval = setFirst(set, &item);
	while (retval) {
	    DRM_DEBUG("free video memory 0x%x\n", item);
	    mmFreeMem((PMemBlock)item);
	    retval = setNext(set, &item);
	}
	setDestroy(set);
	    
	/* AGP Memory */
	set = global_ppriv[i].sets[1];
	retval = setFirst(set, &item);
	while (retval) {
   	    DRM_DEBUG("free agp memory 0x%x\n", item);
	    mmFreeMem((PMemBlock)item);
	    retval = setNext(set, &item);
	}
	setDestroy(set);
	
	global_ppriv[i].used = 0;	  
    }
    
    return 1;
}
int via_mem_alloc(struct inode *inode, struct file *filp, unsigned int cmd,
		  unsigned long arg)
{
    drm_via_mem_t mem;
    if (copy_from_user(&mem, (drm_via_mem_t *)arg, sizeof(mem)))
	return -EFAULT;
    switch (mem.type) {
	case VIDEO :
	    if (via_fb_alloc(&mem) < 0)
		return -EFAULT;
	    if (copy_to_user((drm_via_mem_t *)arg, &mem, sizeof(mem))) 
		return -EFAULT;
	    return 0;
	case AGP :
	    if (via_agp_alloc(&mem) < 0)
		return -EFAULT;
	    if (copy_to_user((drm_via_mem_t *)arg, &mem, sizeof(mem))) 
		return -EFAULT;
	    return 0;
    }

    return -EFAULT;
}
int via_fb_alloc(drm_via_mem_t* mem)
{
    drm_via_mm_t fb;
    PMemBlock block;
    int retval = 0;
   
    if (!FBHeap)
	return -1;

    fb.size = mem->size;
    fb.context = mem->context;
  
    block = mmAllocMem(FBHeap, fb.size, 5, 0);
    if (block) {
	fb.offset = block->ofs;
/*=* John Sheng [2004.6.3] For AMD64 *=*/
#ifndef __x86_64__
	fb.free = (unsigned int)block;
#else
	fb.free = (unsigned long)block;
#endif
	if (!add_alloc_set(fb.context, VIDEO, fb.free)) {
    	    DRM_DEBUG("adding to allocation set fails\n");
    	    mmFreeMem((PMemBlock)fb.free);
    	    retval = -1;
	}
    }
    else {  
	fb.offset = 0;
	fb.size = 0;
	fb.free = 0;
    }
   
    mem->offset = fb.offset;
    mem->index = fb.free;

    DRM_DEBUG("alloc fb, size = %d, offset = %d\n", fb.size, (int)fb.offset);
    DRM_DEBUG("alloc fb, index = %d\n", fb.free);
    return retval;
}
int via_agp_alloc(drm_via_mem_t* mem)
{
    drm_via_mm_t agp;
    PMemBlock block;
    int retval = 0;

    if (!AgpHeap)
	return -1;

    agp.size = mem->size;
    agp.context = mem->context;
  
    block = mmAllocMem(AgpHeap, agp.size, 5, 0);
    if (block) {
	agp.offset = block->ofs;
/*=* John Sheng [2004.6.3] For AMD64 *=*/
#ifndef __x86_64__
	agp.free = (unsigned int)block;
#else
	agp.free = (unsigned long)block;
#endif
	if (!add_alloc_set(agp.context, AGP, agp.free)) {
    	    DRM_DEBUG("adding to allocation set fails\n");
    	    mmFreeMem((PMemBlock)agp.free);
    	     retval = -1;
	}
    }
    else {  
	agp.offset = 0;
	agp.size = 0;
	agp.free = 0;
    }	
   
    mem->offset = agp.offset;
    mem->index = agp.free;

    DRM_DEBUG("alloc agp, size = %d, offset = %d\n", agp.size, (unsigned int)agp.offset);
    return retval;
}
int via_mem_free(struct inode *inode, struct file *filp, unsigned int cmd,
		  unsigned long arg)
{
    drm_via_mem_t mem;
    
    if (copy_from_user(&mem, (drm_via_mem_t *)arg, sizeof(mem)))
	return -EFAULT;
	
    switch (mem.type) {

	case VIDEO :
	    if (via_fb_free(&mem) == 0)
		return 0;
	    break;
	case AGP :
	    if (via_agp_free(&mem) == 0)
		return 0;
	    break;
    }
    
    return -EFAULT;
}
int via_fb_free(drm_via_mem_t* mem)
{
    drm_via_mm_t fb;
    int retval = 0;

    DRM_DEBUG("via_fb_free - in\n");    
    
    if (!FBHeap) {
	return -1;
    }
    DRM_DEBUG("via_fb_free - 1\n");    
    fb.free = mem->index;
    fb.context = mem->context;
    DRM_DEBUG("index = %d, context = %d\n", fb.free, fb.context);
    
    if (!fb.free) {
	return -1;
    }
    DRM_DEBUG("via_fb_free - 2\n");    
    mmFreeMem((PMemBlock)fb.free);
    DRM_DEBUG("via_fb_free - 3\n");    
    if (!del_alloc_set(fb.context, VIDEO, fb.free)) {
	retval = -1;
    }
    
    DRM_DEBUG("free fb, free = %d\n", fb.free);
    
    return retval;
} 
int via_agp_free(drm_via_mem_t* mem)
{
    drm_via_mm_t agp;
  
    int retval = 0;

    agp.free = mem->index;
    agp.context = mem->context;
    
    if (!agp.free)
	return -1;

    mmFreeMem((PMemBlock)agp.free);
    
    if (!del_alloc_set(agp.context, AGP, agp.free)) {
	retval = -1;
    }

    DRM_DEBUG("free agp, free = %d\n", agp.free);
  
    return retval;
}

/* export functions for video driver */
EXPORT_SYMBOL(via_fb_alloc);
EXPORT_SYMBOL(via_fb_free);
