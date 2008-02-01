/*
 *  spcp8x5 USB to serial adaptor driver ring buffer header file
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 * 
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

// 1st in 1st out buffer 4 driver 
struct RingBuf 
{
	unsigned int	buf_size;
	char			*buf_buf;
	char			*buf_get;
	char			*buf_put;
};

// name : alloc_ringbuf
// in	: size - the size of the ring buffer
// out 	: RingBuf* - if success return the ptr,else return NULL
// desc : alloc the ring buf and alloc the buffer itself
static inline struct RingBuf* alloc_ringbuf(unsigned int size)
{
	struct RingBuf* pb;

	if (size == 0)
		return NULL;

	pb = (struct RingBuf*)kmalloc(sizeof(struct RingBuf), GFP_KERNEL);
	if (pb == NULL)
		return NULL;

	pb->buf_buf = kmalloc(size, GFP_KERNEL);
	if (pb->buf_buf == NULL) {
		kfree(pb);
		return NULL;
	}

	pb->buf_size = size;
	pb->buf_get = pb->buf_put = pb->buf_buf;

	return pb;
}

// name : free_ringbuf
// in	: pb - ptr to the ring buf
// out	: NULL
// desc : free the ring buf and the buffer itself
static inline void free_ringbuf(struct RingBuf* pb)
{
	if (pb != NULL) {
		if (pb->buf_buf != NULL)
			kfree(pb->buf_buf);
		kfree(pb);
	}	
}

// name : clear_ringbuf
// in	: pb -  ptr to the ring buffer
// out	: NULL 
// desc : clear pipo , juest repoint the pointer here
static inline void clear_ringbuf(struct RingBuf* pb)
{
	if (pb != NULL)
		pb->buf_get = pb->buf_put;
}

// name : ringbuf_avail_data
// in 	: pb - prt to the ring buffer
// out	: unsigned int - data avail in ring buffer
// desc : get the number of data in the pipo
static inline unsigned int ringbuf_avail_data(struct RingBuf* pb)
{
	if (pb != NULL)
		return ((pb->buf_size + pb->buf_put - pb->buf_get) % pb->buf_size);
	else
		return 0;
}

// name : ringbuf_avail_space
// in 	: pb - ptr to the ring buffer
// out	: unsinged int - space avail in the ring buffer
// desc : get the number of space in the pipo
static inline unsigned int ringbuf_avail_space(struct RingBuf *pb)
{
	if (pb != NULL)
		return ((pb->buf_size + pb->buf_get - pb->buf_put - 1) % pb->buf_size);
	else
		return 0;
}

// name : put_ringbuf
// in	: pb - ptr to the ring buffer
//		: buf - the buf be put into ring buffer
//		: count - the count of buf that be put into ring buffer
// out	: unsigned int - the number of data put into ring buffer
// desc : put count data into pipo
static unsigned int put_ringbuf(
							struct RingBuf *pb, 
							const char *buf,
							unsigned int count)
{
	unsigned int len;

	if (pb == NULL)
		return 0;

	len  = ringbuf_avail_space(pb);
	if (count > len)
		count = len;

	if (count == 0)
		return 0;

	len = pb->buf_buf + pb->buf_size - pb->buf_put;
	if (count > len) {
		memcpy(pb->buf_put, buf, len);
		memcpy(pb->buf_buf, buf+len, count - len);
		pb->buf_put = pb->buf_buf + count - len;
	} else {
		memcpy(pb->buf_put, buf, count);
		if (count < len)
			pb->buf_put += count;
		else /* count == len */
			pb->buf_put = pb->buf_buf;
	}
	return count;
}

// name : get_ringbuf
// in	: pt - ptr to the ringbuf
//		: buf - the buffer be put 
//		: count - request count
// out	: unsigned int - number of data get from ring buffer
// desc : get count data from pipo
static unsigned int get_ringbuf(
							struct RingBuf *pb, 
							char *buf,
							unsigned int count)
{
	unsigned int len;

	if (pb == NULL || buf == NULL)
		return 0;

	len = ringbuf_avail_data(pb);
	if (count > len)
		count = len;

	if (count == 0)
		return 0;

	len = pb->buf_buf + pb->buf_size - pb->buf_get;
	if (count > len) {
		memcpy(buf, pb->buf_get, len);
		memcpy(buf+len, pb->buf_buf, count - len);
		pb->buf_get = pb->buf_buf + count - len;
	} else {
		memcpy(buf, pb->buf_get, count);
		if (count < len)
			pb->buf_get += count;
		else /* count == len */
			pb->buf_get = pb->buf_buf;
	}

	return count;
}


#endif
