/*
 * native Matroska demuxer
 * Written by Aurelien Jacobs <aurel@gnuage.org>
 * Based on the one written by Ronald Bultje for gstreamer
 *   and on demux_mkv.cpp from Moritz Bunkus.
 * Licence: GPL
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "ebml.h"
#include "matroska.h"

#include "mp_msg.h"
#include "help_mp.h"

#include "vobsub.h"
#include "subreader.h"
#include "libvo/sub.h"

#include "libass/ass.h"
#include "libass/ass_mp.h"

#include "libavutil/common.h"

#ifdef USE_QTX_CODECS
#include "loader/qtx/qtxsdk/components.h"
#endif

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include "libavutil/lzo.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/avstring.h"

static unsigned char sipr_swaps[38][2]={
    {0,63},{1,22},{2,44},{3,90},{5,81},{7,31},{8,86},{9,58},{10,36},{12,68},
    {13,39},{14,73},{15,53},{16,69},{17,57},{19,88},{20,34},{21,71},{24,46},
    {25,94},{26,54},{28,75},{29,50},{32,70},{33,92},{35,74},{38,85},{40,56},
    {42,87},{43,65},{45,59},{48,79},{49,93},{51,89},{55,95},{61,76},{67,83},
    {77,80} };

// Map flavour to bytes per second
#define SIPR_FLAVORS 4
#define ATRC_FLAVORS 8
#define COOK_FLAVORS 34
static int sipr_fl2bps[SIPR_FLAVORS] = {813, 1062, 625, 2000};
static int atrc_fl2bps[ATRC_FLAVORS] = {8269, 11714, 13092, 16538, 18260, 22050, 33075, 44100};
static int cook_fl2bps[COOK_FLAVORS] = {1000, 1378, 2024, 2584, 4005, 5513, 8010, 4005, 750, 2498,
                                        4048, 5513, 8010, 11973, 8010, 2584, 4005, 2067, 2584, 2584,
                                        4005, 4005, 5513, 5513, 8010, 12059, 1550, 8010, 12059, 5513,
                                        12016, 16408, 22911, 33506};

typedef struct
{
  uint32_t order, type, scope;
  uint32_t comp_algo;
  uint8_t *comp_settings;
  int comp_settings_len;
} mkv_content_encoding_t;

typedef struct mkv_track
{
  int tnum;
  char *name;
  
  char *codec_id;
  int ms_compat;
  char *language;

  int type;
  
  uint32_t v_width, v_height, v_dwidth, v_dheight;
  float v_frate;

  uint32_t a_formattag;
  uint32_t a_channels, a_bps;
  float a_sfreq;

  float default_duration;

  int default_track;

  void *private_data;
  unsigned int private_size;

  /* stuff for realmedia */
  int realmedia;
  int rv_kf_base, rv_kf_pts;
  float rv_pts;  /* previous video timestamp */
  float ra_pts;  /* previous audio timestamp */

  /** realaudio descrambling */
  int sub_packet_size; ///< sub packet size, per stream
  int sub_packet_h; ///< number of coded frames per block
  int coded_framesize; ///< coded frame size, per stream
  int audiopk_size; ///< audio packet size
  unsigned char *audio_buf; ///< place to store reordered audio data
  float *audio_timestamp; ///< timestamp for each audio packet
  int sub_packet_cnt; ///< number of subpacket already received
  int audio_filepos; ///< file position of first audio packet in block

  /* stuff for quicktime */
  int fix_i_bps;
  float qt_last_a_pts;

  int subtitle_type;

  /* The timecodes of video frames might have to be reordered if they're
     in display order (the timecodes, not the frames themselves!). In this
     case demux packets have to be cached with the help of these variables. */
  int reorder_timecodes;
  demux_packet_t **cached_dps;
  int num_cached_dps, num_allocated_dps;
  float max_pts;

  /* generic content encoding support */
  mkv_content_encoding_t *encodings;
  int num_encodings;

  /* For VobSubs and SSA/ASS */
  sh_sub_t *sh_sub;
} mkv_track_t;

typedef struct mkv_index
{
  int tnum;
  uint64_t timecode, filepos;
} mkv_index_t;

typedef struct mkv_demuxer
{
  off_t segment_start;

  float duration, last_pts;
  uint64_t last_filepos;

  mkv_track_t **tracks;
  int num_tracks;

  uint64_t tc_scale, cluster_tc, first_tc;
  int has_first_tc;

  uint64_t cluster_size;
  uint64_t blockgroup_size;

  mkv_index_t *indexes;
  int num_indexes;

  off_t *parsed_cues;
  int parsed_cues_num;
  off_t *parsed_seekhead;
  int parsed_seekhead_num;

  uint64_t *cluster_positions;
  int num_cluster_pos;

  int64_t skip_to_timecode;
  int v_skip_to_keyframe, a_skip_to_keyframe;

  int64_t stop_timecode;
  
  int last_aid;
  int audio_tracks[MAX_A_STREAMS];
} mkv_demuxer_t;

#define REALHEADER_SIZE    16
#define RVPROPERTIES_SIZE  34
#define RAPROPERTIES4_SIZE 56
#define RAPROPERTIES5_SIZE 70

/* for e.g. "-slang ger" */
extern char *dvdsub_lang;
extern char *audio_lang;
extern int dvdsub_id;

/**
 * \brief ensures there is space for at least one additional element
 * \param array array to grow
 * \param nelem current number of elements in array
 * \param elsize size of one array element
 */
static void grow_array(void **array, int nelem, size_t elsize) {
  if (!(nelem & 31))
    *array = realloc(*array, (nelem + 32) * elsize);
}

static mkv_track_t *
demux_mkv_find_track_by_num (mkv_demuxer_t *d, int n, int type)
{
  int i, id;

  for (i=0, id=0; i < d->num_tracks; i++)
    if (d->tracks[i] != NULL && d->tracks[i]->type == type)
      if (id++ == n)
        return d->tracks[i];
  
  return NULL;
}

static void
add_cluster_position (mkv_demuxer_t *mkv_d, uint64_t position)
{
  int i = mkv_d->num_cluster_pos;

  while (i--)
    if (mkv_d->cluster_positions[i] == position)
      return;

  grow_array(&mkv_d->cluster_positions, mkv_d->num_cluster_pos,
             sizeof(uint64_t));
  mkv_d->cluster_positions[mkv_d->num_cluster_pos++] = position;
}


#define AAC_SYNC_EXTENSION_TYPE 0x02b7
static int
aac_get_sample_rate_index (uint32_t sample_rate)
{
  if (92017 <= sample_rate)
    return 0;
  else if (75132 <= sample_rate)
    return 1;
  else if (55426 <= sample_rate)
    return 2;
  else if (46009 <= sample_rate)
    return 3;
  else if (37566 <= sample_rate)
    return 4;
  else if (27713 <= sample_rate)
    return 5;
  else if (23004 <= sample_rate)
    return 6;
  else if (18783 <= sample_rate)
    return 7;
  else if (13856 <= sample_rate)
    return 8;
  else if (11502 <= sample_rate)
    return 9;
  else if (9391 <= sample_rate)
    return 10;
  else
    return 11;
}


static int
vobsub_parse_size (sh_sub_t *sh, const char *start)
{
  if (sscanf(&start[6], "%dx%d", &sh->width, &sh->height) == 2)
    {
      mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] VobSub size: %ux%u\n",
             sh->width, sh->height);
      return 1;
    }
  return 0;
}

static int
vobsub_parse_palette (sh_sub_t *sh, const char *start)
{
  int i;

  start += 8;
  while (isspace(*start))
    start++;
  for (i = 0; i < 16; i++)
    {
      unsigned int tmp;
      if (sscanf(start, "%06x", &tmp) != 1)
        break;
      sh->palette[i] = vobsub_palette_to_yuv(tmp);
      start += 6;
      while ((*start == ',') || isspace(*start))
        start++;
    }
  if (i == 16)
    {
      mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] VobSub palette: %06x,%06x,"
             "%06x,%06x,%06x,%06x,%06x,%06x,%06x,%06x,%06x,%06x,%06x,"
             "%06x,%06x,%06x\n", sh->palette[0],
             sh->palette[1], sh->palette[2],
             sh->palette[3], sh->palette[4],
             sh->palette[5], sh->palette[6],
             sh->palette[7], sh->palette[8],
             sh->palette[9], sh->palette[10],
             sh->palette[11], sh->palette[12],
             sh->palette[13], sh->palette[14],
             sh->palette[15]);
      sh->has_palette = 1;
      return 2;
    }
  return 0;
}

static int
vobsub_parse_custom_colors (sh_sub_t *sh, const char *start)
{
  int use_custom_colors, i;
  const char *p;
  unsigned int tridx = 0;

  use_custom_colors = 0;
  start += 14;
  while (isspace(*start))
    start++;
   if (!strncasecmp(start, "ON", 2) || (*start == '1'))
     use_custom_colors = 1;
   else if (!strncasecmp(start, "OFF", 3) || (*start == '0'))
     use_custom_colors = 0;
   mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] VobSub custom colors: %s\n",
          use_custom_colors ? "ON" : "OFF");
   if ((p = strstr(start, "tridx:")) != NULL)
     tridx = strtoul(p + 6, NULL, 2);
   if ((start = strstr(start, "colors:")) != NULL)
     {
       start += 7;
       while (isspace(*start))
         start++;
       for (i = 0; i < 4; i++)
         {
           unsigned int tmp;
           if (sscanf(start, "%06x", &tmp) != 1)
             break;
           sh->colors[i] = vobsub_rgb_to_yuv(tmp);
           if ((tridx << i) & 0x08)
             sh->colors[i] |= 1 << 31;
           start += 6;
           while ((*start == ',') || isspace(*start))
             start++;
         }
       if (i == 4)
         {
           sh->custom_colors = 4;
           mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] VobSub colors: %08x,"
                  "%08x,%08x,%08x\n", sh->colors[0],
                  sh->colors[1], sh->colors[2],
                  sh->colors[3]);
         }
     }
   if (!use_custom_colors)
     sh->custom_colors = 0;
   return 4;
}

static int
vobsub_parse_forced_subs (sh_sub_t *sh, const char *start)
{
  start += 12;
  while (isspace(*start))
    start++;
  if (!strncasecmp(start, "on", 2) || (*start == '1'))
    sh->forced_subs_only = 1;
  else if (!strncasecmp(start, "off", 3) || (*start == '0'))
    sh->forced_subs_only = 0;
  else
    return 0;
  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] VobSub forced subs: %d\n",
         sh->forced_subs_only);
  return 8;
}

/** \brief Free cached demux packets
 *
 * Reordering the timecodes requires caching of demux packets. This function
 * frees all these cached packets and the memory for the cached pointers
 * itself.
 *
 * \param demuxer The demuxer for which the cache is to be freed.
 */
static void
free_cached_dps (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  mkv_track_t *track;
  int i, k;

  for (k = 0; k < mkv_d->num_tracks; k++)
    {
      track = mkv_d->tracks[k];
      for (i = 0; i < track->num_cached_dps; i++)
        free_demux_packet (track->cached_dps[i]);
      free(track->cached_dps);
      track->cached_dps = NULL;
      track->num_cached_dps = 0;
      track->num_allocated_dps = 0;
      track->max_pts = 0;
    }
}

static int
demux_mkv_parse_idx (mkv_track_t *t)
{
  int things_found, last;
  char *buf, *pos, *start;

  if ((t->private_data == NULL) || (t->private_size == 0))
    return 0;

  things_found = 0;
  buf = malloc(t->private_size + 1);
  if (buf == NULL)
    return 0;
  memcpy(buf, t->private_data, t->private_size);
  buf[t->private_size] = 0;
  t->sh_sub->has_palette = 0;

  pos = buf;
  start = buf;
  last = 0;
  do
    {
      if ((*pos == 0) || (*pos == '\r') || (*pos == '\n'))
        {
          if (*pos == 0)
            last = 1;
          *pos = 0;

          if (!strncasecmp(start, "size: ", 6))
            things_found |= vobsub_parse_size(t->sh_sub, start);
          else if (!strncasecmp(start, "palette:", 8))
            things_found |= vobsub_parse_palette(t->sh_sub, start);
          else if (!strncasecmp(start, "custom colors:", 14))
            things_found |= vobsub_parse_custom_colors(t->sh_sub, start);
          else if (!strncasecmp(start, "forced subs:", 12))
            things_found |= vobsub_parse_forced_subs(t->sh_sub, start);

          if (last)
            break;
          do
            {
              pos++;
            }
          while ((*pos == '\r') || (*pos == '\n'));
          start = pos;
        }
      else
        pos++;
    }
  while (!last && (*start != 0));

  free(buf);

  return (things_found & 3) == 3;
}


static int
demux_mkv_decode (mkv_track_t *track, uint8_t *src, uint8_t **dest,
                  uint32_t *size, uint32_t type)
{
  int i, result;
  int modified = 0;

  *dest = src;
  if (track->num_encodings <= 0)
    return 0;

  for (i=0; i<track->num_encodings; i++)
    {
      if (!(track->encodings[i].scope & type))
        continue;

#ifdef HAVE_ZLIB
      if (track->encodings[i].comp_algo == 0)
        {
          /* zlib encoded track */
          z_stream zstream;

          zstream.zalloc = (alloc_func) 0;
          zstream.zfree = (free_func) 0;
          zstream.opaque = (voidpf) 0;
          if (inflateInit (&zstream) != Z_OK)
            {
              mp_msg (MSGT_DEMUX, MSGL_WARN,
                      MSGTR_MPDEMUX_MKV_ZlibInitializationFailed);
              return modified;
            }
          zstream.next_in = (Bytef *) src;
          zstream.avail_in = *size;

          modified = 1;
          *dest = NULL;
          zstream.avail_out = *size;
          do {
            *size += 4000;
            *dest = realloc (*dest, *size);
            zstream.next_out = (Bytef *) (*dest + zstream.total_out);
            result = inflate (&zstream, Z_NO_FLUSH);
            if (result != Z_OK && result != Z_STREAM_END)
              {
                mp_msg (MSGT_DEMUX, MSGL_WARN,
                        MSGTR_MPDEMUX_MKV_ZlibDecompressionFailed);
                free(*dest);
                *dest = NULL;
                inflateEnd (&zstream);
                return modified;
              }
            zstream.avail_out += 4000;
          } while (zstream.avail_out == 4000 &&
                   zstream.avail_in != 0 && result != Z_STREAM_END);

          *size = zstream.total_out;
          inflateEnd (&zstream);
        }
#endif
      if (track->encodings[i].comp_algo == 2)
        {
          /* lzo encoded track */
          int dstlen = *size * 3;

          *dest = NULL;
          while (1)
            {
              int srclen = *size;
              if (dstlen > SIZE_MAX - LZO_OUTPUT_PADDING) goto lzo_fail;
              *dest = realloc (*dest, dstlen + LZO_OUTPUT_PADDING);
              result = lzo1x_decode (*dest, &dstlen, src, &srclen);
              if (result == 0)
                break;
              if (!(result & LZO_OUTPUT_FULL))
                {
lzo_fail:
                  mp_msg (MSGT_DEMUX, MSGL_WARN,
                          MSGTR_MPDEMUX_MKV_LzoDecompressionFailed);
                  free(*dest);
                  *dest = NULL;
                  return modified;
                }
              mp_msg (MSGT_DEMUX, MSGL_DBG2,
                      "[mkv] lzo decompression buffer too small.\n");
              dstlen *= 2;
            }
          *size = dstlen;
        }
    }

  return modified;
}


static int
demux_mkv_read_info (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;
  uint64_t tc_scale = 1000000;
  long double duration = 0.;

  length = ebml_read_length (s, NULL);
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TIMECODESCALE:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 1;
            tc_scale = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + timecode scale: %"PRIu64"\n",
                    tc_scale);
            break;
          }

        case MATROSKA_ID_DURATION:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 1;
            duration = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + duration: %.3Lfs\n",
                    duration * tc_scale / 1000000000.0);
            break;
          }

        default:
          ebml_read_skip (s, &l); 
          break;
        }
      length -= l + il;
    }
  mkv_d->tc_scale = tc_scale;
  mkv_d->duration = duration * tc_scale / 1000000000.0;
  return 0;
}

/**
 * \brief free array of kv_content_encoding_t
 * \param encodings pointer to array
 * \param numencodings number of encodings in array
 */
static void
demux_mkv_free_encodings(mkv_content_encoding_t *encodings, int numencodings)
{
  while (numencodings-- > 0)
    free(encodings[numencodings].comp_settings);
  free(encodings);
}

static int
demux_mkv_read_trackencodings (demuxer_t *demuxer, mkv_track_t *track)
{
  stream_t *s = demuxer->stream;
  mkv_content_encoding_t *ce, e;
  uint64_t len, length, l;
  int il, n;

  ce = malloc (sizeof (*ce));
  n = 0;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_CONTENTENCODING:
          {
            uint64_t len;
            int i;

            memset (&e, 0, sizeof (e));
            e.scope = 1;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t num, l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CONTENTENCODINGORDER:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.order = num;
                    break;

                  case MATROSKA_ID_CONTENTENCODINGSCOPE:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.scope = num;
                    break;

                  case MATROSKA_ID_CONTENTENCODINGTYPE:
                    num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      goto err_out;
                    e.type = num;
                    break;

                  case MATROSKA_ID_CONTENTCOMPRESSION:
                    {
                      uint64_t le;

                      le = ebml_read_length (s, &i);
                      l = le + i;

                      while (le > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CONTENTCOMPALGO:
                              num = ebml_read_uint (s, &l);
                              if (num == EBML_UINT_INVALID)
                                goto err_out;
                              e.comp_algo = num;
                              break;

                            case MATROSKA_ID_CONTENTCOMPSETTINGS:
                              l = ebml_read_length (s, &i);
                              e.comp_settings = malloc (l);
                              stream_read (s, e.comp_settings, l);
                              e.comp_settings_len = l;
                              l += i;
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          le -= l + il;
                        }

                      if (e.type == 1)
                        {
                          mp_msg(MSGT_DEMUX, MSGL_WARN,
                                 MSGTR_MPDEMUX_MKV_TrackEncrypted, track->tnum);
                        }
                      else if (e.type != 0)
                        {
                          mp_msg(MSGT_DEMUX, MSGL_WARN,
                                 MSGTR_MPDEMUX_MKV_UnknownContentEncoding, track->tnum);
                        }

                      if (e.comp_algo != 0 && e.comp_algo != 2)
                        {
                          mp_msg (MSGT_DEMUX, MSGL_WARN,
                                  MSGTR_MPDEMUX_MKV_UnknownCompression,
                                  track->tnum, e.comp_algo);
                        }
#ifndef HAVE_ZLIB
                      else if (e.comp_algo == 0)
                        {
                          mp_msg (MSGT_DEMUX, MSGL_WARN,
                                  MSGTR_MPDEMUX_MKV_ZlibCompressionUnsupported,
                                  track->tnum);
                        }
#endif

                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            for (i=0; i<n; i++)
              if (e.order <= ce[i].order)
                break;
            ce = realloc (ce, (n+1) *sizeof (*ce));
            memmove (ce+i+1, ce+i, (n-i) * sizeof (*ce));
            memcpy (ce+i, &e, sizeof (e));
            n++;
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;
    }

  track->encodings = ce;
  track->num_encodings = n;
  return len;

err_out:
  demux_mkv_free_encodings(ce, n);
  return 0;
}

static int
demux_mkv_read_trackaudio (demuxer_t *demuxer, mkv_track_t *track)
{
  stream_t *s = demuxer->stream;
  uint64_t len, length, l;
  int il;

  track->a_sfreq = 8000.0;
  track->a_channels = 1;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_AUDIOSAMPLINGFREQ:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 0;
            track->a_sfreq = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Sampling frequency: %f\n",
                    track->a_sfreq);
            break;
          }

        case MATROSKA_ID_AUDIOBITDEPTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->a_bps = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Bit depth: %u\n",
                    track->a_bps);
            break;
          }

        case MATROSKA_ID_AUDIOCHANNELS:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->a_channels = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Channels: %u\n",
                    track->a_channels);
            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return len;
}

static int
demux_mkv_read_trackvideo (demuxer_t *demuxer, mkv_track_t *track)
{
  stream_t *s = demuxer->stream;
  uint64_t len, length, l;
  int il;

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_VIDEOFRAMERATE:
          {
            long double num = ebml_read_float (s, &l);
            if (num == EBML_FLOAT_INVALID)
              return 0;
            track->v_frate = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Frame rate: %f\n",
                    track->v_frate);
            if (track->v_frate > 0)
              track->default_duration = 1 / track->v_frate;
            break;
          }

        case MATROSKA_ID_VIDEODISPLAYWIDTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_dwidth = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Display width: %u\n",
                    track->v_dwidth);
            break;
          }

        case MATROSKA_ID_VIDEODISPLAYHEIGHT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_dheight = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Display height: %u\n",
                    track->v_dheight);
            break;
          }

        case MATROSKA_ID_VIDEOPIXELWIDTH:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_width = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Pixel width: %u\n",
                    track->v_width);
            break;
          }

        case MATROSKA_ID_VIDEOPIXELHEIGHT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->v_height = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |   + Pixel height: %u\n",
                    track->v_height);
            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return len;
}

/**
 * \brief free any data associated with given track
 * \param track track of which to free data
 */
static void
demux_mkv_free_trackentry(mkv_track_t *track) {
  free (track->name);
  free (track->codec_id);
  free (track->language);
  free (track->private_data);
  free (track->audio_buf);
  free (track->audio_timestamp);
  demux_mkv_free_encodings(track->encodings, track->num_encodings);
  free(track);
}

static int
demux_mkv_read_trackentry (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  mkv_track_t *track;
  uint64_t len, length, l;
  int il;

  track = calloc (1, sizeof (*track));
  /* set default values */
  track->default_track = 1;
  track->name = 0;
  track->language = strdup("eng");

  len = length = ebml_read_length (s, &il);
  len += il;
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TRACKNUMBER:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            track->tnum = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Track number: %u\n",
                    track->tnum);
            break;
          }

        case MATROSKA_ID_TRACKNAME:
          {
            track->name = ebml_read_utf8 (s, &l);
            if (track->name == NULL)
              goto err_out;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Name: %s\n",
                    track->name);
            break;
          }
          
        case MATROSKA_ID_TRACKTYPE:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              return 0;
            track->type = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Track type: ");
            switch (track->type)
              {
              case MATROSKA_TRACK_AUDIO:
                mp_msg (MSGT_DEMUX, MSGL_V, "Audio\n");
                break;
              case MATROSKA_TRACK_VIDEO:
                mp_msg (MSGT_DEMUX, MSGL_V, "Video\n");
                break;
              case MATROSKA_TRACK_SUBTITLE:
                mp_msg (MSGT_DEMUX, MSGL_V, "Subtitle\n");
                break;
              default:
                mp_msg (MSGT_DEMUX, MSGL_V, "unknown\n");
                break;
            }
            break;
          }

        case MATROSKA_ID_TRACKAUDIO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Audio track\n");
          l = demux_mkv_read_trackaudio (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

        case MATROSKA_ID_TRACKVIDEO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Video track\n");
          l = demux_mkv_read_trackvideo (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

        case MATROSKA_ID_CODECID:
          track->codec_id = ebml_read_ascii (s, &l);
          if (track->codec_id == NULL)
            goto err_out;
          if (!strcmp (track->codec_id, MKV_V_MSCOMP) ||
              !strcmp (track->codec_id, MKV_A_ACM))
            track->ms_compat = 1;
          else if (!strcmp (track->codec_id, MKV_S_VOBSUB))
            track->subtitle_type = MATROSKA_SUBTYPE_VOBSUB;
          else if (!strcmp (track->codec_id, MKV_S_TEXTSSA)
                   || !strcmp (track->codec_id, MKV_S_TEXTASS)
                   || !strcmp (track->codec_id, MKV_S_SSA)
                   || !strcmp (track->codec_id, MKV_S_ASS))
            {
              track->subtitle_type = MATROSKA_SUBTYPE_SSA;
            }
          else if (!strcmp (track->codec_id, MKV_S_TEXTASCII))
            track->subtitle_type = MATROSKA_SUBTYPE_TEXT;
          if (!strcmp (track->codec_id, MKV_S_TEXTUTF8))
            {
              track->subtitle_type = MATROSKA_SUBTYPE_TEXT;
            }
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Codec ID: %s\n",
                  track->codec_id);
          break;

        case MATROSKA_ID_CODECPRIVATE:
          {
            int x;
            uint64_t num = ebml_read_length (s, &x);
	    // audit: cheap guard against overflows later..
	    if (num > SIZE_MAX - 1000) return 0;
            l = x + num;
            track->private_data = malloc (num + LZO_INPUT_PADDING);
            if (stream_read(s, track->private_data, num) != (int) num)
              goto err_out;
            track->private_size = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + CodecPrivate, length "
                    "%u\n", track->private_size);
            break;
          }

        case MATROSKA_ID_TRACKLANGUAGE:
          free(track->language);
          track->language = ebml_read_utf8 (s, &l);
          if (track->language == NULL)
            goto err_out;
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Language: %s\n",
                  track->language);
          break;

        case MATROSKA_ID_TRACKFLAGDEFAULT:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            track->default_track = num;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default flag: %u\n",
                    track->default_track);
            break;
          }

        case MATROSKA_ID_TRACKDEFAULTDURATION:
          {
            uint64_t num = ebml_read_uint (s, &l);
            if (num == EBML_UINT_INVALID)
              goto err_out;
            if (num == 0)
              mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default duration: 0");
            else
              {
                track->v_frate = 1000000000.0 / num;
                track->default_duration = num / 1000000000.0;
                mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + Default duration: "
                        "%.3fms ( = %.3f fps)\n",num/1000000.0,track->v_frate);
              }
            break;
          }

        case MATROSKA_ID_TRACKENCODINGS:
          l = demux_mkv_read_trackencodings (demuxer, track);
          if (l == 0)
            goto err_out;
          break;

        default:
          ebml_read_skip (s, &l);
          break;
        }
      length -= l + il;
    }

  mkv_d->tracks[mkv_d->num_tracks++] = track;
  return len;

err_out:
  demux_mkv_free_trackentry(track);
  return 0;
}

static int
demux_mkv_read_tracks (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;

  mkv_d->tracks = malloc (sizeof (*mkv_d->tracks));
  mkv_d->num_tracks = 0;

  length = ebml_read_length (s, NULL);
  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_TRACKENTRY:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + a track...\n");
          mkv_d->tracks = realloc (mkv_d->tracks,
                                   (mkv_d->num_tracks+1)
                                   *sizeof (*mkv_d->tracks));
          l = demux_mkv_read_trackentry (demuxer);
          if (l == 0)
            return 1;
          break;

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }
  return 0;
}

static int
demux_mkv_read_cues (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l, time, track, pos;
  off_t off;
  int i, il;

  if (index_mode == 0) {
    ebml_read_skip (s, NULL);
    return 0;
  }
  off = stream_tell (s);
  for (i=0; i<mkv_d->parsed_cues_num; i++)
    if (mkv_d->parsed_cues[i] == off)
      {
        ebml_read_skip (s, NULL);
        return 0;
      }
  mkv_d->parsed_cues = realloc (mkv_d->parsed_cues, 
                                (mkv_d->parsed_cues_num+1)
                                * sizeof (off_t));
  mkv_d->parsed_cues[mkv_d->parsed_cues_num++] = off;

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing cues ] -----------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      time = track = pos = EBML_UINT_INVALID;

      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_POINTENTRY:
          {
            uint64_t len;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CUETIME:
                    time = ebml_read_uint (s, &l);
                    break;

                  case MATROSKA_ID_CUETRACKPOSITION:
                    {
                      uint64_t le;

                      le = ebml_read_length (s, &i);
                      l = le + i;

                      while (le > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CUETRACK:
                              track = ebml_read_uint (s, &l);
                              break;

                            case MATROSKA_ID_CUECLUSTERPOSITION:
                              pos = ebml_read_uint (s, &l);
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          le -= l + il;
                        }
                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;

      if (time != EBML_UINT_INVALID && track != EBML_UINT_INVALID
          && pos != EBML_UINT_INVALID)
        {
          grow_array(&mkv_d->indexes, mkv_d->num_indexes, sizeof(mkv_index_t));
          mkv_d->indexes[mkv_d->num_indexes].tnum = track;
          mkv_d->indexes[mkv_d->num_indexes].timecode = time;
          mkv_d->indexes[mkv_d->num_indexes].filepos =mkv_d->segment_start+pos;
          mp_msg (MSGT_DEMUX, MSGL_DBG2, "[mkv] |+ found cue point "
                  "for track %"PRIu64": timecode %"PRIu64", filepos: %"PRIu64"\n", 
                  track, time, mkv_d->segment_start + pos);
          mkv_d->num_indexes++;
        }
    }

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing cues ] -----------\n");
  return 0;
}

static int
demux_mkv_read_chapters (demuxer_t *demuxer)
{
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;

  if (demuxer->chapters)
    {
      ebml_read_skip (s, NULL);
      return 0;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing chapters ] ---------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_EDITIONENTRY:
          {
            uint64_t len;
            int i;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_CHAPTERATOM:
                    {
                      uint64_t len, start=0, end=0;
                      char* name = 0;
                      int i;
                      int cid;

                      len = ebml_read_length (s, &i);
                      l = len + i;

                      while (len > 0)
                        {
                          uint64_t l;
                          int il;

                          switch (ebml_read_id (s, &il))
                            {
                            case MATROSKA_ID_CHAPTERTIMESTART:
                              start = ebml_read_uint (s, &l) / 1000000;
                              break;

                            case MATROSKA_ID_CHAPTERTIMEEND:
                              end = ebml_read_uint (s, &l) / 1000000;
                              break;

                            case MATROSKA_ID_CHAPTERDISPLAY:
                              {
                                uint64_t len;
                                int i;

                                len = ebml_read_length (s, &i);
                                l = len + i;
                                while (len > 0)
                                  {
                                    uint64_t l;
                                    int il;

                                    switch (ebml_read_id (s, &il))
                                      {
                                        case MATROSKA_ID_CHAPSTRING:
                                          name = ebml_read_utf8 (s, &l);
                                          break;
                                        default:
                                          ebml_read_skip (s, &l);
                                          break;
                                      }
                                    len -= l + il;
                                  }
                              }
                              break;

                            default:
                              ebml_read_skip (s, &l);
                              break;
                            }
                          len -= l + il;
                        }

                      if (!name)
                        name = strdup("(unnamed)");
                      
                      cid = demuxer_add_chapter(demuxer, name, start, end);
                      
                      mp_msg(MSGT_DEMUX, MSGL_V,
                             "[mkv] Chapter %u from %02d:%02d:%02d."
                             "%03d to %02d:%02d:%02d.%03d, %s\n",
                             cid,
                             (int) (start / 60 / 60 / 1000),
                             (int) ((start / 60 / 1000) % 60),
                             (int) ((start / 1000) % 60),
                             (int) (start % 1000),
                             (int) (end / 60 / 60 / 1000),
                             (int) ((end / 60 / 1000) % 60),
                             (int) ((end / 1000) % 60),
                             (int) (end % 1000), name);

                      free(name);
                      break;
                    }

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }
            break;
          }

        default:
          ebml_read_skip (s, &l);
          break;
        }

      length -= l + il;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing chapters ] ---------\n");
  return 0;
}

static int
demux_mkv_read_tags (demuxer_t *demuxer)
{
  ebml_read_skip (demuxer->stream, NULL);
  return 0;
}

static int
demux_mkv_read_attachments (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l;
  int il;

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing attachments ] ---------\n");
  length = ebml_read_length (s, NULL);

  while (length > 0)
    {
      switch (ebml_read_id (s, &il))
        {
          case MATROSKA_ID_ATTACHEDFILE:
            {
              uint64_t len;
              int i;
              char* name = NULL;
              char* mime = NULL;
              char* data = NULL;
              int data_size = 0;

              len = ebml_read_length (s, &i);
              l = len + i;

              mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] | + an attachment...\n");

              while (len > 0)
                {
                  uint64_t l;
                  int il;

                  switch (ebml_read_id (s, &il))
                    {
                    case MATROSKA_ID_FILENAME:
                      name = ebml_read_utf8 (s, &l);
                      if (name == NULL)
                        return 0;
                      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileName: %s\n",
                        name);
                      break;

                    case MATROSKA_ID_FILEMIMETYPE:
                      mime = ebml_read_ascii (s, &l);
                      if (mime == NULL)
                        return 0;
                      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileMimeType: %s\n",
                        mime);
                      break;

                    case MATROSKA_ID_FILEDATA:
                      {
                        int x;
                        uint64_t num = ebml_read_length (s, &x);
                        l = x + num;
                        free(data);
                        data = malloc (num);
                        if (stream_read(s, data, num) != (int) num)
                        {
                          free(data);
                          return 0;
                        }
                        data_size = num;
                        mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |  + FileData, length "
                                "%u\n", data_size);
                        break;
                      }

                    default:
                      ebml_read_skip (s, &l);
                      break;
                    }
                  len -= l + il;
                }

              demuxer_add_attachment(demuxer, name, mime, data, data_size);
              mp_msg(MSGT_DEMUX, MSGL_V,
                     "[mkv] Attachment: %s, %s, %u bytes\n",
                     name, mime, data_size);
              break;
            }

          default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;
    }

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing attachments ] ---------\n");
  return 0;
}

static int
demux_mkv_read_seekhead (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t length, l, seek_pos, saved_pos, num;
  uint32_t seek_id;
  int i, il, res = 0;
  off_t off;

  off = stream_tell (s);
  for (i=0; i<mkv_d->parsed_seekhead_num; i++)
    if (mkv_d->parsed_seekhead[i] == off)
      {
        ebml_read_skip (s, NULL);
        return 0;
      }
  mkv_d->parsed_seekhead = realloc (mkv_d->parsed_seekhead, 
                                    (mkv_d->parsed_seekhead_num+1)
                                    * sizeof (off_t));
  mkv_d->parsed_seekhead[mkv_d->parsed_seekhead_num++] = off;

  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] /---- [ parsing seek head ] ---------\n");
  length = ebml_read_length (s, NULL);
  /* off now holds the position of the next element after the seek head. */
  off = stream_tell (s) + length;
  while (length > 0 && !res)
    {

      seek_id = 0;
      seek_pos = EBML_UINT_INVALID;

      switch (ebml_read_id (s, &il))
        {
        case MATROSKA_ID_SEEKENTRY:
          {
            uint64_t len;

            len = ebml_read_length (s, &i);
            l = len + i;

            while (len > 0)
              {
                uint64_t l;
                int il;

                switch (ebml_read_id (s, &il))
                  {
                  case MATROSKA_ID_SEEKID:
                    num = ebml_read_uint (s, &l);
                    if (num != EBML_UINT_INVALID)
                      seek_id = num;
                    break;

                  case MATROSKA_ID_SEEKPOSITION:
                    seek_pos = ebml_read_uint (s, &l);
                    break;

                  default:
                    ebml_read_skip (s, &l);
                    break;
                  }
                len -= l + il;
              }

            break;
          }

        default:
            ebml_read_skip (s, &l);
            break;
        }
      length -= l + il;

      if (seek_id == 0 || seek_id == MATROSKA_ID_CLUSTER
          || seek_pos == EBML_UINT_INVALID ||
          ((mkv_d->segment_start + seek_pos) >= (uint64_t)demuxer->movi_end))
        continue;

      saved_pos = stream_tell (s);
      if (!stream_seek (s, mkv_d->segment_start + seek_pos))
        res = 1;
      else
        {
          if (ebml_read_id (s, &il) != seek_id)
            res = 1;
          else
            switch (seek_id)
              {
              case MATROSKA_ID_CUES:
                if (demux_mkv_read_cues (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_TAGS:
                if (demux_mkv_read_tags (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_SEEKHEAD:
                if (demux_mkv_read_seekhead (demuxer))
                  res = 1;
                break;

              case MATROSKA_ID_CHAPTERS:
                if (demux_mkv_read_chapters (demuxer))
                  res = 1;
                break;
              }
        }

      stream_seek (s, saved_pos);
    }
  if (res)
    {
      /* If there was an error then try to skip this seek head. */
      if (stream_seek (s, off))
        res = 0;
    }
  else
  if (length > 0)
     stream_seek (s, stream_tell (s) + length);
  mp_msg(MSGT_DEMUX, MSGL_V, "[mkv] \\---- [ parsing seek head ] ---------\n");
  return res;
}

static int
demux_mkv_open_video (demuxer_t *demuxer, mkv_track_t *track, int vid);
static int
demux_mkv_open_audio (demuxer_t *demuxer, mkv_track_t *track, int aid);
static int
demux_mkv_open_sub (demuxer_t *demuxer, mkv_track_t *track, int sid);

static void
display_create_tracks (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *)demuxer->priv;
  int i, vid=0, aid=0, sid=0;

  for (i=0; i<mkv_d->num_tracks; i++)
    {
      char *type = "unknown", str[32];
      *str = '\0';
      switch (mkv_d->tracks[i]->type)
        {
        case MATROSKA_TRACK_VIDEO:
          type = "video";
          demux_mkv_open_video(demuxer, mkv_d->tracks[i], vid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VID_%d_NAME=%s\n", vid, mkv_d->tracks[i]->name);
          sprintf (str, "-vid %u", vid++);
          break;
        case MATROSKA_TRACK_AUDIO:
          type = "audio";
          demux_mkv_open_audio(demuxer, mkv_d->tracks[i], aid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_NAME=%s\n", aid, mkv_d->tracks[i]->name);
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_LANG=%s\n", aid, mkv_d->tracks[i]->language);
          sprintf (str, "-aid %u, -alang %.5s",aid++,mkv_d->tracks[i]->language);
          break;
        case MATROSKA_TRACK_SUBTITLE:
          type = "subtitles";
          demux_mkv_open_sub(demuxer, mkv_d->tracks[i], sid);
          if (mkv_d->tracks[i]->name)
            mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_NAME=%s\n", sid, mkv_d->tracks[i]->name);
          mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_LANG=%s\n", sid, mkv_d->tracks[i]->language);
          sprintf (str, "-sid %u, -slang %.5s",sid++,mkv_d->tracks[i]->language);
          break;
        }
      if (mkv_d->tracks[i]->name)
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_TrackIDName,
             mkv_d->tracks[i]->tnum, type, mkv_d->tracks[i]->codec_id, mkv_d->tracks[i]->name, str);
      else
        mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_TrackID,
             mkv_d->tracks[i]->tnum, type, mkv_d->tracks[i]->codec_id, str);
    }
}

typedef struct {
  char *id;
  int fourcc;
  int extradata;
} videocodec_info_t;

static const videocodec_info_t vinfo[] = {
  { MKV_V_MPEG1,     mmioFOURCC('m', 'p', 'g', '1'), 0 },
  { MKV_V_MPEG2,     mmioFOURCC('m', 'p', 'g', '2'), 0 },
  { MKV_V_MPEG4_SP,  mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_ASP, mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_AP,  mmioFOURCC('m', 'p', '4', 'v'), 1 },
  { MKV_V_MPEG4_AVC, mmioFOURCC('a', 'v', 'c', '1'), 1 },
  { MKV_V_THEORA,    mmioFOURCC('t', 'h', 'e', 'o'), 1 },
  { NULL, 0, 0 }
};

static int
demux_mkv_open_video (demuxer_t *demuxer, mkv_track_t *track, int vid)
{
  BITMAPINFOHEADER *bih;
  void *ImageDesc = NULL;
  sh_video_t *sh_v;

  if (track->ms_compat)  /* MS compatibility mode */
    {
      BITMAPINFOHEADER *src;

      if (track->private_data == NULL
          || track->private_size < sizeof (BITMAPINFOHEADER))
        return 1;

      src = (BITMAPINFOHEADER *) track->private_data;
      bih = calloc (1, track->private_size);
      bih->biSize = le2me_32 (src->biSize);
      bih->biWidth = le2me_32 (src->biWidth);
      bih->biHeight = le2me_32 (src->biHeight);
      bih->biPlanes = le2me_16 (src->biPlanes);
      bih->biBitCount = le2me_16 (src->biBitCount);
      bih->biCompression = le2me_32 (src->biCompression);
      bih->biSizeImage = le2me_32 (src->biSizeImage);
      bih->biXPelsPerMeter = le2me_32 (src->biXPelsPerMeter);
      bih->biYPelsPerMeter = le2me_32 (src->biYPelsPerMeter);
      bih->biClrUsed = le2me_32 (src->biClrUsed);
      bih->biClrImportant = le2me_32 (src->biClrImportant);
      memcpy((char *) bih + sizeof (BITMAPINFOHEADER),
             (char *) src + sizeof (BITMAPINFOHEADER),
             track->private_size - sizeof (BITMAPINFOHEADER));

      if (track->v_width == 0)
        track->v_width = bih->biWidth;
      if (track->v_height == 0)
        track->v_height = bih->biHeight;
    }
  else
    {
      bih = calloc (1, sizeof (BITMAPINFOHEADER));
      bih->biSize = sizeof (BITMAPINFOHEADER);
      bih->biWidth = track->v_width;
      bih->biHeight = track->v_height;
      bih->biBitCount = 24;
      bih->biSizeImage = bih->biWidth * bih->biHeight * bih->biBitCount/8;

      if (track->private_size >= RVPROPERTIES_SIZE
          && (!strcmp (track->codec_id, MKV_V_REALV10)
              || !strcmp (track->codec_id, MKV_V_REALV20)
              || !strcmp (track->codec_id, MKV_V_REALV30)
              || !strcmp (track->codec_id, MKV_V_REALV40)))
        {
          unsigned char *dst, *src;
          uint32_t type2;
          unsigned int cnt;

          src = track->private_data + RVPROPERTIES_SIZE;

          cnt = track->private_size - RVPROPERTIES_SIZE;
          bih = realloc(bih, sizeof (BITMAPINFOHEADER)+8+cnt);
          bih->biSize = 48+cnt;
          bih->biPlanes = 1;
          type2 = AV_RB32(src - 4);
          if (type2 == 0x10003000 || type2 == 0x10003001)
            bih->biCompression=mmioFOURCC('R','V','1','3');
          else
            bih->biCompression=mmioFOURCC('R','V',track->codec_id[9],'0');
          dst = (unsigned char *) (bih + 1);
          // copy type1 and type2 info from rv properties
          memcpy(dst, src - 8, 8);
          stream_read(demuxer->stream, dst+8, cnt);
          track->realmedia = 1;

#ifdef USE_QTX_CODECS
        }
      else if (track->private_size >= sizeof (ImageDescription)
               && !strcmp(track->codec_id, MKV_V_QUICKTIME))
        {
          ImageDescriptionPtr idesc;

          idesc = (ImageDescriptionPtr) track->private_data;
          idesc->idSize = be2me_32 (idesc->idSize);
          idesc->cType = be2me_32 (idesc->cType);
          idesc->version = be2me_16 (idesc->version);
          idesc->revisionLevel = be2me_16 (idesc->revisionLevel);
          idesc->vendor = be2me_32 (idesc->vendor);
          idesc->temporalQuality = be2me_32 (idesc->temporalQuality);
          idesc->spatialQuality = be2me_32 (idesc->spatialQuality);
          idesc->width = be2me_16 (idesc->width);
          idesc->height = be2me_16 (idesc->height);
          idesc->hRes = be2me_32 (idesc->hRes);
          idesc->vRes = be2me_32 (idesc->vRes);
          idesc->dataSize = be2me_32 (idesc->dataSize);
          idesc->frameCount = be2me_16 (idesc->frameCount);
          idesc->depth = be2me_16 (idesc->depth);
          idesc->clutID = be2me_16 (idesc->clutID);
          bih->biPlanes = 1;
          bih->biCompression = idesc->cType;
          ImageDesc = idesc;
#endif /* USE_QTX_CODECS */

        }
      else
        {
          const videocodec_info_t *vi = vinfo;
          while (vi->id && strcmp(vi->id, track->codec_id)) vi++;
          bih->biCompression = vi->fourcc;
          if (vi->extradata && track->private_data && (track->private_size > 0))
            {
              bih->biSize += track->private_size;
              bih = realloc (bih, bih->biSize);
              memcpy (bih + 1, track->private_data, track->private_size);
            }
          track->reorder_timecodes = user_correct_pts == 0;
          if (!vi->id) {
              mp_msg (MSGT_DEMUX,MSGL_WARN, MSGTR_MPDEMUX_MKV_UnknownCodecID,
                      track->codec_id, track->tnum);
              free(bih);
              return 1;
          }
        }
    }

  sh_v = new_sh_video_vid (demuxer, track->tnum, vid);
  sh_v->bih = bih;
  sh_v->format = sh_v->bih->biCompression;
  if (track->v_frate == 0.0)
    track->v_frate = 25.0;
  sh_v->fps = track->v_frate;
  sh_v->frametime = 1 / track->v_frate;
  sh_v->aspect = 0;
  if (!track->realmedia)
    {
      sh_v->disp_w = track->v_width;
      sh_v->disp_h = track->v_height;
      if (track->v_dheight)
      sh_v->aspect = (float)track->v_dwidth / (float)track->v_dheight;
    }
  else
    {
      // vd_realvid.c will set aspect to disp_w/disp_h and rederive
      // disp_w and disp_h from the RealVideo stream contents returned
      // by the Real DLLs. If DisplayWidth/DisplayHeight was not set in
      // the Matroska file then it has already been set to PixelWidth/Height
      // by check_track_information.
      sh_v->disp_w = track->v_dwidth;
      sh_v->disp_h = track->v_dheight;
    }
  sh_v->ImageDesc = ImageDesc;
  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] Aspect: %f\n", sh_v->aspect);

  sh_v->ds = demuxer->video;
  return 0;
}

static int
demux_mkv_open_audio (demuxer_t *demuxer, mkv_track_t *track, int aid)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  sh_audio_t *sh_a = new_sh_audio_aid(demuxer, track->tnum, aid);
  demux_packet_t *dp;
  if(!sh_a) return 1;
  mkv_d->audio_tracks[mkv_d->last_aid] = track->tnum;

  if (track->language && (strcmp(track->language, "und") != 0))
    sh_a->lang = strdup(track->language);
  sh_a->ds = demuxer->audio;
  sh_a->wf = malloc (sizeof (WAVEFORMATEX));
  if (track->ms_compat && (track->private_size >= sizeof(WAVEFORMATEX)))
    {
      WAVEFORMATEX *wf = (WAVEFORMATEX *)track->private_data;
      sh_a->wf = realloc(sh_a->wf, track->private_size);
      sh_a->wf->wFormatTag = le2me_16 (wf->wFormatTag);
      sh_a->wf->nChannels = le2me_16 (wf->nChannels);
      sh_a->wf->nSamplesPerSec = le2me_32 (wf->nSamplesPerSec);
      sh_a->wf->nAvgBytesPerSec = le2me_32 (wf->nAvgBytesPerSec);
      sh_a->wf->nBlockAlign = le2me_16 (wf->nBlockAlign);
      sh_a->wf->wBitsPerSample = le2me_16 (wf->wBitsPerSample);
      sh_a->wf->cbSize = track->private_size - sizeof(WAVEFORMATEX);
      memcpy(sh_a->wf + 1, wf + 1, track->private_size - sizeof(WAVEFORMATEX));
      if (track->a_sfreq == 0.0)
        track->a_sfreq = sh_a->wf->nSamplesPerSec;
      if (track->a_channels == 0)
        track->a_channels = sh_a->wf->nChannels;
      if (track->a_bps == 0)
        track->a_bps = sh_a->wf->wBitsPerSample;
      track->a_formattag = sh_a->wf->wFormatTag;
    }
  else
    {
      memset(sh_a->wf, 0, sizeof (WAVEFORMATEX));
      if (!strcmp(track->codec_id, MKV_A_MP3) ||
          !strcmp(track->codec_id, MKV_A_MP2))
        track->a_formattag = 0x0055;
      else if (!strncmp(track->codec_id, MKV_A_AC3, strlen(MKV_A_AC3)))
        track->a_formattag = 0x2000;
      else if (!strcmp(track->codec_id, MKV_A_DTS))
        track->a_formattag = 0x2001;
      else if (!strcmp(track->codec_id, MKV_A_PCM) ||
               !strcmp(track->codec_id, MKV_A_PCM_BE))
        track->a_formattag = 0x0001;
      else if (!strcmp(track->codec_id, MKV_A_AAC_2MAIN) ||
               !strncmp(track->codec_id, MKV_A_AAC_2LC,
                        strlen(MKV_A_AAC_2LC)) ||
               !strcmp(track->codec_id, MKV_A_AAC_2SSR) ||
               !strcmp(track->codec_id, MKV_A_AAC_4MAIN) ||
               !strncmp(track->codec_id, MKV_A_AAC_4LC,
                        strlen(MKV_A_AAC_4LC)) ||
               !strcmp(track->codec_id, MKV_A_AAC_4SSR) ||
               !strcmp(track->codec_id, MKV_A_AAC_4LTP) ||
               !strcmp(track->codec_id, MKV_A_AAC))
        track->a_formattag = mmioFOURCC('M', 'P', '4', 'A');
      else if (!strcmp(track->codec_id, MKV_A_VORBIS))
        {
          if (track->private_data == NULL)
            return 1;
          track->a_formattag = mmioFOURCC('v', 'r', 'b', 's');
        }
      else if (!strcmp(track->codec_id, MKV_A_QDMC))
        track->a_formattag = mmioFOURCC('Q', 'D', 'M', 'C');
      else if (!strcmp(track->codec_id, MKV_A_QDMC2))
        track->a_formattag = mmioFOURCC('Q', 'D', 'M', '2');
      else if (!strcmp(track->codec_id, MKV_A_WAVPACK))
        track->a_formattag = mmioFOURCC('W', 'V', 'P', 'K');
      else if (!strcmp(track->codec_id, MKV_A_FLAC))
        {
          if (track->private_data == NULL || track->private_size == 0)
            {
              mp_msg (MSGT_DEMUX, MSGL_WARN,
                      MSGTR_MPDEMUX_MKV_FlacTrackDoesNotContainValidHeaders);
              return 1;
            }
          track->a_formattag = mmioFOURCC ('f', 'L', 'a', 'C');
        }
      else if (track->private_size >= RAPROPERTIES4_SIZE)
        {
          if (!strcmp(track->codec_id, MKV_A_REAL28))
            track->a_formattag = mmioFOURCC('2', '8', '_', '8');
          else if (!strcmp(track->codec_id, MKV_A_REALATRC))
            track->a_formattag = mmioFOURCC('a', 't', 'r', 'c');
          else if (!strcmp(track->codec_id, MKV_A_REALCOOK))
            track->a_formattag = mmioFOURCC('c', 'o', 'o', 'k');
          else if (!strcmp(track->codec_id, MKV_A_REALDNET))
            track->a_formattag = mmioFOURCC('d', 'n', 'e', 't');
          else if (!strcmp(track->codec_id, MKV_A_REALSIPR))
            track->a_formattag = mmioFOURCC('s', 'i', 'p', 'r');
        }
      else
        {
          mp_msg (MSGT_DEMUX, MSGL_WARN, MSGTR_MPDEMUX_MKV_UnknownAudioCodec,
                  track->codec_id, track->tnum);
          free_sh_audio(demuxer, track->tnum);
          return 1;
        }
    }

  sh_a->format = track->a_formattag;
  sh_a->wf->wFormatTag = track->a_formattag;
  sh_a->channels = track->a_channels;
  sh_a->wf->nChannels = track->a_channels;
  sh_a->samplerate = (uint32_t) track->a_sfreq;
  sh_a->wf->nSamplesPerSec = (uint32_t) track->a_sfreq;
  if (track->a_bps == 0)
    {
      sh_a->samplesize = 2;
      sh_a->wf->wBitsPerSample = 16;
    }
  else
    {
      sh_a->samplesize = track->a_bps / 8;
      sh_a->wf->wBitsPerSample = track->a_bps;
    }
  if (track->a_formattag == 0x0055)  /* MP3 || MP2 */
    {
      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1152;
    }
  else if ((track->a_formattag == 0x2000) || /* AC3 */
           (track->a_formattag == 0x2001)) /* DTS */
    {
      free(sh_a->wf);
      sh_a->wf = NULL;
    }
  else if (track->a_formattag == 0x0001)  /* PCM || PCM_BE */
    {
      sh_a->wf->nAvgBytesPerSec = sh_a->channels * sh_a->samplerate*2;
      sh_a->wf->nBlockAlign = sh_a->wf->nAvgBytesPerSec;
      if (!strcmp(track->codec_id, MKV_A_PCM_BE))
        sh_a->format = mmioFOURCC('t', 'w', 'o', 's');
    }
  else if (!strcmp(track->codec_id, MKV_A_QDMC) ||
           !strcmp(track->codec_id, MKV_A_QDMC2))
    {
      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1486;
      track->fix_i_bps = 1;
      track->qt_last_a_pts = 0.0;
      if (track->private_data != NULL)
        {
          sh_a->codecdata=malloc(track->private_size);
          memcpy (sh_a->codecdata, track->private_data,
                  track->private_size);
          sh_a->codecdata_len = track->private_size;
        }
    }
  else if (track->a_formattag == mmioFOURCC('M', 'P', '4', 'A'))
    {
      int profile, srate_idx;

      sh_a->wf->nAvgBytesPerSec = 16000;
      sh_a->wf->nBlockAlign = 1024;

      if (!strcmp (track->codec_id, MKV_A_AAC) &&
          (NULL != track->private_data))
        {
          sh_a->codecdata=malloc(track->private_size);
          memcpy (sh_a->codecdata, track->private_data,
                  track->private_size);
          sh_a->codecdata_len = track->private_size;
          return 0;
        }

      /* Recreate the 'private data' */
      /* which faad2 uses in its initialization */
      srate_idx = aac_get_sample_rate_index (sh_a->samplerate);
      if (!strncmp (&track->codec_id[12], "MAIN", 4))
        profile = 0;
      else if (!strncmp (&track->codec_id[12], "LC", 2))
        profile = 1;
      else if (!strncmp (&track->codec_id[12], "SSR", 3))
        profile = 2;
      else
        profile = 3;
      sh_a->codecdata = malloc (5);
      sh_a->codecdata[0] = ((profile+1) << 3) | ((srate_idx&0xE) >> 1);
      sh_a->codecdata[1] = ((srate_idx&0x1)<<7)|(track->a_channels<<3);

      if (strstr(track->codec_id, "SBR") != NULL)
        {
          /* HE-AAC (aka SBR AAC) */
          sh_a->codecdata_len = 5;

          sh_a->samplerate *= 2;
          sh_a->wf->nSamplesPerSec *= 2;
          srate_idx = aac_get_sample_rate_index(sh_a->samplerate);
          sh_a->codecdata[2] = AAC_SYNC_EXTENSION_TYPE >> 3;
          sh_a->codecdata[3] = ((AAC_SYNC_EXTENSION_TYPE&0x07)<<5) | 5;
          sh_a->codecdata[4] = (1 << 7) | (srate_idx << 3);
          track->default_duration = 1024.0 / (sh_a->samplerate / 2);
        }
      else
        {
          sh_a->codecdata_len = 2;
          track->default_duration = 1024.0 / (float)sh_a->samplerate;
        }
    }
  else if (track->a_formattag == mmioFOURCC('v', 'r', 'b', 's'))  /* VORBIS */
    {
      sh_a->wf->cbSize = track->private_size;
      sh_a->wf = realloc(sh_a->wf, sizeof(WAVEFORMATEX) + sh_a->wf->cbSize);
      memcpy((unsigned char *) (sh_a->wf+1), track->private_data, sh_a->wf->cbSize);
    }
  else if (track->private_size >= RAPROPERTIES4_SIZE
           && !strncmp (track->codec_id, MKV_A_REALATRC, 7))
    {
      /* Common initialization for all RealAudio codecs */
      unsigned char *src = track->private_data;
      int codecdata_length, version;
      int flavor;

      sh_a->wf->nAvgBytesPerSec = 0;  /* FIXME !? */

      version = AV_RB16(src + 4);
      flavor = AV_RB16(src + 22);
      track->coded_framesize = AV_RB32(src + 24);
      track->sub_packet_h = AV_RB16(src + 40);
      sh_a->wf->nBlockAlign =
      track->audiopk_size = AV_RB16(src + 42);
      track->sub_packet_size = AV_RB16(src + 44);
      if (version == 4)
        {
          src += RAPROPERTIES4_SIZE;
          src += src[0] + 1;
          src += src[0] + 1;
        }
      else
        src += RAPROPERTIES5_SIZE;

      src += 3;
      if (version == 5)
        src++;
      codecdata_length = AV_RB32(src);
      src += 4;
      sh_a->wf->cbSize = codecdata_length;
      sh_a->wf = realloc (sh_a->wf,
                          sizeof (WAVEFORMATEX) +
                          sh_a->wf->cbSize);
      memcpy(((char *)(sh_a->wf + 1)), src, codecdata_length);

      switch (track->a_formattag) {
        case mmioFOURCC('a', 't', 'r', 'c'):
          sh_a->wf->nAvgBytesPerSec = atrc_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->sub_packet_size;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('c', 'o', 'o', 'k'):
          sh_a->wf->nAvgBytesPerSec = cook_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->sub_packet_size;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('s', 'i', 'p', 'r'):
          sh_a->wf->nAvgBytesPerSec = sipr_fl2bps[flavor];
          sh_a->wf->nBlockAlign = track->coded_framesize;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
        case mmioFOURCC('2', '8', '_', '8'):
          sh_a->wf->nAvgBytesPerSec = 3600;
          sh_a->wf->nBlockAlign = track->coded_framesize;
          track->audio_buf = malloc(track->sub_packet_h * track->audiopk_size);
          track->audio_timestamp = malloc(track->sub_packet_h * sizeof(float));
          break;
      }

      track->realmedia = 1;
    }
  else if (!strcmp(track->codec_id, MKV_A_FLAC) ||
           (track->a_formattag == 0xf1ac))
    {
      unsigned char *ptr;
      int size;
      free(sh_a->wf);
      sh_a->wf = NULL;

      if (track->a_formattag == mmioFOURCC('f', 'L', 'a', 'C'))
        {
          ptr = (unsigned char *)track->private_data;
          size = track->private_size;
        }
      else
        {
          sh_a->format = mmioFOURCC('f', 'L', 'a', 'C');
          ptr = (unsigned char *) track->private_data
            + sizeof (WAVEFORMATEX);
          size = track->private_size - sizeof (WAVEFORMATEX);
        }
      if (size < 4 || ptr[0] != 'f' || ptr[1] != 'L' ||
          ptr[2] != 'a' || ptr[3] != 'C')
        {
          dp = new_demux_packet (4);
          memcpy (dp->buffer, "fLaC", 4);
        }
      else
        {
          dp = new_demux_packet (size);
          memcpy (dp->buffer, ptr, size);
        }
      dp->pts = 0;
      dp->flags = 0;
      ds_add_packet (demuxer->audio, dp);
    }
  else if (track->a_formattag == mmioFOURCC('W', 'V', 'P', 'K'))
    {  /* do nothing, still works */  }
  else if (!track->ms_compat || (track->private_size < sizeof(WAVEFORMATEX)))
    {
      free_sh_audio(demuxer, track->tnum);
      return 1;
    }

  return 0;
}

/** \brief Parse the private data for VobSub subtitle tracks.

  This function tries to parse the private data for all VobSub tracks.
  The private data contains the normal text from the original .idx file.
  Things like the palette, subtitle dimensions and custom colors are
  stored here.

  \param demuxer The generic demuxer.
*/
static void
demux_mkv_parse_vobsub_data (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  mkv_track_t *track;
  int i;

  for (i = 0; i < mkv_d->num_tracks; i++)
    {
      track = mkv_d->tracks[i];
      if ((track->type != MATROSKA_TRACK_SUBTITLE) ||
          (track->subtitle_type != MATROSKA_SUBTYPE_VOBSUB))
        continue;

      if (!demux_mkv_parse_idx (track))
        {
          free (track->private_data);
          track->private_data = NULL;
          track->private_size = 0;
        }
    }
}

static int
demux_mkv_open_sub (demuxer_t *demuxer, mkv_track_t *track, int sid)
{
  if (track->subtitle_type != MATROSKA_SUBTYPE_UNKNOWN)
    {
      int size, m;
      uint8_t *buffer;
      sh_sub_t *sh = new_sh_sub_sid(demuxer, track->tnum, sid);
      track->sh_sub = sh;
      sh->type = 't';
      if (track->subtitle_type == MATROSKA_SUBTYPE_VOBSUB)
        sh->type = 'v';
      if (track->subtitle_type == MATROSKA_SUBTYPE_SSA)
        sh->type = 'a';
      size = track->private_size;
      m = demux_mkv_decode (track,track->private_data,&buffer,&size,2);
      if (buffer && m)
        {
          free (track->private_data);
          track->private_data = buffer;
          track->private_size = size;
        }
      sh->extradata=malloc(track->private_size);
      memcpy (sh->extradata, track->private_data,
              track->private_size);
      sh->extradata_len = track->private_size;
      if (track->language && (strcmp(track->language, "und") != 0))
        sh->lang = strdup(track->language);
    }
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_ERR, MSGTR_MPDEMUX_MKV_SubtitleTypeNotSupported,
              track->codec_id);
      return 1;
    }

  return 0;
}

static void demux_mkv_seek (demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags);

/** \brief Given a matroska track number and type, find the id that mplayer would ask for.
 *  \param d The demuxer for which the subtitle id should be returned.
 *  \param num The matroska track number we are looking up.
 *  \param type The track type.
 */
static int demux_mkv_reverse_id(mkv_demuxer_t *d, int num, int type)
{
  int i, id;
  
  for (i=0, id=0; i < d->num_tracks; i++)
    if (d->tracks[i] != NULL && d->tracks[i]->type == type) {
      if (d->tracks[i]->tnum == num)
        return id;
      id++;
    }
  
  return -1;
}

static int
demux_mkv_open (demuxer_t *demuxer)
{
  stream_t *s = demuxer->stream;
  mkv_demuxer_t *mkv_d;
  mkv_track_t *track;
  int i, version, cont = 0;
  char *str;

  stream_seek(s, s->start_pos);
  str = ebml_read_header (s, &version);
  if (str == NULL || strcmp (str, "matroska") || version > 2)
    {
      mp_msg (MSGT_DEMUX, MSGL_DBG2, "[mkv] no head found\n");
      return 0;
    }
  free (str);

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] Found the head...\n");

  if (ebml_read_id (s, NULL) != MATROSKA_ID_SEGMENT)
    {
      mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] but no segment :(\n");
      return 0;
    }
  ebml_read_length (s, NULL);  /* return bytes number until EOF */

  mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] + a segment...\n");

  mkv_d = calloc (1, sizeof (mkv_demuxer_t));
  demuxer->priv = mkv_d;
  mkv_d->tc_scale = 1000000;
  mkv_d->segment_start = stream_tell (s);
  mkv_d->parsed_cues = malloc (sizeof (off_t));
  mkv_d->parsed_seekhead = malloc (sizeof (off_t));

  while (!cont)
    {
      switch (ebml_read_id (s, NULL))
        {
        case MATROSKA_ID_INFO:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ segment information...\n");
          cont = demux_mkv_read_info (demuxer);
          break;

        case MATROSKA_ID_TRACKS:
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ segment tracks...\n");
          cont = demux_mkv_read_tracks (demuxer);
          break;

        case MATROSKA_ID_CUES:
          cont = demux_mkv_read_cues (demuxer);
          break;

        case MATROSKA_ID_TAGS:
          cont = demux_mkv_read_tags (demuxer);
          break;

        case MATROSKA_ID_SEEKHEAD:
          cont = demux_mkv_read_seekhead (demuxer);
          break;

        case MATROSKA_ID_CHAPTERS:
          cont = demux_mkv_read_chapters (demuxer);
          break;

        case MATROSKA_ID_ATTACHMENTS:
          cont = demux_mkv_read_attachments (demuxer);
          break;

        case MATROSKA_ID_CLUSTER:
          {
            int p, l;
            mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] |+ found cluster, headers are "
                    "parsed completely :)\n");
            /* get the first cluster timecode */
            p = stream_tell(s);
            l = ebml_read_length (s, NULL);
            while (ebml_read_id (s, NULL) != MATROSKA_ID_CLUSTERTIMECODE)
              {
                ebml_read_skip (s, NULL);
                if (stream_tell (s) >= p + l)
                  break;
              }
            if (stream_tell (s) < p + l)
              {
                uint64_t num = ebml_read_uint (s, NULL);
                if (num == EBML_UINT_INVALID)
                  return 0;
                mkv_d->first_tc = num * mkv_d->tc_scale / 1000000.0;
                mkv_d->has_first_tc = 1;
              }
            stream_seek (s, p - 4);
            cont = 1;
            break;
          }

        default:
          cont = 1;
        case EBML_ID_VOID:
          ebml_read_skip (s, NULL);
          break;
        }
    }

  display_create_tracks (demuxer);

  /* select video track */
  track = NULL;
  if (demuxer->video->id == -1)  /* automatically select a video track */
    {
      /* search for a video track that has the 'default' flag set */
      for (i=0; i<mkv_d->num_tracks; i++)
        if (mkv_d->tracks[i]->type == MATROSKA_TRACK_VIDEO
            && mkv_d->tracks[i]->default_track)
          {
            track = mkv_d->tracks[i];
            break;
          }

      if (track == NULL)
        /* no track has the 'default' flag set */
        /* let's take the first video track */
        for (i=0; i<mkv_d->num_tracks; i++)
          if (mkv_d->tracks[i]->type == MATROSKA_TRACK_VIDEO)
            {
              track = mkv_d->tracks[i];
              break;
            }
    }
  else if (demuxer->video->id != -2)  /* -2 = no video at all */
    track = demux_mkv_find_track_by_num (mkv_d, demuxer->video->id,
                                         MATROSKA_TRACK_VIDEO);

  if (track && demuxer->v_streams[track->tnum])
              {
                mp_msg (MSGT_DEMUX, MSGL_INFO,
                        MSGTR_MPDEMUX_MKV_WillPlayVideoTrack, track->tnum);
                demuxer->video->id = track->tnum;
                demuxer->video->sh = demuxer->v_streams[track->tnum];
              }
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_NoVideoTrackFound);
      demuxer->video->id = -2;
    }

  /* select audio track */
  track = NULL;
  if (track == NULL)
    /* search for an audio track that has the 'default' flag set */
    for (i=0; i < mkv_d->num_tracks; i++)
      if (mkv_d->tracks[i]->type == MATROSKA_TRACK_AUDIO
          && mkv_d->tracks[i]->default_track)
        {
          track = mkv_d->tracks[i];
          break;
        }

  if (track == NULL)
    /* no track has the 'default' flag set */
    /* let's take the first audio track */
    for (i=0; i < mkv_d->num_tracks; i++)
      if (mkv_d->tracks[i]->type == MATROSKA_TRACK_AUDIO)
        {
          track = mkv_d->tracks[i];
          break;
        }

  if (track && demuxer->a_streams[track->tnum])
    {
      demuxer->audio->id = track->tnum;
      demuxer->audio->sh = demuxer->a_streams[track->tnum];
    }
  else
    {
      mp_msg (MSGT_DEMUX, MSGL_INFO, MSGTR_MPDEMUX_MKV_NoAudioTrackFound);
      demuxer->audio->id = -2;
    }


  if(demuxer->audio->id != -2)
  for (i=0; i < mkv_d->num_tracks; i++)
    {
      if(mkv_d->tracks[i]->type != MATROSKA_TRACK_AUDIO)
          continue;
      if(demuxer->a_streams[track->tnum])
        {
          mkv_d->last_aid++;
          if(mkv_d->last_aid == MAX_A_STREAMS)
            break;
        }
    }

  demux_mkv_parse_vobsub_data (demuxer);

  if (demuxer->chapters)
    {
      for (i=0; i < (int)demuxer->num_chapters; i++)
        {
          demuxer->chapters[i].start -= mkv_d->first_tc;
          demuxer->chapters[i].end -= mkv_d->first_tc;
        }
      if (dvd_last_chapter > 0 && dvd_last_chapter <= demuxer->num_chapters)
        {
          if (demuxer->chapters[dvd_last_chapter-1].end != 0)
            mkv_d->stop_timecode = demuxer->chapters[dvd_last_chapter-1].end;
          else if (dvd_last_chapter + 1 <= demuxer->num_chapters)
            mkv_d->stop_timecode = demuxer->chapters[dvd_last_chapter].start;
        }
    }

  if (s->end_pos == 0 || (mkv_d->indexes == NULL && index_mode < 0))
    demuxer->seekable = 0;
  else
    {
      demuxer->movi_start = s->start_pos;
      demuxer->movi_end = s->end_pos;
      demuxer->seekable = 1;
    }

  return DEMUXER_TYPE_MATROSKA;
}

static void
demux_close_mkv (demuxer_t *demuxer)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;

  if (mkv_d)
    {
      int i;
      free_cached_dps (demuxer);
      if (mkv_d->tracks)
        {
          for (i=0; i<mkv_d->num_tracks; i++)
            demux_mkv_free_trackentry(mkv_d->tracks[i]);
          free (mkv_d->tracks);
        }
      free (mkv_d->indexes);
      free (mkv_d->cluster_positions);
      free (mkv_d->parsed_cues);
      free (mkv_d->parsed_seekhead);
      free (mkv_d);
    }
}

static int
demux_mkv_read_block_lacing (uint8_t *buffer, uint64_t *size,
                             uint8_t *laces, uint32_t **all_lace_sizes)
{
  uint32_t total = 0, *lace_size;
  uint8_t flags;
  int i;

  *all_lace_sizes = NULL;
  lace_size = NULL;
  /* lacing flags */
  flags = *buffer++;
  (*size)--;

  switch ((flags & 0x06) >> 1)
    {
    case 0:  /* no lacing */
      *laces = 1;
      lace_size = calloc(*laces, sizeof(uint32_t));
      lace_size[0] = *size;
      break;

    case 1:  /* xiph lacing */
    case 2:  /* fixed-size lacing */
    case 3:  /* EBML lacing */
      *laces = *buffer++;
      (*size)--;
      (*laces)++;
      lace_size = calloc(*laces, sizeof(uint32_t));

      switch ((flags & 0x06) >> 1)
        {
        case 1:  /* xiph lacing */
          for (i=0; i < *laces-1; i++)
            {
              lace_size[i] = 0; 
              do
                {
                  lace_size[i] += *buffer;
                  (*size)--;
                } while (*buffer++ == 0xFF);
              total += lace_size[i];
            }
          lace_size[i] = *size - total;
          break;

        case 2:  /* fixed-size lacing */
          for (i=0; i < *laces; i++)
            lace_size[i] = *size / *laces;
          break;

        case 3:  /* EBML lacing */
          {
            int l;
            uint64_t num = ebml_read_vlen_uint (buffer, &l);
            if (num == EBML_UINT_INVALID) {
              free(lace_size);
              return 1;
            }
            buffer += l;
            *size -= l;

            total = lace_size[0] = num;
            for (i=1; i < *laces-1; i++)
              {
                int64_t snum;
                snum = ebml_read_vlen_int (buffer, &l);
                if (snum == EBML_INT_INVALID) {
                  free(lace_size);
                  return 1;
                }
                buffer += l;
                *size -= l;
                lace_size[i] = lace_size[i-1] + snum;
                total += lace_size[i];
              }
            lace_size[i] = *size - total;
            break;
          }
        }
      break;
    }
  *all_lace_sizes = lace_size;
  return 0;
}

static void
handle_subtitles(demuxer_t *demuxer, mkv_track_t *track, char *block,
                 int64_t size, uint64_t block_duration, uint64_t timecode)
{
  demux_packet_t *dp;

  if (block_duration == 0)
    {
      mp_msg (MSGT_DEMUX, MSGL_WARN,
              MSGTR_MPDEMUX_MKV_NoBlockDurationForSubtitleTrackFound);
      return;
    }

  sub_utf8 = 1;
  dp = new_demux_packet(size);
  memcpy(dp->buffer, block, size);
  dp->pts = timecode / 1000.0f;
  dp->endpts = (timecode + block_duration) / 1000.0f;
  ds_add_packet(demuxer->sub, dp);
}

// Taken from demux_real.c. Thanks to the original developpers :)
#define SKIP_BITS(n) buffer <<= n
#define SHOW_BITS(n) ((buffer) >> (32 - (n)))

static float real_fix_timestamp(mkv_track_t *track, unsigned char *s,
                                int timestamp) {
  float v_pts;
  uint32_t buffer = (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
  int kf = timestamp;
  int pict_type;
  int orig_kf;

  if (!strcmp(track->codec_id, MKV_V_REALV30) ||
      !strcmp(track->codec_id, MKV_V_REALV40)) {

    if (!strcmp(track->codec_id, MKV_V_REALV30)) {
      SKIP_BITS(3);
      pict_type = SHOW_BITS(2);
      SKIP_BITS(2 + 7);
    }else{
      SKIP_BITS(1);
      pict_type = SHOW_BITS(2);
      SKIP_BITS(2 + 7 + 3);
    }
    kf = SHOW_BITS(13);         // kf= 2*SHOW_BITS(12);
    orig_kf = kf;
    if (pict_type <= 1) {
      // I frame, sync timestamps:
      track->rv_kf_base = timestamp - kf;
      mp_msg(MSGT_DEMUX, MSGL_DBG2, "\nTS: base=%08X\n", track->rv_kf_base);
      kf = timestamp;
    } else {
      // P/B frame, merge timestamps:
      int tmp = timestamp - track->rv_kf_base;
      kf |= tmp & (~0x1fff);    // combine with packet timestamp
      if (kf < (tmp - 4096))    // workaround wrap-around problems
        kf += 8192;
      else if (kf > (tmp + 4096))
        kf -= 8192;
      kf += track->rv_kf_base;
    }
    if (pict_type != 3) {       // P || I  frame -> swap timestamps
      int tmp = kf;
      kf = track->rv_kf_pts;
      track->rv_kf_pts = tmp;
    }
    mp_msg(MSGT_DEMUX, MSGL_DBG2, "\nTS: %08X -> %08X (%04X) %d %02X %02X %02X "
           "%02X %5d\n", timestamp, kf, orig_kf, pict_type, s[0], s[1], s[2],
           s[3], kf - (int)(1000.0 * track->rv_pts));
  }
  v_pts = kf * 0.001f;
  track->rv_pts = v_pts;

  return v_pts;
}

static void
handle_realvideo (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                  uint32_t size, int block_bref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  demux_packet_t *dp;
  uint32_t timestamp = mkv_d->last_pts * 1000;
  uint32_t *hdr;
  uint8_t chunks;
  int isize;
#ifdef WORDS_BIGENDIAN
  uint8_t *p;
  int i;
#endif

  chunks = *buffer++;
  isize = --size - (chunks+1)*8;
  dp = new_demux_packet (REALHEADER_SIZE + size);
  memcpy (dp->buffer + REALHEADER_SIZE, buffer + (chunks+1)*8, isize);
#ifdef WORDS_BIGENDIAN
  p = (uint8_t *)(dp->buffer + REALHEADER_SIZE + isize);
  for (i = 0; i<(chunks+1)*8; i+=4) {
    p[i] = *((uint8_t *)buffer+i+3);
    p[i+1] = *((uint8_t *)buffer+i+2);
    p[i+2] = *((uint8_t *)buffer+i+1);
    p[i+3] = *((uint8_t *)buffer+i);
  }
#else
  memcpy (dp->buffer + REALHEADER_SIZE + isize, buffer, (chunks+1)*8);
#endif

  hdr = dp->buffer;
  *hdr++ = chunks;                 // number of chunks
  *hdr++ = timestamp;              // timestamp from packet header
  *hdr++ = isize;                  // length of actual data
  *hdr++ = REALHEADER_SIZE + isize;    // offset to chunk offset array

  if (mkv_d->v_skip_to_keyframe)
    {
      dp->pts = mkv_d->last_pts;
      track->rv_kf_base = 0;
      track->rv_kf_pts = timestamp;
    }
  else
    dp->pts = real_fix_timestamp (track, dp->buffer + REALHEADER_SIZE,
                                  timestamp);
  dp->pos = demuxer->filepos;
  dp->flags = block_bref ? 0 : 0x10;

  ds_add_packet(demuxer->video, dp);
}

static void
handle_realaudio (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                  uint32_t size, int block_bref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  int sps = track->sub_packet_size;
  int sph = track->sub_packet_h;
  int cfs = track->coded_framesize;
  int w = track->audiopk_size;
  int spc = track->sub_packet_cnt;
  demux_packet_t *dp;
  int x;

  if ((track->a_formattag == mmioFOURCC('2', '8', '_', '8')) ||
      (track->a_formattag == mmioFOURCC('c', 'o', 'o', 'k')) ||
      (track->a_formattag == mmioFOURCC('a', 't', 'r', 'c')) ||
      (track->a_formattag == mmioFOURCC('s', 'i', 'p', 'r')))
    {
//      if(!block_bref)
//        spc = track->sub_packet_cnt = 0;
      switch (track->a_formattag) {
        case mmioFOURCC('2', '8', '_', '8'):
          for (x = 0; x < sph / 2; x++)
            memcpy(track->audio_buf + x * 2 * w + spc * cfs, buffer + cfs * x, cfs);
          break;
        case mmioFOURCC('c', 'o', 'o', 'k'):
        case mmioFOURCC('a', 't', 'r', 'c'):
          for (x = 0; x < w / sps; x++)
            memcpy(track->audio_buf + sps * (sph * x + ((sph + 1) / 2) * (spc & 1) + (spc >> 1)), buffer + sps * x, sps);
          break;
        case mmioFOURCC('s', 'i', 'p', 'r'):
          memcpy(track->audio_buf + spc * w, buffer, w);
          if (spc == sph - 1)
            {
              int n;
              int bs = sph * w * 2 / 96;  // nibbles per subpacket
              // Perform reordering
              for(n=0; n < 38; n++)
                {
                  int j;
                  int i = bs * sipr_swaps[n][0];
                  int o = bs * sipr_swaps[n][1];
                  // swap nibbles of block 'i' with 'o'      TODO: optimize
                  for(j = 0;j < bs; j++)
                    {
                      int x = (i & 1) ? (track->audio_buf[i >> 1] >> 4) : (track->audio_buf[i >> 1] & 0x0F);
                      int y = (o & 1) ? (track->audio_buf[o >> 1] >> 4) : (track->audio_buf[o >> 1] & 0x0F);
                      if(o & 1)
                        track->audio_buf[o >> 1] = (track->audio_buf[o >> 1] & 0x0F) | (x << 4);
                      else
                        track->audio_buf[o >> 1] = (track->audio_buf[o >> 1] & 0xF0) | x;
                      if(i & 1)
                        track->audio_buf[i >> 1] = (track->audio_buf[i >> 1] & 0x0F) | (y << 4);
                      else
                        track->audio_buf[i >> 1] = (track->audio_buf[i >> 1] & 0xF0) | y;
                      ++i; ++o;
                    }
                }
            }
          break;
      }
      track->audio_timestamp[track->sub_packet_cnt] = (track->ra_pts == mkv_d->last_pts) ? 0 : (mkv_d->last_pts);
      track->ra_pts = mkv_d->last_pts;
      if (track->sub_packet_cnt == 0)
        track->audio_filepos = demuxer->filepos;
      if (++(track->sub_packet_cnt) == sph)
        {
           int apk_usize = ((WAVEFORMATEX*)((sh_audio_t*)demuxer->audio->sh)->wf)->nBlockAlign;
           track->sub_packet_cnt = 0;
           // Release all the audio packets
           for (x = 0; x < sph*w/apk_usize; x++)
             {
               dp = new_demux_packet(apk_usize);
               memcpy(dp->buffer, track->audio_buf + x * apk_usize, apk_usize);
               /* Put timestamp only on packets that correspond to original audio packets in file */
               dp->pts = (x * apk_usize % w) ? 0 : track->audio_timestamp[x * apk_usize / w];
               dp->pos = track->audio_filepos; // all equal
               dp->flags = x ? 0 : 0x10; // Mark first packet as keyframe
               ds_add_packet(demuxer->audio, dp);
             }
        }
   } else { // Not a codec that require reordering
  dp = new_demux_packet (size);
  memcpy(dp->buffer, buffer, size);
  if (track->ra_pts == mkv_d->last_pts && !mkv_d->a_skip_to_keyframe)
    dp->pts = 0;
  else
    dp->pts = mkv_d->last_pts;
  track->ra_pts = mkv_d->last_pts;

  dp->pos = demuxer->filepos;
  dp->flags = block_bref ? 0 : 0x10;
  ds_add_packet (demuxer->audio, dp);
  }
}

/** Reorder timecodes and add cached demux packets to the queues.
 *
 * Timecode reordering is needed if a video track contains B frames that
 * are timestamped in display order (e.g. MPEG-1, MPEG-2 or "native" MPEG-4).
 * MPlayer doesn't like timestamps in display order. This function adjusts
 * the timestamp of cached frames (which are exactly one I/P frame followed
 * by one or more B frames) so that they are in coding order again.
 *
 * Example: The track with 25 FPS contains four frames with the timecodes
 * I at 0ms, P at 120ms, B at 40ms and B at 80ms. As soon as the next I
 * or P frame arrives these timecodes can be changed to I at 0ms, P at 40ms,
 * B at 80ms and B at 120ms.
 *
 * This works for simple H.264 B-frame pyramids, but not for arbitrary orders.
 *
 * \param demuxer The Matroska demuxer struct for this instance.
 * \param track The track structure whose cache should be handled.
 */
static void
flush_cached_dps (demuxer_t *demuxer, mkv_track_t *track)
{
  int i, ok;

  if (track->num_cached_dps == 0)
    return;

  do {
    ok = 1;
    for (i = 1; i < track->num_cached_dps; i++)
      if (track->cached_dps[i - 1]->pts > track->cached_dps[i]->pts) {
        float tmp_pts = track->cached_dps[i - 1]->pts;
        track->cached_dps[i - 1]->pts = track->cached_dps[i]->pts;
        track->cached_dps[i]->pts = tmp_pts;
        ok = 0;
      }
  } while (!ok);

  for (i = 0; i < track->num_cached_dps; i++)
    ds_add_packet (demuxer->video, track->cached_dps[i]);
  track->num_cached_dps = 0;
}

/** Cache video frames if timecodes have to be reordered.
 *
 * Timecode reordering is needed if a video track contains B frames that
 * are timestamped in display order (e.g. MPEG-1, MPEG-2 or "native" MPEG-4).
 * This function takes in a Matroska block read from the file, allocates a
 * demux packet for it, fills in its values, allocates space for storing
 * pointers to the cached demux packets and adds the packet to it. If
 * the packet contains an I or a P frame then ::flush_cached_dps is called
 * in order to send the old cached frames downstream.
 *
 * \param demuxer The Matroska demuxer struct for this instance.
 * \param track The packet is meant for this track.
 * \param buffer The actual frame contents.
 * \param size The frame size in bytes.
 * \param block_bref A relative timecode (backward reference). If it is \c 0
 *   then the frame is an I frame.
 * \param block_fref A relative timecode (forward reference). If it is \c 0
 *   then the frame is either an I frame or a P frame depending on the value
 *   of \a block_bref. Otherwise it's a B frame.
 */
static void
handle_video_bframes (demuxer_t *demuxer, mkv_track_t *track, uint8_t *buffer,
                      uint32_t size, int block_bref, int block_fref)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  demux_packet_t *dp;

  dp = new_demux_packet (size);
  memcpy(dp->buffer, buffer, size);
  dp->pos = demuxer->filepos;
  dp->pts = mkv_d->last_pts;
  if ((track->num_cached_dps > 0) && (dp->pts < track->max_pts))
    block_fref = 1;
  if (block_fref == 0)          /* I or P frame */
    flush_cached_dps (demuxer, track);
  if (block_bref != 0)          /* I frame, don't cache it */
    dp->flags = 0x10;
  if ((track->num_cached_dps + 1) > track->num_allocated_dps)
    {
      track->cached_dps = (demux_packet_t **)
        realloc(track->cached_dps, (track->num_cached_dps + 10) *
                sizeof(demux_packet_t *));
      track->num_allocated_dps += 10;
    }
  track->cached_dps[track->num_cached_dps] = dp;
  track->num_cached_dps++;
  if (dp->pts > track->max_pts)
    track->max_pts = dp->pts;
}

static int
handle_block (demuxer_t *demuxer, uint8_t *block, uint64_t length,
              uint64_t block_duration, int64_t block_bref, int64_t block_fref, uint8_t simpleblock)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  mkv_track_t *track = NULL;
  demux_stream_t *ds = NULL;
  uint64_t old_length;
  int64_t tc;
  uint32_t *lace_size;
  uint8_t laces, flags;
  int i, num, tmp, use_this_block = 1;
  float current_pts;
  int16_t time;

  /* first byte(s): track num */
  num = ebml_read_vlen_uint (block, &tmp);
  block += tmp;
  /* time (relative to cluster time) */
  time = block[0] << 8 | block[1];
  block += 2;
  length -= tmp + 2;
  old_length = length;
  flags = block[0];
  if (demux_mkv_read_block_lacing (block, &length, &laces, &lace_size))
    return 0;
  block += old_length - length;

  tc = ((time*mkv_d->tc_scale+mkv_d->cluster_tc) /1000000.0 - mkv_d->first_tc);
  if (tc < 0)
    tc = 0;
  if (mkv_d->stop_timecode > 0 && tc > mkv_d->stop_timecode) {
    free(lace_size);
    return -1;
  }
  current_pts = tc / 1000.0;

  for (i=0; i<mkv_d->num_tracks; i++)
    if (mkv_d->tracks[i]->tnum == num) {
      track = mkv_d->tracks[i];
      break;
    }
  if (track == NULL)
    {
      free(lace_size);
      return 1;
    }
  if (num == demuxer->audio->id)
    {
      ds = demuxer->audio;

      if (mkv_d->a_skip_to_keyframe) 
        {
          if (simpleblock) 
            {
               if (!(flags&0x80))   /*current frame isn't a keyframe*/
                 use_this_block = 0;
            }
          else if (block_bref != 0)
            use_this_block = 0;
        }
      else if (mkv_d->v_skip_to_keyframe)
        use_this_block = 0;

      if (track->fix_i_bps && use_this_block)
        {
          sh_audio_t *sh = (sh_audio_t *) ds->sh;

          if (block_duration != 0)
            {
              sh->i_bps = length * 1000 / block_duration;
              track->fix_i_bps = 0;
            }
          else if (track->qt_last_a_pts == 0.0)
            track->qt_last_a_pts = current_pts;
          else if(track->qt_last_a_pts != current_pts)
            {
              sh->i_bps = length / (current_pts - track->qt_last_a_pts);
              track->fix_i_bps = 0;
            }
        }
    }
  else if (tc < mkv_d->skip_to_timecode)
    use_this_block = 0;
  else if (num == demuxer->video->id)
    {
      ds = demuxer->video;
      if (mkv_d->v_skip_to_keyframe)
        {
          if (simpleblock)
            {
              if (!(flags&0x80))   /*current frame isn't a keyframe*/
                use_this_block = 0;
            }
          else if (block_bref != 0 || block_fref != 0)
            use_this_block = 0;
        }
    }
  else if (num == demuxer->sub->id)
    {
      ds = demuxer->sub;
      if (track->subtitle_type != MATROSKA_SUBTYPE_VOBSUB)
        {
          if (!mkv_d->v_skip_to_keyframe)
            handle_subtitles (demuxer, track, block, length,
                              block_duration, tc);
          use_this_block = 0;
        }
    }
  else
    use_this_block = 0;

  if (use_this_block)
    {
      mkv_d->last_pts = current_pts;
      mkv_d->last_filepos = demuxer->filepos;

      for (i=0; i < laces; i++)
        {
          if (ds == demuxer->video && track->realmedia)
            handle_realvideo (demuxer, track, block, lace_size[i], block_bref);
          else if (ds == demuxer->audio && track->realmedia)
            handle_realaudio (demuxer, track, block, lace_size[i], block_bref);
          else if (ds == demuxer->video && track->reorder_timecodes)
            handle_video_bframes (demuxer, track, block, lace_size[i],
                                  block_bref, block_fref);
          else
            {
              int modified, size = lace_size[i];
              demux_packet_t *dp;
              uint8_t *buffer;
              modified = demux_mkv_decode (track, block, &buffer, &size, 1);
              if (buffer)
                {
                  dp = new_demux_packet (size);
                  memcpy (dp->buffer, buffer, size);
                  if (modified)
                    free (buffer);
                  dp->flags = (block_bref == 0 && block_fref == 0) ? 0x10 : 0;
                  /* If default_duration is 0, assume no pts value is known
                   * for packets after the first one (rather than all pts
                   * values being the same) */
                  if (i == 0 || track->default_duration)
                  dp->pts = mkv_d->last_pts + i * track->default_duration;
                  ds_add_packet (ds, dp);
                }
            }
          block += lace_size[i];
        }

      if (ds == demuxer->video)
        {
          mkv_d->v_skip_to_keyframe = 0;
          mkv_d->skip_to_timecode = 0;
        }
      else if (ds == demuxer->audio)
        mkv_d->a_skip_to_keyframe = 0;

      free(lace_size);
      return 1;
    }

  free(lace_size);
  return 0;
}

static int
demux_mkv_fill_buffer (demuxer_t *demuxer, demux_stream_t *ds)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  stream_t *s = demuxer->stream;
  uint64_t l;
  int il, tmp;

  while (1)
    {
      while (mkv_d->cluster_size > 0)
        {
          uint64_t block_duration = 0,  block_length = 0;
          int64_t block_bref = 0, block_fref = 0;
          uint8_t *block = NULL;

          while (mkv_d->blockgroup_size > 0)
            {
              switch (ebml_read_id (s, &il))
                {
                case MATROSKA_ID_BLOCKDURATION:
                  {
                    block_duration = ebml_read_uint (s, &l);
                    if (block_duration == EBML_UINT_INVALID) {
                      free(block);
                      return 0;
                    }
                    block_duration *= mkv_d->tc_scale / 1000000.0;
                    break;
                  }

                case MATROSKA_ID_BLOCK:
                  block_length = ebml_read_length (s, &tmp);
                  free(block);
                  if (block_length > SIZE_MAX - LZO_INPUT_PADDING) return 0;
                  block = malloc (block_length + LZO_INPUT_PADDING);
                  demuxer->filepos = stream_tell (s);
                  if (stream_read (s,block,block_length) != (int) block_length)
                  {
                    free(block);
                    return 0;
                  }
                  l = tmp + block_length;
                  break;

                case MATROSKA_ID_REFERENCEBLOCK:
                  {
                    int64_t num = ebml_read_int (s, &l);
                    if (num == EBML_INT_INVALID) {
                      free(block);
                      return 0;
                    }
                    if (num <= 0)
                      block_bref = num;
                    else
                      block_fref = num;
                    break;
                  }

                case EBML_ID_INVALID:
                  free(block);
                  return 0;

                default:
                  ebml_read_skip (s, &l);
                  break;
                }
              mkv_d->blockgroup_size -= l + il;
              mkv_d->cluster_size -= l + il;
            }

          if (block)
            {
              int res = handle_block (demuxer, block, block_length,
                                      block_duration, block_bref, block_fref, 0);
              free (block);
              if (res < 0)
                return 0;
              if (res)
                return 1;
            }

          if (mkv_d->cluster_size > 0)
            {
              switch (ebml_read_id (s, &il))
                {
                case MATROSKA_ID_CLUSTERTIMECODE:
                  {
                    uint64_t num = ebml_read_uint (s, &l);
                    if (num == EBML_UINT_INVALID)
                      return 0;
                    if (!mkv_d->has_first_tc)
                      {
                        mkv_d->first_tc = num * mkv_d->tc_scale / 1000000.0;
                        mkv_d->has_first_tc = 1;
                      }
                    mkv_d->cluster_tc = num * mkv_d->tc_scale;
                    break;
                  }

                case MATROSKA_ID_BLOCKGROUP:
                  mkv_d->blockgroup_size = ebml_read_length (s, &tmp);
                  l = tmp;
                  break;

                case MATROSKA_ID_SIMPLEBLOCK:
                  {
                    int res;
                    block_length = ebml_read_length (s, &tmp);
                    block = malloc (block_length);
                    demuxer->filepos = stream_tell (s);
                    if (stream_read (s,block,block_length) != (int) block_length)
                    {
                      free(block);
                      return 0;
                    }
                    l = tmp + block_length;
                    res = handle_block (demuxer, block, block_length,
                                        block_duration, block_bref, block_fref, 1);
                    free (block);
                    mkv_d->cluster_size -= l + il;
                    if (res < 0)
                      return 0;
                    else if (res)
                      return 1;
                    else mkv_d->cluster_size += l + il;
                    break;
                  }
                case EBML_ID_INVALID:
                  return 0;

                default:
                  ebml_read_skip (s, &l);
                  break;
                }
              mkv_d->cluster_size -= l + il;
            }
        }

      if (ebml_read_id (s, &il) != MATROSKA_ID_CLUSTER)
        return 0;
      add_cluster_position(mkv_d, stream_tell(s)-il);
      mkv_d->cluster_size = ebml_read_length (s, NULL);
    }

  return 0;
}

static void
demux_mkv_seek (demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags)
{
  free_cached_dps (demuxer);
  if (!(flags & SEEK_FACTOR))  /* time in secs */
    {
      mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
      stream_t *s = demuxer->stream;
      int64_t target_timecode = 0, diff, min_diff=0xFFFFFFFFFFFFFFFLL;
      int i;

      if (!(flags & SEEK_ABSOLUTE))  /* relative seek */
        target_timecode = (int64_t) (mkv_d->last_pts * 1000.0);
      target_timecode += (int64_t)(rel_seek_secs * 1000.0);
      if (target_timecode < 0)
        target_timecode = 0;

      if (mkv_d->indexes == NULL)  /* no index was found */
        {
          uint64_t target_filepos, cluster_pos, max_pos;

          target_filepos = (uint64_t) (target_timecode * mkv_d->last_filepos
                                       / (mkv_d->last_pts * 1000.0));

          max_pos = mkv_d->cluster_positions[mkv_d->num_cluster_pos-1];
          if (target_filepos > max_pos)
            {
              if ((off_t) max_pos > stream_tell (s))
                stream_seek (s, max_pos);
              else
                stream_seek (s, stream_tell (s) + mkv_d->cluster_size);
              /* parse all the clusters upto target_filepos */
              while (!s->eof && stream_tell(s) < (off_t) target_filepos)
                {
                  switch (ebml_read_id (s, &i))
                    {
                    case MATROSKA_ID_CLUSTER:
                      add_cluster_position(mkv_d, (uint64_t) stream_tell(s)-i);
                      break;

                    case MATROSKA_ID_CUES:
                      demux_mkv_read_cues (demuxer);
                      break;
                    }
                  ebml_read_skip (s, NULL);
                }
              if (s->eof)
                stream_reset(s);
            }

          if (mkv_d->indexes == NULL)
            {
              cluster_pos = mkv_d->cluster_positions[0];
              /* Let's find the nearest cluster */
              for (i=0; i < mkv_d->num_cluster_pos; i++)
                {
                  diff = mkv_d->cluster_positions[i] - target_filepos;
                  if (rel_seek_secs < 0 && diff < 0 && -diff < min_diff)
                    {
                      cluster_pos = mkv_d->cluster_positions[i];
                      min_diff = -diff;
                    }
                  else if (rel_seek_secs > 0
                           && (diff < 0 ? -1 * diff : diff) < min_diff)
                    {
                      cluster_pos = mkv_d->cluster_positions[i];
                      min_diff = diff < 0 ? -1 * diff : diff;
                    }
                }
              mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              stream_seek (s, cluster_pos);
            }
        }
      else
        {
          mkv_index_t *index = NULL;
          int seek_id = (demuxer->video->id < 0) ? demuxer->audio->id : demuxer->video->id;  

          /* let's find the entry in the indexes with the smallest */
          /* difference to the wanted timecode. */
          for (i=0; i < mkv_d->num_indexes; i++)
            if (mkv_d->indexes[i].tnum == seek_id)
              {
                diff = target_timecode + mkv_d->first_tc -
                       (int64_t) mkv_d->indexes[i].timecode * mkv_d->tc_scale / 1000000.0;

                if ((flags & SEEK_ABSOLUTE || target_timecode <= mkv_d->last_pts*1000)) {
                    // Absolute seek or seek backward: find the last index
                    // position before target time
                    if (diff < 0 || diff >= min_diff)
                        continue;
                }
                else {
                    // Relative seek forward: find the first index position
                    // after target time. If no such index exists, find last
                    // position between current position and target time.
                    if (diff <= 0) {
                        if (min_diff <= 0 && diff <= min_diff)
                            continue;
                    }
                    else if (diff >= FFMIN(target_timecode - mkv_d->last_pts,
                                           min_diff))
                        continue;
                }
                min_diff = diff;
                index = mkv_d->indexes + i;
              }

          if (index)  /* We've found an entry. */
            {
              mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
              stream_seek (s, index->filepos);
            }
        }

      if (demuxer->video->id >= 0)
        mkv_d->v_skip_to_keyframe = 1;
      if (rel_seek_secs > 0.0)
        mkv_d->skip_to_timecode = target_timecode;
      mkv_d->a_skip_to_keyframe = 1;

      demux_mkv_fill_buffer(demuxer, NULL);
    }
  else if ((demuxer->movi_end <= 0) || !(flags & SEEK_ABSOLUTE))
    mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] seek unsupported flags\n");
  else
    {
      mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
      stream_t *s = demuxer->stream;
      uint64_t target_filepos;
      mkv_index_t *index = NULL;
      int i;

      if (mkv_d->indexes == NULL)  /* no index was found */
        {                       /* I'm lazy... */
          mp_msg (MSGT_DEMUX, MSGL_V, "[mkv] seek unsupported flags\n");
          return;
        }

      target_filepos = (uint64_t)(demuxer->movi_end * rel_seek_secs);
      for (i=0; i < mkv_d->num_indexes; i++)
        if (mkv_d->indexes[i].tnum == demuxer->video->id)
          if ((index == NULL) ||
              ((mkv_d->indexes[i].filepos >= target_filepos) &&
               ((index->filepos < target_filepos) ||
                (mkv_d->indexes[i].filepos < index->filepos))))
            index = &mkv_d->indexes[i];

      if (!index)
        return;

      mkv_d->cluster_size = mkv_d->blockgroup_size = 0;
      stream_seek (s, index->filepos);

      if (demuxer->video->id >= 0)
        mkv_d->v_skip_to_keyframe = 1;
      mkv_d->skip_to_timecode = index->timecode;
      mkv_d->a_skip_to_keyframe = 1;

      demux_mkv_fill_buffer(demuxer, NULL);
    }
}

static int
demux_mkv_control (demuxer_t *demuxer, int cmd, void *arg)
{
  mkv_demuxer_t *mkv_d = (mkv_demuxer_t *) demuxer->priv;
  
  switch (cmd)
    {
    case DEMUXER_CTRL_CORRECT_PTS:
      return DEMUXER_CTRL_OK;
    case DEMUXER_CTRL_GET_TIME_LENGTH:
      if (mkv_d->duration == 0)
        return DEMUXER_CTRL_DONTKNOW;

      *((double *)arg) = (double)mkv_d->duration;
      return DEMUXER_CTRL_OK;

    case DEMUXER_CTRL_GET_PERCENT_POS:
      if (mkv_d->duration == 0)
        {
            return DEMUXER_CTRL_DONTKNOW;
        }

      *((int *) arg) = (int) (100 * mkv_d->last_pts / mkv_d->duration);
      return DEMUXER_CTRL_OK; 

    case DEMUXER_CTRL_SWITCH_AUDIO:
      if (demuxer->audio && demuxer->audio->sh) {
        sh_audio_t *sh = demuxer->a_streams[demuxer->audio->id];
        int aid = *(int*)arg;
        if (aid < 0)
          aid = (sh->aid + 1) % mkv_d->last_aid;
        if (aid != sh->aid) {
          mkv_track_t *track = demux_mkv_find_track_by_num (mkv_d, aid, MATROSKA_TRACK_AUDIO);
          if (track) {
            demuxer->audio->id = track->tnum;
            sh = demuxer->a_streams[demuxer->audio->id];
            ds_free_packs(demuxer->audio);
          }
        }
        *(int*)arg = sh->aid;
      } else
        *(int*)arg = -2;
      return DEMUXER_CTRL_OK;

    default:
      return DEMUXER_CTRL_NOTIMPL;
    }
}

const demuxer_desc_t demuxer_desc_matroska = {
  "Matroska demuxer",
  "mkv",
  "Matroska",
  "Aurelien Jacobs",
  "",
  DEMUXER_TYPE_MATROSKA,
  1, // safe autodetect
  demux_mkv_open,
  demux_mkv_fill_buffer,
  NULL,
  demux_close_mkv,
  demux_mkv_seek,
  demux_mkv_control
};
