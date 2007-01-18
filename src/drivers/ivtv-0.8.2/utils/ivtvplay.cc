/*
 * ivtvplay
 *
 * This is a simple program to play MPEG files in conjunction with the iTVC15
 * driver. It allows seeking to arbitrary GOPs by timestamp and runs a sync
 * thread to show frames as they are played with cycle-accurate timestamps.
 *
 * It is intended as a reference and testing implementation.
 *
 * Copyright (c) 2003 Matt T. Yourst <yourst@yourst.com>
 * This program is licensed under the GNU General Public License, version 2
 */
#define _FILE_OFFSET_BITS 64

#include <unistd.h>
#include <features.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <limits.h>
//#undef _FILE_OFFSET_BITS
//#include <sys/sendfile.h>
//#define _FILE_OFFSET_BITS 64
#include <asm/unistd.h>
#include <linux/unistd.h>
//__NR_sendfile
#include <math.h>
#include <float.h>
#include <sys/user.h>
#include <sys/time.h>
#include <assert.h>
#include <linux/types.h>

#define __user
#include <linux/videodev2.h>
#define IVTV_INTERNAL
#include "ivtv.h"

typedef unsigned long W32;
typedef unsigned long long W64;

static inline W64 rdtsc() {
  W64 t;
  asm volatile("rdtsc" : "=A" (t));
  return t;
}

inline int bit(unsigned int v, unsigned int i) {
  return (v >> i) & 1;
}

typedef union {
  struct { uint32_t padding:5, broken:1, closed:1, frame:6, second:6, padding2:1, minute:6, hour:5, drop:1; };
  struct { uint32_t data; };
} gop_header_t;

struct mpeg_index_entry {
  unsigned int frame;
  gop_header_t timestamp;
  unsigned long long offset;
};

struct mpeg_file {
  struct mpeg_index_entry* index;
  int index_count;
  int mpegfd;
  W64 mpeg_file_size;
  W64 offset;
};

struct ivtv_dec_timing_info {
  int frame;
  int ptslo;
  int ptshi;
  int scrlo;
  int scrhi;
};

#define IVTV_API_DEC_GET_TIMING 0x00000015
#define IVTV_API_DEC_START_PLAYBACK 0x00000001
#define IVTV_API_DEC_STOP_PLAYBACK 0x00000002
#define IVTV_API_DEC_PAUSE_PLAYBACK 0x0000000d

/*
static inline int ivtv_api_dec_get_timing(int ivtvfd, struct ivtv_dec_timing_info* timing) {
  int rc;
  struct ivtv_ioctl_fwapi req;
  req.cmd = IVTV_API_DEC_GET_TIMING;
  req.args = 5;
  if ((rc = ioctl(ivtvfd, IVTV_IOC_FWAPI, &req)) < 0)
    return rc;

  timing->frame = req.data[0];
  timing->ptslo = req.data[1];
  timing->ptshi = req.data[2];
  timing->scrlo = req.data[3];
  timing->scrhi = req.data[4];
  return req.result;
}

static inline int ivtv_api_dec_pause(int ivtvfd, int blackframe) {
  int rc;
  struct ivtv_ioctl_fwapi req;
  req.cmd = IVTV_API_DEC_PAUSE_PLAYBACK;
  req.args = 1;
  req.data[0] = blackframe;
  if ((rc = ioctl(ivtvfd, IVTV_IOC_FWAPI, &req)) < 0)
    return rc;
  return req.result;
}

static inline int ivtv_api_dec_play(int ivtvfd, int firstframe) {
  int rc;
  struct ivtv_ioctl_fwapi req;
  req.cmd = IVTV_API_DEC_START_PLAYBACK;
  req.args = 1;
  req.data[0] = firstframe;
  if ((rc = ioctl(ivtvfd, IVTV_IOC_FWAPI, &req)) < 0)
    return rc;
  return req.result;
}

static inline int ivtv_api_dec_stop(int ivtvfd, int blackframe, int ptslo, int ptshi) {
  int rc;
  struct ivtv_ioctl_fwapi req;
  req.cmd = IVTV_API_DEC_STOP_PLAYBACK;
  req.args = 3;
  req.data[0] = blackframe;
  req.data[1] = ptslo;
  req.data[2] = ptshi;
  if ((rc = ioctl(ivtvfd, IVTV_IOC_FWAPI, &req)) < 0)
    return rc;
  return req.result;
}
*/
char* timestamp_to_string(char* str, gop_header_t timestamp) {
  static char buf[256];
  char* p = (str) ? str : buf;
  snprintf(p, sizeof(buf), "%d:%02d:%02d:%d", timestamp.hour, timestamp.minute, timestamp.second, timestamp.frame);
  return p;
}

gop_header_t pts_to_timestamp(W32 pts, float fps) {
  gop_header_t gop;
  // MPEG "system clock reference" (SCR) and "presentation timestamp" (PTS) run at 90 KHz (90000 ticks per second):

#define MPEG_CLOCK_FREQ 90000
  int seconds = pts / MPEG_CLOCK_FREQ;
  int fracsec = pts % MPEG_CLOCK_FREQ;

  int minutes = seconds / 60;
  seconds = seconds % 60;

  int hours = minutes / 60;
  minutes = minutes % 60;

  gop.hour = hours;
  gop.minute = minutes;
  gop.second = seconds;
  gop.frame = (int)ceilf(((float)fracsec / (float)MPEG_CLOCK_FREQ) * fps);
  return gop;
}

char* pts_to_string(char* str, unsigned long pts) {
  static char buf[256];
  char* p = (str) ? str : buf;

  int seconds = pts / MPEG_CLOCK_FREQ;
  int fracsec = pts % MPEG_CLOCK_FREQ;

  int minutes = seconds / 60;
  seconds = seconds % 60;

  int hours = minutes / 60;
  minutes = minutes % 60;

  float fps = 30;
  int frame = (int)ceilf(((float)fracsec / (float)MPEG_CLOCK_FREQ) * fps);

  snprintf(p, sizeof(buf), "%d:%02d:%02d:%d", hours, minutes, seconds, frame);
  return p;
}

static inline gop_header_t make_timestamp(int hour, int minute, int second, int frame) {
  gop_header_t gop;
  gop.hour = hour;
  gop.minute = minute;
  gop.second = second;
  gop.frame = frame;
  return gop;
}

int mpeg_open(struct mpeg_file* mpeg, char* filename) {

  int mpegfd;
  int indexfd;

  char indexfilename[PATH_MAX];
  snprintf(indexfilename, sizeof(indexfilename), "%s.index", filename);

  if ((mpegfd = open(filename, O_RDONLY | O_LARGEFILE)) < 0) {
    fprintf(stderr, "Failed to open %s: %s\n", filename, strerror(errno));
    return -2;
  }
  mpeg->mpegfd = mpegfd;

  mpeg->mpeg_file_size = lseek64(mpegfd, 0, SEEK_END);
  lseek64(mpegfd, 0, SEEK_SET);

  if ((indexfd = open(indexfilename, O_RDONLY | O_LARGEFILE)) < 0) {
    fprintf(stderr, "Failed to open %s.index: %s\n", filename, strerror(errno));
    return -3;
  }

  W64 index_size = lseek64(indexfd, 0, SEEK_END);
  if (index_size < 0) {
    fprintf(stderr, "Failed to get size of %s.index: %s\n", filename, strerror(errno));
    return -4;
  }

  mpeg->index = (struct mpeg_index_entry*)mmap(NULL, (W32)index_size, PROT_READ, MAP_PRIVATE, indexfd, 0);
  if (!mpeg->index) {
    fprintf(stderr, "Failed to map %s.index: %s\n", filename, strerror(errno));
    return -5;
  }

  close(indexfd);

  mpeg->index_count = (W32)index_size / sizeof(struct mpeg_index_entry);

  printf("Index of %d GOPs:\n", mpeg->index_count);
  for (int i = 0; i < mpeg->index_count; i++) {
    //W64 delta = mpeg->index[i+1].offset - mpeg->index[i].offset;
    //printf("  [% 10d] %-15s (frame % 15d) @ % 20lld (gopsize %d)\n", i, timestamp_to_string(NULL, mpeg->index[i].timestamp), mpeg->index[i].frame, mpeg->index[i].offset, delta);
#define MARK_GOP_START 6
    //printf("INSERT INTO recordedmarkup (chanid, starttime, mark, type, offset) values ('1011', '20030808233000', %d, %d, \"%lld\");\n", (mpeg->index[i].frame / 15), MARK_GOP_START, mpeg->index[i].offset);
    ///video/1011_20030808233000_20030809000000.nuv.index
  }

  mpeg->offset = 0;
  return 0;
}

struct mpeg_index_entry* mpeg_find_nearest(struct mpeg_file* mpeg, const gop_header_t gop) {
  printf("mpeg_find_nearest: gop = %d\n", gop.data);
  int lo = 0;
  int hi = mpeg->index_count-1;
  int i = (hi + lo) / 2;
  for (;;) {
    if (hi == i)
      return &mpeg->index[i];
    i = (lo + hi) / 2;
    if (gop.data > mpeg->index[i].timestamp.data)
      lo = i+1;
    else if (gop.data < mpeg->index[i].timestamp.data) {
      hi = i-1;
    } else {
      return &mpeg->index[i];
    }
  }

  // (should never be reached)
  return NULL;
}

int mpeg_seek_to_time(struct mpeg_file* mpeg, const gop_header_t gop) {
  struct mpeg_index_entry* idx = mpeg_find_nearest(mpeg, gop);
  //printf("Searching for timestamp %d:%02d:%02d:%d: found GOP for timestamp %d:%02d:%02d:%d at file offset %lld.\n",
  //hour, minute, second, frame, idx->timestamp.hour, idx->timestamp.minute, idx->timestamp.second, idx->timestamp.frame, idx->offset);

  mpeg->offset = idx->offset;

  return 0;
}

int writeall(int fd, char* buf, int count) {
  int origcount = count;
  while (count > 0) {
    int n = write(fd, buf, count);
    if (n < 0) return n;
    count -= n;
    buf += n;
  }

  return origcount;
}

int streamfd(int fdout, int fdin, int count) {
  static const int bufsize = 262144;
  char buf[bufsize];

  int remaining = count;
  count = 0;

  while (remaining > 0) {
    int n = (remaining < bufsize) ? remaining : bufsize;
    n = read(fdin, buf, n);
    if (n < 0) return n;
    int k = writeall(fdout, buf, n);
    if (k < 0) return k;
    assert(k == n);
    remaining -= n;
    count += n;
  }

  return count;
}

int mpeg_play(struct mpeg_file* mpeg, int videofd, const gop_header_t start_timestamp, const gop_header_t end_timestamp) {
  static const int bufsize = 65536;

  char* buf = new char[bufsize];

  struct mpeg_index_entry* idx = mpeg_find_nearest(mpeg, start_timestamp);
  struct mpeg_index_entry* end_idx = mpeg_find_nearest(mpeg, end_timestamp);

  printf("mpeg_play: playing %lld bytes from %lld to %lld...\n", (end_idx->offset - idx->offset), idx->offset, end_idx->offset); fflush(stdout);
  printf("Start time: %s (gop %zd)\n", timestamp_to_string(NULL, start_timestamp), (idx - mpeg->index));
  printf("End time: %s (gop %zd)\n", timestamp_to_string(NULL, end_timestamp), (end_idx - mpeg->index));

  /*
  int rc;
  if ((rc = ivtv_api_dec_play(videofd, 0)) < 0) {
    fprintf(stderr, "ivtvplay: Warning: ivtv_api_dec_play() returned %d\n", rc);
  }
  */

  while (idx < end_idx) {
    int chunk_size = ((idx+1)->offset - idx->offset);
    mpeg->offset = idx->offset;

    if (streamfd(videofd, mpeg->mpegfd, chunk_size) < 0) {
      fprintf(stderr, "mpeg_play: error %d (%s)\n", errno, strerror(errno));
      goto done;
    }

    struct ivtv_ioctl_framesync frameinfo;

    if (ioctl(videofd, IVTV_IOC_GET_TIMING, &frameinfo) < 0) {
      fprintf(stderr, "ioctl: IVTV_IOC_FRAMESYNC failed\n");
    } else {
      static const double hz = 1596.027*1000*1000;
      double tframe = ((double)rdtsc()) / hz;

      char ptsstr[64];
      char scrstr[64];
      pts_to_string(ptsstr, frameinfo.pts);
      pts_to_string(scrstr, frameinfo.scr);

      printf("%10.6f: gop %-5zd  bytes %-7d  offset %-12lld  pts %-12s  scr %-12s  frames %d\n", tframe, (idx - mpeg->index), chunk_size, mpeg->offset, ptsstr, scrstr, frameinfo.frame);
    }

    mpeg->offset += chunk_size;

    if (mpeg->offset == idx->offset) // end of file?
      break;
    idx++;
  }
done:
  delete[] buf;
  /*
  if ((rc = ivtv_api_dec_pause(videofd, 0)) < 0) {
    fprintf(stderr, "ivtvplay: Warning: ivtv_api_dec_pause() returned %d\n", rc);
  }
  */
  return 0;
}

//static int accum = 0;

void sync_loop(long videofd) {
  struct ivtv_ioctl_framesync frameinfo;

  printf("sync_loop started on videofd %ld\n", videofd); fflush(stdout);

  //++MTY This applies only to peptidyl:
  static const double hz = 1596.027*1000*1000;

  W64 tprev = rdtsc();
  for (;;) {
    if (ioctl(videofd, IVTV_IOC_FRAMESYNC, &frameinfo) < 0) {
      fprintf(stderr, "ioctl: IVTV_IOC_FRAMESYNC failed\n");
      break;
    } else {
      W64 tframe = rdtsc();
      double tdelta = (tframe - tprev) / hz;
      tprev = tframe;

      char ptsstr[64];
      char scrstr[64];
      pts_to_string(ptsstr, frameinfo.pts);
      pts_to_string(scrstr, frameinfo.scr);

      printf("%10.6f: pts %-20s, scr %-20s, %d frames\n", tdelta, ptsstr, scrstr, frameinfo.frame);
    }
  }
}

void* sync_thread(void* arg) {
  sync_loop((long)arg);
  return NULL;
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    fprintf(stderr, "Syntax is:\n");
    fprintf(stderr, "ivtvplay mpegfile /dev/videoX starthour:minute:second:frame endhour:minute:second:frame\n");
    return -1;
  }

  struct mpeg_file mpeg;
  if (mpeg_open(&mpeg, argv[1]) < 0)
    return -2;

  int videofd;
  if ((videofd = open(argv[2], O_WRONLY | O_CREAT | O_APPEND | O_LARGEFILE, 0644)) < 0) {
    fprintf(stderr, "Failed to open video device %s: %s\n", argv[2], strerror(errno));
    return -3;
  }

  int hour, minute, second, frame;

  if (sscanf(argv[3], "%d:%d:%d:%d", &hour, &minute, &second, &frame) != 4) {
    fprintf(stderr, "Bad start timestamp '%s'\n", argv[3]);
    return -4;
  }
  gop_header_t start_timestamp;
  start_timestamp.data = 0;
  start_timestamp.hour = hour;
  start_timestamp.minute = minute;
  start_timestamp.second = second;
  start_timestamp.frame = frame;

  if (sscanf(argv[4], "%d:%d:%d:%d", &hour, &minute, &second, &frame) != 4) {
    fprintf(stderr, "Bad end timestamp '%s'\n", argv[4]);
    return -4;
  }
  gop_header_t end_timestamp;
  end_timestamp.data = 0;
  end_timestamp.hour = hour;
  end_timestamp.minute = minute;
  end_timestamp.second = second;
  end_timestamp.frame = frame;

  pthread_t sync_thread_tid;
  pthread_create(&sync_thread_tid, NULL, sync_thread, (void*)(long)videofd);
  mpeg_play(&mpeg, videofd, start_timestamp, end_timestamp);

  return 0;
}
