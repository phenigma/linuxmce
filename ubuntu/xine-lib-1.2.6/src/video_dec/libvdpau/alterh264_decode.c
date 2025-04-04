/* kate: space-indent on; indent-width 2; mixedindent off; indent-mode cstyle; remove-trailing-space on;
 * Copyright (C) 2008-2013 the xine project
 * Copyright (C) 2008 Christophe Thommeret <hftom@free.fr>
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * alterh264_decode.c, a H264 video stream parser using VDPAU hardware decoder
 *
 */

#include "alterh264_decode.h"


#define MAX_DPB_SIZE 16
#define MIN_BUFFER_SIZE 10000
#define MAX_BUFFER_SIZE 3145728

#define NAL_UNSPECIFIED 0
#define NAL_SLICE_NO_IDR 1
#define NAL_SLICE_IDR 5
#define NAL_SEI 6
#define NAL_SEQUENCE 7
#define NAL_PICTURE 8
#define NAL_ACCES 9
#define NAL_END_SEQUENCE 10
#define NAL_END_STREAM 11
#define NAL_SEQUENCE_EXT 13

#define SLICE_TYPE_P 0
#define SLICE_TYPE_B 1
#define SLICE_TYPE_I 2
#define SLICE_TYPE_SP 3
#define SLICE_TYPE_SI 4

#define START_IDR_FLAG 1000

#define MAX_POC 2147483647

#define DPB_DRAW_CLEAR   1
#define DPB_DRAW_REFS   2
#define DPB_DRAW_CURRENT 3

//#define MAKE_DAT /*do NOT define this, unless you know what you do */
#ifdef MAKE_DAT
static int nframes;
static FILE *outfile;
#endif


/*-------- DPB -------------------------------------------*/
static void
dpb_print (sequence_t * sequence)
{
  int i;
  dpb_frame_t *frame;
  uint32_t sf;

  for (i = 0; i < MAX_DPB_SIZE; i++)
  {
    frame = sequence->dpb[i];
    if (!frame->used)
      break;
    vo_frame_t *vo = (vo_frame_t *) frame->videoSurface;
    vdpau_accel_t *accel;
    if (vo)
      accel = (vdpau_accel_t *) vo->accel_data;
    sf = (vo) ? accel->surface : -1;
    fprintf (stderr,
	     "{ i:%d u:%d c:%d pn:%d-%d ir:%d-%d tpoc:%d bpoc:%d sf:%u }\n",
	     i, frame->used, frame->completed, frame->PicNum[0],
	     frame->PicNum[1], frame->is_reference[0], frame->is_reference[1],
	     frame->TopFieldOrderCnt, frame->BottomFieldOrderCnt, sf);
  }
}



static void
dpb_clear_all_pts (sequence_t * sequence)
{
  int i;

  for (i = 0; i < MAX_DPB_SIZE; i++)
  {
    if (!sequence->dpb[i]->used)
      break;
    sequence->dpb[i]->pts = 0;
  }
  sequence->cur_pic.pts = 0;
  sequence->cur_pic.drop_pts = 1;
}


static void
dpb_reset (sequence_t * sequence)
{
  int i;

  for (i = 0; i < MAX_DPB_SIZE; i++)
  {
    if (sequence->dpb[i]->videoSurface)
      sequence->dpb[i]->videoSurface->free (sequence->dpb[i]->videoSurface);
    memset (sequence->dpb[i], 0, sizeof (dpb_frame_t));
  }
  if (sequence->cur_pic.videoSurface && !sequence->cur_pic.is_reference[0]
      && !sequence->cur_pic.is_reference[1])
  {
    //fprintf(stderr, "freeing cur_pic\n");
    sequence->cur_pic.videoSurface->free (sequence->cur_pic.videoSurface);
  }
}



static void
dpb_remove (sequence_t * sequence, int index)
{
  lprintf ("|||||||||||||||||||||||||||||||||||||||| dbp_remove\n");
  int i;

  dpb_frame_t *frame = sequence->dpb[index];
  if (frame->videoSurface)
    frame->videoSurface->free (frame->videoSurface);
  memset (frame, 0, sizeof (dpb_frame_t));
  for (i = index; i < (MAX_DPB_SIZE - 1); i++)
  {
    sequence->dpb[i] = sequence->dpb[i + 1];
    if (!sequence->dpb[i]->used)
    {
      sequence->dpb[i + 1] = frame;
      break;
    }
  }
  if (i == (MAX_DPB_SIZE - 1))
    sequence->dpb[i] = frame;
}



static dpb_frame_t *
dpb_get_prev_ref (sequence_t * sequence)
{
  int i = MAX_DPB_SIZE - 1;

  while (i > -1)
  {
    if (sequence->dpb[i]->used)
      return sequence->dpb[i];
    --i;
  }

  return NULL;
}



static void
dpb_draw_frames (vdpau_h264_alter_decoder_t * this_gen, int32_t curpoc,
		 int draw_mode)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  int i, index = 0;
  int32_t poc, tpoc;
  dpb_frame_t *frame;

  while (index > -1)
  {
    index = -1;
    poc = curpoc;
    for (i = 0; i < MAX_DPB_SIZE; i++)
    {
      frame = seq->dpb[i];
      if (!frame->used)
	break;
      tpoc =
	(frame->TopFieldOrderCnt >
	 frame->BottomFieldOrderCnt) ? frame->TopFieldOrderCnt : frame->
	BottomFieldOrderCnt;
      if (!frame->videoSurface->drawn && (tpoc <= poc))
      {
	poc = tpoc;
	index = i;
      }
    }
    if ((index > -1) && (poc <= curpoc))
    {
      //fprintf(stderr,"|||||||||||||||||||||||||||||||||||||||| dpb_draw_frame = %d\n", poc);
      frame = seq->dpb[index];
      frame->videoSurface->pts = frame->pts;
      //fprintf(stderr,"H264 PTS = %llu\n", frame->pts);
      frame->videoSurface->top_field_first = frame->top_field_first;
      frame->videoSurface->draw (frame->videoSurface, this_gen->stream);
      frame->videoSurface->drawn++;
      if ((draw_mode != DPB_DRAW_CLEAR) && !frame->is_reference[0]
	  && !frame->is_reference[1])
	dpb_remove (seq, index);
    }
    else
      index = -1;
  }

  if (draw_mode == DPB_DRAW_CURRENT)
  {
    //fprintf(stderr,"|||||||||||||||||||||||||||||||||||||||| dpb_draw_frame = %d\n", curpoc);
    frame = &seq->cur_pic;
    frame->videoSurface->pts = frame->pts;
    //fprintf(stderr,"H264 PTS = %llu\n", frame->pts);
    frame->videoSurface->top_field_first = frame->top_field_first;
    frame->videoSurface->draw (frame->videoSurface, this_gen->stream);
    frame->videoSurface->free (frame->videoSurface);
  }
  else if (draw_mode == DPB_DRAW_CLEAR)
    dpb_reset (seq);
}



static dpb_frame_t *
dpb_get_PicNum (sequence_t * sequence, int32_t pic_num, int *index)
{
  dpb_frame_t *frame;
  int i = 0;

  for (i = 0; i < MAX_DPB_SIZE; i++)
  {
    frame = sequence->dpb[i];
    if (!frame->used)
      break;
    if ((frame->PicNum[0] == pic_num) || (frame->PicNum[1] == pic_num))
    {
      *index = i;
      return frame;
    }
  }
  return 0;
}



static void
dpb_mmc1 (vdpau_h264_alter_decoder_t * this_gen, int32_t picnum)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  int index;

  lprintf ("dpb_mmc1\n");

  dpb_frame_t *frame = dpb_get_PicNum (seq, picnum, &index);

  if (frame)
  {
    frame->is_reference[0] = frame->is_reference[1] = 0;
    if (frame->videoSurface->drawn)
      dpb_remove (seq, index);
    else
      dpb_draw_frames (this_gen,
		       (frame->TopFieldOrderCnt >
			frame->BottomFieldOrderCnt) ? frame->
		       TopFieldOrderCnt : frame->BottomFieldOrderCnt,
		       DPB_DRAW_REFS);
  }
}



static void
dbp_append (vdpau_h264_alter_decoder_t * this_gen, int second_field)
{
  sequence_t *sequence = (sequence_t *) & this_gen->sequence;
  int i, index = 0, refs = 0;
  int32_t fnw = MAX_POC;
  slice_param_t *sl = &sequence->slice_param;
  pic_param_t *pic = sequence->pic_param[sl->pic_parameter_set_id];
  seq_param_t *sp = sequence->seq_param[pic->seq_parameter_set_id];
  dpb_frame_t *tmp = 0, *cur_pic = &sequence->cur_pic;
  int max = sp->num_ref_frames ? sp->num_ref_frames : 1;
  max = (max > MAX_DPB_SIZE) ? MAX_DPB_SIZE : max;

#ifdef LOG
  vo_frame_t *vo = (vo_frame_t *) cur_pic->videoSurface;
  vdpau_accel_t *accel = (vdpau_accel_t *) vo->accel_data;
  lprintf
    ("|||||||||||||||||||||||||||||||||||||||| dbp_append surface = %d\n",
     accel->surface);
#endif

  if (second_field)
  {
    tmp = dpb_get_prev_ref (sequence);
    if (tmp)
    {
      memcpy (tmp, cur_pic, sizeof (dpb_frame_t));
      cur_pic->videoSurface = NULL;
    }
    else
      fprintf (stderr, "OOPS, no frame to store the second field ?!\n");
    return;
  }

  for (i = 0; i < MAX_DPB_SIZE; i++)
  {
    if (!sequence->dpb[i]->used)
      break;
    if (sequence->dpb[i]->FrameNumWrap < fnw)
    {
      fnw = sequence->dpb[i]->FrameNumWrap;
      index = i;
    }
    refs++;
  }

  if (refs >= max)
  {
    lprintf ("sliding window\n");
    tmp = sequence->dpb[index],
      tmp->is_reference[0] = tmp->is_reference[1] = 0;
    if (tmp->videoSurface->drawn)
      dpb_remove (sequence, index);
    else
      dpb_draw_frames (this_gen,
		       (tmp->TopFieldOrderCnt >
			tmp->BottomFieldOrderCnt) ? tmp->
		       TopFieldOrderCnt : tmp->BottomFieldOrderCnt,
		       DPB_DRAW_REFS);

    for (i = 0; i < MAX_DPB_SIZE; i++)
    {
      if (!sequence->dpb[i]->used)
	break;
    }
  }

  if (i < MAX_DPB_SIZE)
  {
    memcpy (sequence->dpb[i], cur_pic, sizeof (dpb_frame_t));
    if (!cur_pic->field_pic_flag)
      cur_pic->videoSurface = NULL;
  }
}

/*--------------------------------------------------------*/



static void
reset_slices (sequence_t * sequence)
{
  sequence->slices_count = 0;
  sequence->slice_mode = 0;
}



static void
reset_sequence (sequence_t * sequence)
{
  sequence->prevFrameNum = 0;
  sequence->prevFrameNumOffset = 0;
  sequence->prevMMC5 = 0;

  sequence->startup_frame = 0;
  sequence->reset = 0;
  sequence->chroma = 0;
  sequence->pic_pts = 0;
  sequence->bufpos = 0;
  sequence->bufseek = 0;
  sequence->start = -1;
  reset_slices (sequence);
  dpb_reset (sequence);
  memset (&sequence->cur_pic, 0, sizeof (dpb_frame_t));
  sequence->reset = VO_NEW_SEQUENCE_FLAG;
}



static void
set_ratio (sequence_t * seq, seq_param_t * sp)
{
  if (seq->mode_frame && seq->ratio)
    return;
  if (!seq->coded_height)
    seq->coded_height = 1;
  seq->ratio = (double) seq->coded_width / (double) seq->coded_height;
  if (sp->vui.aspect_ratio_info)
  {
    switch (sp->vui.aspect_ratio_idc)
    {
    case ASPECT_1_1:
      seq->ratio = 1 * seq->ratio;
      break;
    case ASPECT_12_11:
      seq->ratio *= 12.0 / 11.0;
      break;
    case ASPECT_10_11:
      seq->ratio *= 10.0 / 11.0;
      break;
    case ASPECT_16_11:
      seq->ratio *= 16.0 / 11.0;
      break;
    case ASPECT_40_33:
      seq->ratio *= 40.0 / 33.0;
      break;
    case ASPECT_24_11:
      seq->ratio *= 24.0 / 11.0;
      break;
    case ASPECT_20_11:
      seq->ratio *= 20.0 / 11.0;
      break;
    case ASPECT_32_11:
      seq->ratio *= 32.0 / 11.0;
      break;
    case ASPECT_80_33:
      seq->ratio *= 80.0 / 33.0;
      break;
    case ASPECT_18_11:
      seq->ratio *= 18.0 / 11.0;
      break;
    case ASPECT_15_11:
      seq->ratio *= 15.0 / 11.0;
      break;
    case ASPECT_64_33:
      seq->ratio *= 64.0 / 33.0;
      break;
    case ASPECT_160_99:
      seq->ratio *= 160.0 / 99.0;
      break;
    case ASPECT_4_3:
      seq->ratio *= 4.0 / 3.0;
      break;
    case ASPECT_3_2:
      seq->ratio *= 3.0 / 2.0;
      break;
    case ASPECT_2_1:
      seq->ratio *= 2.0 / 1.0;
      break;
    case ASPECT_EXTENDED_SAR:
      if (sp->vui.sar_height)
	seq->ratio *= (double) sp->vui.sar_width / sp->vui.sar_height;
      break;
    }
  }
}



static void
parse_scaling_list (bits_reader_t * br, uint8_t * scaling_list, int len,
		    int index)
{
  int last_scale = 8;
  int next_scale = 8;
  int32_t delta_scale;
  uint8_t use_default_scaling_matrix_flag = 0;
  int i;

  const uint8_t *zigzag = (len == 64) ? zigzag_8x8 : zigzag_4x4;

  for (i = 0; i < len; i++)
  {
    if (next_scale != 0)
    {
      delta_scale = read_exp_se (br);
      next_scale = (last_scale + delta_scale + 256) % 256;
      if (i == 0 && next_scale == 0)
      {
	use_default_scaling_matrix_flag = 1;
	break;
      }
    }
    scaling_list[zigzag[i]] = last_scale =
      (next_scale == 0) ? last_scale : next_scale;
  }

  if (use_default_scaling_matrix_flag)
  {
    switch (index)
    {
    case 0:
    case 1:
    case 2:
      {
	for (i = 0; i < sizeof (default_4x4_intra); i++)
	  scaling_list[zigzag_4x4[i]] = default_4x4_intra[i];
	break;
      }
    case 3:
    case 4:
    case 5:
      {
	for (i = 0; i < sizeof (default_4x4_inter); i++)
	  scaling_list[zigzag_4x4[i]] = default_4x4_inter[i];
	break;
      }
    case 6:
      {
	for (i = 0; i < sizeof (default_8x8_intra); i++)
	  scaling_list[zigzag_8x8[i]] = default_8x8_intra[i];
	break;
      }
    case 7:
      {
	for (i = 0; i < sizeof (default_8x8_inter); i++)
	  scaling_list[zigzag_8x8[i]] = default_8x8_inter[i];
	break;
      }
    }
  }
}



static void
scaling_list_fallback_A (uint8_t * scaling_lists_4x4,
			 uint8_t * scaling_lists_8x8, int i)
{
  int j;
  switch (i)
  {
  case 0:
    {
      for (j = 0; j < sizeof (default_4x4_intra); j++)
	scaling_lists_4x4[(i * 16) + zigzag_4x4[j]] = default_4x4_intra[j];
      break;
    }
  case 3:
    {
      for (j = 0; j < sizeof (default_4x4_inter); j++)
	scaling_lists_4x4[(i * 16) + zigzag_4x4[j]] = default_4x4_inter[j];
      break;
    }
  case 1:
  case 2:
  case 4:
  case 5:
    memcpy (&scaling_lists_4x4[i * 16], &scaling_lists_4x4[(i - 1) * 16],
	    6 * 16);
    break;
  case 6:
    {
      for (j = 0; j < sizeof (default_8x8_intra); j++)
	scaling_lists_8x8[(i - 6) * 64 + zigzag_8x8[j]] =
	  default_8x8_intra[j];
      break;
    }
  case 7:
    {
      for (j = 0; j < sizeof (default_8x8_inter); j++)
	scaling_lists_8x8[(i - 6) * 64 + zigzag_8x8[j]] =
	  default_8x8_inter[j];
      break;
    }

  }
}



static void
scaling_list_fallback_B (seq_param_t * sp, pic_param_t * pic, int i)
{
  switch (i)
  {
  case 0:
  case 3:
    memcpy (pic->scaling_lists_4x4[i], sp->scaling_lists_4x4[i],
	    sizeof (pic->scaling_lists_4x4[i]));
    break;
  case 1:
  case 2:
  case 4:
  case 5:
    memcpy (pic->scaling_lists_4x4[i], pic->scaling_lists_4x4[i - 1],
	    sizeof (pic->scaling_lists_4x4[i]));
    break;
  case 6:
  case 7:
    memcpy (pic->scaling_lists_8x8[i - 6], sp->scaling_lists_8x8[i - 6],
	    sizeof (pic->scaling_lists_8x8[i - 6]));
    break;
  }
}



static void
vui_parameters (sequence_t * seq, vui_param_t * vui)
{
  bits_reader_t *br = &seq->br;
  int xine_color_matrix = 4; /* undefined, mpeg range */

  vui->aspect_ratio_info = read_bits (br, 1);
  lprintf ("aspect_ratio_info_present_flag = %d\n", vui->aspect_ratio_info);
  if (vui->aspect_ratio_info)
  {
    vui->aspect_ratio_idc = read_bits (br, 8);
    lprintf ("aspect_ratio_idc = %d\n", vui->aspect_ratio_idc);
    if (vui->aspect_ratio_idc == 255)
    {
      vui->sar_width = read_bits (br, 16);
      lprintf ("sar_width = %d\n", vui->sar_width);
      vui->sar_height = read_bits (br, 16);
      lprintf ("sar_height = %d\n", vui->sar_height);
    }
  }
  if (read_bits (br, 1))	/* overscan_info_present_flag */
    skip_bits (br, 1);		/* overscan_appropriate_falg */
  if (read_bits (br, 1))
  {				/* video_signal_type_present_flag */
    skip_bits (br, 3);		/*video_format */
    xine_color_matrix |= read_bits (br, 1);  /*video_full_range_flag */
    vui->colour_desc = read_bits (br, 1);
    lprintf ("colour_desc = %d\n", vui->colour_desc);
    if (vui->colour_desc)
    {
      skip_bits (br, 8);        /* colour_primaries */
      skip_bits (br, 8);	/* transfer_characteristics */
      xine_color_matrix = (xine_color_matrix & 1) | (read_bits (br, 8) << 1);  /* matrix_coefficients */
    }
  }
  VO_SET_FLAGS_CM (xine_color_matrix, seq->color_matrix);
  if (read_bits (br, 1))
  {				/* chroma_loc_info_present_flag */
    read_exp_ue (br);		/* chroma_sample_loc_type_top_field */
    read_exp_ue (br);		/* chroma_sample_loc_type_bottom_field */
  }
  vui->timing_info = read_bits (br, 1);
  lprintf ("timing_info = %d\n", vui->timing_info);
  if (vui->timing_info)
  {
    vui->num_units_in_tick = read_bits (br, 32);
    lprintf ("num_units_in_tick = %u\n", vui->num_units_in_tick);
    vui->time_scale = read_bits (br, 32);
    lprintf ("time_scale = %u\n", vui->time_scale);
    if (vui->time_scale > 0)
      seq->video_step =
	180000. * (double) vui->num_units_in_tick / (double) vui->time_scale;
  }
}



static void
seq_parameter_set_data (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  seq_param_t *sp;
  int i;

  uint8_t profile_idc = read_bits (&seq->br, 8);
  lprintf ("profile_idc = %d\n", profile_idc);
  uint8_t constraint_set0_flag = read_bits (&seq->br, 1);
  lprintf ("constraint_set0_flag = %d\n", constraint_set0_flag);
  uint8_t constraint_set1_flag = read_bits (&seq->br, 1);
  lprintf ("constraint_set1_flag = %d\n", constraint_set1_flag);
  uint8_t constraint_set2_flag = read_bits (&seq->br, 1);
  lprintf ("constraint_set2_flag = %d\n", constraint_set2_flag);
  uint8_t constraint_set3_flag = read_bits (&seq->br, 1);
  lprintf ("constraint_set3_flag = %d\n", constraint_set3_flag);
  skip_bits (&seq->br, 4);
  uint8_t level_idc = read_bits (&seq->br, 8);
  lprintf ("level_idc = %d\n", level_idc);

  uint8_t seq_parameter_set_id = read_exp_ue (&seq->br);
  lprintf ("seq_parameter_set_id = %d\n", seq_parameter_set_id);
  if (seq_parameter_set_id > 31)
  {
    lprintf ("OOPS : seq_parameter_set_id > 31 !!\n");
    return;
  }

  if (!seq->seq_param[seq_parameter_set_id])
    seq->seq_param[seq_parameter_set_id] =
      (seq_param_t *) calloc (1, sizeof (seq_param_t));
  if (!seq->seq_param[seq_parameter_set_id])
  {
    lprintf ("OOPS : can't allocate SPS %d !!\n", seq_parameter_set_id);
    return;
  }

  sp = seq->seq_param[seq_parameter_set_id];
  sp->profile_idc = profile_idc;
  switch (profile_idc)
  {
  case 100:
    seq->profile = VDP_DECODER_PROFILE_H264_HIGH;
    break;
  case 77:
    seq->profile = VDP_DECODER_PROFILE_H264_MAIN;
    break;
  case 66:			/* nvidia's vdpau doesn't suppot baseline, force main */
  default:
    seq->profile = VDP_DECODER_PROFILE_H264_MAIN;
  }
  sp->constraint_set0_flag = constraint_set0_flag;
  sp->constraint_set1_flag = constraint_set1_flag;
  sp->constraint_set2_flag = constraint_set2_flag;
  sp->constraint_set3_flag = constraint_set3_flag;
  sp->level_idc = level_idc;

  memset (&sp->scaling_lists_4x4, 16, sizeof (sp->scaling_lists_4x4));
  memset (&sp->scaling_lists_8x8, 16, sizeof (sp->scaling_lists_8x8));

  sp->chroma_format_idc = 1;
  sp->separate_colour_plane_flag = 0;
  if (sp->profile_idc == 100 || sp->profile_idc == 110
      || sp->profile_idc == 122 || sp->profile_idc == 244
      || sp->profile_idc == 44 || sp->profile_idc == 83
      || sp->profile_idc == 86)
  {
    sp->chroma_format_idc = read_exp_ue (&seq->br);
    lprintf ("chroma_format_idc = %u\n", sp->chroma_format_idc);
    if (sp->chroma_format_idc == 3)
    {
      sp->separate_colour_plane_flag = read_bits (&seq->br, 1);
      lprintf ("separate_colour_plane_flag = %d\n",
	       sp->separate_colour_plane_flag);
    }
    sp->bit_depth_luma_minus8 = read_exp_ue (&seq->br);
    lprintf ("bit_depth_luma_minus8 = %u\n", sp->bit_depth_luma_minus8);
    sp->bit_depth_chroma_minus8 = read_exp_ue (&seq->br);
    lprintf ("bit_depth_chroma_minus8 = %u\n", sp->bit_depth_chroma_minus8);
    sp->qpprime_y_zero_transform_bypass_flag = read_bits (&seq->br, 1);
    lprintf ("qpprime_y_zero_transform_bypass_flag = %u\n",
	     sp->qpprime_y_zero_transform_bypass_flag);
    sp->seq_scaling_matrix_present_flag = read_bits (&seq->br, 1);
    lprintf ("seq_scaling_matrix_present_flag = %u\n",
	     sp->seq_scaling_matrix_present_flag);
    if (sp->seq_scaling_matrix_present_flag)
    {
      for (i = 0; i < 8; i++)
      {
	int scaling_flag = read_bits (&seq->br, 1);
	if (scaling_flag)
	{
	  if (i < 6)
	    parse_scaling_list (&seq->br, &sp->scaling_lists_4x4[i][0], 16,
				i);
	  else
	    parse_scaling_list (&seq->br, &sp->scaling_lists_8x8[i - 6][0],
				64, i);
	}
	else
	  scaling_list_fallback_A ((uint8_t *) sp->scaling_lists_4x4,
				   (uint8_t *) sp->scaling_lists_8x8, i);
      }
    }
  }
  sp->log2_max_frame_num_minus4 = read_exp_ue (&seq->br);
  lprintf ("log2_max_frame_num_minus4 = %u\n", sp->log2_max_frame_num_minus4);
  sp->pic_order_cnt_type = read_exp_ue (&seq->br);
  lprintf ("pic_order_cnt_type = %u\n", sp->pic_order_cnt_type);
  if (sp->pic_order_cnt_type == 0)
  {
    sp->log2_max_pic_order_cnt_lsb_minus4 = read_exp_ue (&seq->br);
    lprintf ("log2_max_pic_order_cnt_lsb_minus4 = %u\n",
	     sp->log2_max_pic_order_cnt_lsb_minus4);
  }
  else if (sp->pic_order_cnt_type == 1)
  {
    sp->delta_pic_order_always_zero_flag = read_bits (&seq->br, 1);
    lprintf ("delta_pic_order_always_zero_flag = %u\n",
	     sp->delta_pic_order_always_zero_flag);
    sp->offset_for_non_ref_pic = read_exp_se (&seq->br);
    lprintf ("offset_for_non_ref_pic = %d\n", sp->offset_for_non_ref_pic);
    sp->offset_for_top_to_bottom_field = read_exp_se (&seq->br);
    lprintf ("offset_for_top_to_bottom_field = %d\n",
	     sp->offset_for_top_to_bottom_field);
    sp->num_ref_frames_in_pic_order_cnt_cycle = read_exp_ue (&seq->br);
    lprintf ("num_ref_frames_in_pic_order_cnt_cycle = %u\n",
	     sp->num_ref_frames_in_pic_order_cnt_cycle);
    for (i = 0; i < sp->num_ref_frames_in_pic_order_cnt_cycle; i++)
    {
      sp->offset_for_ref_frame[i] = read_exp_se (&seq->br);
      lprintf ("offset_for_ref_frame[%d] = %d\n", i,
	       sp->offset_for_ref_frame[i]);
    }
  }
  sp->num_ref_frames = read_exp_ue (&seq->br);
  if (sp->num_ref_frames > 16)
    sp->num_ref_frames = 16;
  lprintf ("num_ref_frames = %u\n", sp->num_ref_frames);
  sp->gaps_in_frame_num_value_allowed_flag = read_bits (&seq->br, 1);
  lprintf ("gaps_in_frame_num_value_allowed_flag = %u\n",
	   sp->gaps_in_frame_num_value_allowed_flag);
  sp->pic_width_in_mbs_minus1 = read_exp_ue (&seq->br);
  lprintf ("pic_width_in_mbs_minus1 = %u\n", sp->pic_width_in_mbs_minus1);
  sp->pic_height_in_map_units_minus1 = read_exp_ue (&seq->br);
  lprintf ("pic_height_in_map_units_minus1 = %u\n",
	   sp->pic_height_in_map_units_minus1);
  sp->frame_mbs_only_flag = read_bits (&seq->br, 1);
  lprintf ("frame_mbs_only_flag = %u\n", sp->frame_mbs_only_flag);

  seq->coded_width = (sp->pic_width_in_mbs_minus1 + 1) * 16;
  seq->coded_height =
    (2 - sp->frame_mbs_only_flag) * (sp->pic_height_in_map_units_minus1 +
				     1) * 16;

  if (!sp->frame_mbs_only_flag)
  {
    sp->mb_adaptive_frame_field_flag = read_bits (&seq->br, 1);
    lprintf ("mb_adaptive_frame_field_flag = %u\n",
	     sp->mb_adaptive_frame_field_flag);
  }
  else
    sp->mb_adaptive_frame_field_flag = 0;
  sp->direct_8x8_inference_flag = read_bits (&seq->br, 1);
  lprintf ("direct_8x8_inference_flag = %u\n", sp->direct_8x8_inference_flag);
  sp->frame_cropping_flag = read_bits (&seq->br, 1);
  lprintf ("frame_cropping_flag = %u\n", sp->frame_cropping_flag);
  if (sp->frame_cropping_flag)
  {
    sp->frame_crop_left_offset = read_exp_ue (&seq->br);
    lprintf ("frame_crop_left_offset = %u\n", sp->frame_crop_left_offset);
    sp->frame_crop_right_offset = read_exp_ue (&seq->br);
    lprintf ("frame_crop_right_offset = %u\n", sp->frame_crop_right_offset);
    sp->frame_crop_top_offset = read_exp_ue (&seq->br);
    lprintf ("frame_crop_top_offset = %u\n", sp->frame_crop_top_offset);
    sp->frame_crop_bottom_offset = read_exp_ue (&seq->br);
    lprintf ("frame_crop_bottom_offset = %u\n", sp->frame_crop_bottom_offset);
    seq->coded_height -=
      (2 - sp->frame_mbs_only_flag) * 2 * sp->frame_crop_bottom_offset;
  }
  if (seq->coded_height == 1088)
    seq->coded_height = 1080;
  sp->vui_parameters_present_flag = read_bits (&seq->br, 1);
  lprintf ("vui_parameters_present_flag = %u\n",
	   sp->vui_parameters_present_flag);
  if (sp->vui_parameters_present_flag)
    vui_parameters (seq, &sp->vui);
  set_ratio (seq, sp);
}



static void
pic_parameter_set (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  pic_param_t *pic;
  seq_param_t *sp;
  int i;

  uint8_t pic_parameter_set_id = read_exp_ue (&seq->br);
  lprintf ("pic_parameter_set_id = %u\n", pic_parameter_set_id);
  if (!seq->pic_param[pic_parameter_set_id])
    seq->pic_param[pic_parameter_set_id] =
      (pic_param_t *) calloc (1, sizeof (pic_param_t));
  if (!seq->pic_param[pic_parameter_set_id])
  {
    lprintf ("OOPS : can't allocate PPS %d !!\n", pic_parameter_set_id);
    return;
  }
  pic = seq->pic_param[pic_parameter_set_id];

  uint8_t seq_parameter_set_id = read_exp_ue (&seq->br);
  lprintf ("seq_parameter_set_id = %u\n", seq_parameter_set_id);
  if (seq_parameter_set_id > 31)
  {
    lprintf ("OOPS : referenced SPS (%d) does not exist !!\n",
	     seq_parameter_set_id);
    return;
  }
  if (!seq->seq_param[seq_parameter_set_id])
  {
    lprintf ("OOPS : referenced SPS (%d) does not exist !!\n",
	     seq_parameter_set_id);
    return;
  }

  pic->seq_parameter_set_id = seq_parameter_set_id;
  sp = seq->seq_param[pic->seq_parameter_set_id];
  pic->entropy_coding_mode_flag = read_bits (&seq->br, 1);
  lprintf ("entropy_coding_mode_flag = %u\n", pic->entropy_coding_mode_flag);
  pic->pic_order_present_flag = read_bits (&seq->br, 1);
  lprintf ("pic_order_present_flag = %u\n", pic->pic_order_present_flag);
  uint8_t num_slice_groups_minus1 = read_exp_ue (&seq->br);
  lprintf ("num_slice_groups_minus1 = %u\n", num_slice_groups_minus1);
  if (num_slice_groups_minus1 > 0)
  {
    uint8_t slice_group_map_type = read_exp_ue (&seq->br);
    lprintf ("slice_group_map_type = %u\n", slice_group_map_type);
    if (!slice_group_map_type)
    {
      for (i = 0; i < num_slice_groups_minus1; i++)
	read_exp_ue (&seq->br);
    }
    else if (slice_group_map_type == 2)
    {
      for (i = 0; i < num_slice_groups_minus1; i++)
      {
	read_exp_ue (&seq->br);
	read_exp_ue (&seq->br);
      }
    }
    else if (slice_group_map_type == 3 || slice_group_map_type == 4
	     || slice_group_map_type == 5)
    {
      read_bits (&seq->br, 1);
      read_exp_ue (&seq->br);
    }
    else if (slice_group_map_type == 6)
    {
      read_exp_ue (&seq->br);
    }
  }
  pic->num_ref_idx_l0_active_minus1 = read_exp_ue (&seq->br);
  lprintf ("num_ref_idx_l0_active_minus1 = %u\n",
	   pic->num_ref_idx_l0_active_minus1);
  pic->num_ref_idx_l1_active_minus1 = read_exp_ue (&seq->br);
  lprintf ("num_ref_idx_l1_active_minus1 = %u\n",
	   pic->num_ref_idx_l1_active_minus1);
  pic->weighted_pred_flag = read_bits (&seq->br, 1);
  lprintf ("weighted_pred_flag = %u\n", pic->weighted_pred_flag);
  pic->weighted_bipred_idc = read_bits (&seq->br, 2);
  lprintf ("weighted_bipred_idc = %u\n", pic->weighted_bipred_idc);
  pic->pic_init_qp_minus26 = read_exp_se (&seq->br);
  lprintf ("pic_init_qp_minus26 = %d\n", pic->pic_init_qp_minus26);
  pic->pic_init_qs_minus26 = read_exp_se (&seq->br);
  lprintf ("pic_init_qs_minus26 = %d\n", pic->pic_init_qs_minus26);
  pic->chroma_qp_index_offset = read_exp_se (&seq->br);
  lprintf ("chroma_qp_index_offset = %d\n", pic->chroma_qp_index_offset);
  pic->deblocking_filter_control_present_flag = read_bits (&seq->br, 1);
  lprintf ("deblocking_filter_control_present_flag = %u\n",
	   pic->deblocking_filter_control_present_flag);
  pic->constrained_intra_pred_flag = read_bits (&seq->br, 1);
  lprintf ("constrained_intra_pred_flag = %u\n",
	   pic->constrained_intra_pred_flag);
  pic->redundant_pic_cnt_present_flag = read_bits (&seq->br, 1);
  lprintf ("redundant_pic_cnt_present_flag = %u\n",
	   pic->redundant_pic_cnt_present_flag);

  uint32_t more = more_rbsp_data (&seq->br);
  lprintf ("more bits = %u (buflen = %d) (still = %d)\n", more,
	   seq->br.length, seq->br.start + seq->br.length - seq->br.buffer);
  if (more)
  {
    pic->transform_8x8_mode_flag = read_bits (&seq->br, 1);
    lprintf ("transform_8x8_mode_flag = %u\n", pic->transform_8x8_mode_flag);
    pic->pic_scaling_matrix_present_flag = read_bits (&seq->br, 1);
    lprintf ("pic_scaling_matrix_present_flag = %u\n",
	     pic->pic_scaling_matrix_present_flag);
    if (pic->pic_scaling_matrix_present_flag)
    {
      for (i = 0; i < 8; i++)
      {
	if (i < 6 || pic->transform_8x8_mode_flag)
	  pic->pic_scaling_list_present_flag[i] = read_bits (&seq->br, 1);
	else
	  pic->pic_scaling_list_present_flag[i] = 0;

	if (pic->pic_scaling_list_present_flag[i])
	{
	  if (i < 6)
	    parse_scaling_list (&seq->br, &pic->scaling_lists_4x4[i][0], 16,
				i);
	  else
	    parse_scaling_list (&seq->br, &pic->scaling_lists_8x8[i - 6][0],
				64, i);
	}
	else
	{
	  if (!sp->seq_scaling_matrix_present_flag)
	    scaling_list_fallback_A ((uint8_t *) pic->scaling_lists_4x4,
				     (uint8_t *) pic->scaling_lists_8x8, i);
	  else
	    scaling_list_fallback_B (sp, pic, i);
	}
      }
    }
    pic->second_chroma_qp_index_offset = read_exp_se (&seq->br);
    lprintf ("second_chroma_qp_index_offset = %d\n",
	     pic->second_chroma_qp_index_offset);
  }
  else
  {
    pic->transform_8x8_mode_flag = 0;
    pic->pic_scaling_matrix_present_flag = 0;
    pic->second_chroma_qp_index_offset = pic->chroma_qp_index_offset;
  }
}



static void
pred_weight_table (vdpau_h264_alter_decoder_t * this_gen, uint8_t slice_type,
		   uint8_t ChromaArrayType, uint8_t l0, uint8_t l1)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  int i;

  read_exp_ue (&seq->br);
  if (ChromaArrayType)
    read_exp_ue (&seq->br);
  for (i = 0; i <= l0; i++)
  {
    if (read_bits (&seq->br, 1))
    {
      read_exp_se (&seq->br);
      read_exp_se (&seq->br);
    }
    if (ChromaArrayType && read_bits (&seq->br, 1))
    {
      read_exp_se (&seq->br);
      read_exp_se (&seq->br);
      read_exp_se (&seq->br);
      read_exp_se (&seq->br);
    }
  }
  if (slice_type == SLICE_TYPE_B)
  {
    for (i = 0; i <= l1; i++)
    {
      if (read_bits (&seq->br, 1))
      {
	read_exp_se (&seq->br);
	read_exp_se (&seq->br);
      }
      if (ChromaArrayType)
      {
	if (read_bits (&seq->br, 1))
	{
	  read_exp_se (&seq->br);
	  read_exp_se (&seq->br);
	  read_exp_se (&seq->br);
	  read_exp_se (&seq->br);
	}
      }
    }
  }
}



static void
ref_pic_list_reordering (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;

  if ((sl->slice_type != SLICE_TYPE_I) && (sl->slice_type != SLICE_TYPE_SI))
  {
    if (read_bits (&seq->br, 1))
    {
      uint32_t tmp/*, diff*/;
      do
      {
	tmp = read_exp_ue (&seq->br);
	if (tmp == 0 || tmp == 1)
	  /*diff =*/ read_exp_ue (&seq->br);
	else if (tmp == 2)
          /*diff =*/ read_exp_ue (&seq->br);
      }
      while (tmp != 3 && !seq->br.oflow);
    }
  }
  if (sl->slice_type == SLICE_TYPE_B)
  {
    if (read_bits (&seq->br, 1))
    {
      uint32_t tmp2/*, diff2*/;
      do
      {
	tmp2 = read_exp_ue (&seq->br);
	if (tmp2 == 0 || tmp2 == 1)
	  /*diff2 =*/ read_exp_ue (&seq->br);
	else if (tmp2 == 2)
	  /*diff2 =*/ read_exp_ue (&seq->br);
      }
      while (tmp2 != 3 && !seq->br.oflow);
    }
  }
}



static void
dec_ref_pic_marking (vdpau_h264_alter_decoder_t * this_gen, uint8_t idr)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  int32_t pic_num;

  if (idr)
  {
#ifdef LOG
    uint8_t no_output_of_prior_pics_flag = read_bits (&seq->br, 1);
    lprintf ("no_output_of_prior_pics_flag = %u\n",
	     no_output_of_prior_pics_flag);
    uint8_t long_term_reference_flag = read_bits (&seq->br, 1);
    lprintf ("long_term_reference_flag = %u\n", long_term_reference_flag);
#else
    skip_bits (&seq->br, 2);
#endif
  }
  else
  {
    uint8_t adaptive_ref_pic_marking_mode_flag = read_bits (&seq->br, 1);
    lprintf ("adaptive_ref_pic_marking_mode_flag = %u\n",
	     adaptive_ref_pic_marking_mode_flag);
    if (!adaptive_ref_pic_marking_mode_flag)
    {
      if (seq->cur_pic.field_pic_flag
	  && (seq->cur_pic.completed == PICTURE_DONE)
	  && (seq->cur_pic.is_reference[0] || seq->cur_pic.is_reference[1]))
      {
	seq->cur_pic.is_reference[0] = seq->cur_pic.is_reference[1] =
	  SHORT_TERM_REF;
	lprintf ("short_ref marking\n");
      }
      // sliding window is always performed in dpb_append()
    }
    else
    {
      uint8_t memory_management_control_operation;
      do
      {
	memory_management_control_operation = read_exp_ue (&seq->br);
	lprintf ("memory_management_control_operation = %u\n",
		 memory_management_control_operation);
	if (memory_management_control_operation == 1
	    || memory_management_control_operation == 3)
	{
	  uint32_t difference_of_pic_nums_minus1 = read_exp_ue (&seq->br);
	  lprintf ("difference_of_pic_nums_minus1 = %u\n",
		   difference_of_pic_nums_minus1);
	  pic_num =
	    seq->cur_pic.PicNum[0] - (difference_of_pic_nums_minus1 + 1);
	  dpb_mmc1 (this_gen, pic_num);
	}
	if (memory_management_control_operation == 2)
	{
#ifdef LOG
	  uint32_t long_term_pic_num = read_exp_ue (&seq->br);
	  lprintf ("long_term_pic_num = %u\n", long_term_pic_num);
#else
          read_exp_ue (&seq->br);
#endif
	}
	if (memory_management_control_operation == 3
	    || memory_management_control_operation == 6)
	{
#ifdef LOG
	  uint32_t long_term_frame_idx = read_exp_ue (&seq->br);
	  lprintf ("long_term_frame_idx = %u\n", long_term_frame_idx);
#else
          read_exp_ue (&seq->br);
#endif
	}
	if (memory_management_control_operation == 4)
	{
#ifdef LOG
	  uint32_t max_long_term_frame_idx_plus1 = read_exp_ue (&seq->br);
	  lprintf ("max_long_term_frame_idx_plus1 = %u\n",
		   max_long_term_frame_idx_plus1);
#else
          read_exp_ue (&seq->br);
#endif
	}
      }
      while (memory_management_control_operation && !seq->br.oflow);
    }
  }
}



static void
slice_header (vdpau_h264_alter_decoder_t * this_gen, uint8_t nal_ref_idc,
	      uint8_t nal_unit_type)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;
  pic_param_t *pic;
  seq_param_t *sp;

  sl->nal_ref_idc = nal_ref_idc;
  sl->nal_unit_type = nal_unit_type;

  read_exp_ue (&seq->br);	/* first_mb_in_slice */
  sl->slice_type = read_exp_ue (&seq->br) % 5;
  lprintf ("slice_type = %u\n", sl->slice_type);
  sl->pic_parameter_set_id = read_exp_ue (&seq->br);
  lprintf ("pic_parameter_set_id = %u\n", sl->pic_parameter_set_id);
  if (!seq->pic_param[sl->pic_parameter_set_id])
  {
    lprintf ("OOPS : referenced PPS (%d) does not exist !!\n",
	     sl->pic_parameter_set_id);
    seq->cur_pic.missing_header = 1;
    return;
  }
  pic = seq->pic_param[sl->pic_parameter_set_id];
  if (!seq->seq_param[pic->seq_parameter_set_id])
  {
    lprintf ("OOPS : referenced SPS (%d) does not exist !!\n",
	     pic->seq_parameter_set_id);
    seq->cur_pic.missing_header = 1;
    return;
  }

  if (!seq->startup_frame && (sl->slice_type == SLICE_TYPE_I)
      && !seq->cur_pic.completed)
    seq->startup_frame = 1;

  lprintf ("seq_parameter_set_id = %u\n", pic->seq_parameter_set_id);
  sp = seq->seq_param[pic->seq_parameter_set_id];
  if (sp->separate_colour_plane_flag)
    read_bits (&seq->br, 2);	/* colour_plane_id */
  sl->frame_num = read_bits (&seq->br, sp->log2_max_frame_num_minus4 + 4);
  lprintf ("frame_num = %u\n", sl->frame_num);
  sl->MaxFrameNum = 1 << (sp->log2_max_frame_num_minus4 + 4);

  sl->field_pic_flag = sl->bottom_field_flag =
    sl->delta_pic_order_cnt_bottom = 0;
  sl->delta_pic_order_cnt[0] = sl->delta_pic_order_cnt[1] = 0;

  if (!sp->frame_mbs_only_flag)
  {
    sl->field_pic_flag = read_bits (&seq->br, 1);
    lprintf ("field_pic_flag = %u\n", sl->field_pic_flag);
    if (sl->field_pic_flag)
    {
      sl->bottom_field_flag = read_bits (&seq->br, 1);
      lprintf ("bottom_field_flag = %u\n", sl->bottom_field_flag);
    }
  }
  if (nal_unit_type == NAL_SLICE_IDR)
  {
    sl->idr_pic_id = read_exp_ue (&seq->br);
    lprintf ("idr_pic_id = %u\n", sl->idr_pic_id);
  }
  if (sp->pic_order_cnt_type == 0)
  {
    sl->pic_order_cnt_lsb =
      read_bits (&seq->br, sp->log2_max_pic_order_cnt_lsb_minus4 + 4);
    lprintf ("pic_order_cnt_lsb = %u\n", sl->pic_order_cnt_lsb);
    if (pic->pic_order_present_flag && !sl->field_pic_flag)
    {
      sl->delta_pic_order_cnt_bottom = read_exp_se (&seq->br);
      lprintf ("delta_pic_order_cnt_bottom = %d\n",
	       sl->delta_pic_order_cnt_bottom);
    }
  }
  if (sp->pic_order_cnt_type == 1 && !sp->delta_pic_order_always_zero_flag)
  {
    sl->delta_pic_order_cnt[0] = read_exp_se (&seq->br);
    lprintf ("delta_pic_order_cnt[0] = %d\n", sl->delta_pic_order_cnt[0]);
    if (pic->pic_order_present_flag && !sl->field_pic_flag)
    {
      sl->delta_pic_order_cnt[1] = read_exp_se (&seq->br);
      lprintf ("delta_pic_order_cnt[1] = %d\n", sl->delta_pic_order_cnt[1]);
    }
  }
  if (pic->redundant_pic_cnt_present_flag)
  {
    sl->redundant_pic_cnt = read_exp_ue (&seq->br);
    lprintf ("redundant_pic_cnt = %u\n", sl->redundant_pic_cnt);
  }
  if (sl->slice_type == SLICE_TYPE_B)
    skip_bits (&seq->br, 1);	/* direct_spatial_mv_pred_flag */

  sl->num_ref_idx_l0_active_minus1 = pic->num_ref_idx_l0_active_minus1;
  sl->num_ref_idx_l1_active_minus1 = pic->num_ref_idx_l1_active_minus1;

  if (sl->slice_type == SLICE_TYPE_P || sl->slice_type == SLICE_TYPE_SP
      || sl->slice_type == SLICE_TYPE_B)
  {
    if (read_bits (&seq->br, 1))
    {
      lprintf ("num_ref_idx_active_override_flag = 1\n");
      sl->num_ref_idx_l0_active_minus1 = read_exp_ue (&seq->br);
      if (sl->slice_type == SLICE_TYPE_B)
	sl->num_ref_idx_l1_active_minus1 = read_exp_ue (&seq->br);
      lprintf ("num_ref_idx_l0_active_minus1 = %u\n",
	       sl->num_ref_idx_l0_active_minus1);
      lprintf ("num_ref_idx_l1_active_minus1 = %u\n",
	       sl->num_ref_idx_l1_active_minus1);
    }
  }
}



static void
slice_header_post (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;

  if (!sl->nal_ref_idc)
    return;

  pic_param_t *pic = seq->pic_param[sl->pic_parameter_set_id];
  seq_param_t *sp = seq->seq_param[pic->seq_parameter_set_id];

  if ((pic->weighted_pred_flag
       && ((sl->slice_type == SLICE_TYPE_P)
	   || (sl->slice_type == SLICE_TYPE_SP)))
      || ((pic->weighted_bipred_idc == 1)
	  && (sl->slice_type == SLICE_TYPE_B)))
  {
    uint8_t chroma =
      (sp->separate_colour_plane_flag) ? 0 : sp->chroma_format_idc;
    pred_weight_table (this_gen, sl->slice_type, chroma,
		       sl->num_ref_idx_l0_active_minus1,
		       sl->num_ref_idx_l1_active_minus1);
  }

  dec_ref_pic_marking (this_gen, (sl->nal_unit_type == 5) ? 1 : 0);
}



static void
decode_poc (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;
  pic_param_t *pic = seq->pic_param[sl->pic_parameter_set_id];
  seq_param_t *sp = seq->seq_param[pic->seq_parameter_set_id];
  int parity = sl->bottom_field_flag ? 1 : 0;

  seq->cur_pic.used = 1;
  seq->cur_pic.FrameNum = sl->frame_num;
  seq->cur_pic.is_reference[parity] = sl->nal_ref_idc;
  seq->cur_pic.field_pic_flag = sl->field_pic_flag;

  if (sl->field_pic_flag)
  {
    if (!seq->cur_pic.completed)
      seq->cur_pic.top_field_first = !parity;
    seq->cur_pic.completed |=
      (parity ? PICTURE_BOTTOM_DONE : PICTURE_TOP_DONE);
  }
  else
  {
    seq->cur_pic.is_reference[!parity] = seq->cur_pic.is_reference[parity];
    seq->cur_pic.completed = PICTURE_DONE;
  }

  if (sp->pic_order_cnt_type == 0)
  {
    dpb_frame_t *prev_pic = dpb_get_prev_ref (seq);
    int32_t prevPicOrderCntMsb, prevPicOrderCntLsb;
    uint32_t MaxPicOrderCntLsb =
      1 << (sp->log2_max_pic_order_cnt_lsb_minus4 + 4);

    seq->cur_pic.pic_order_cnt_lsb = sl->pic_order_cnt_lsb;
    seq->cur_pic.top_field_first =
      (sl->delta_pic_order_cnt_bottom < 0) ? 0 : 1;

    if (!prev_pic)
    {
      seq->cur_pic.PicOrderCntMsb = seq->cur_pic.TopFieldOrderCnt =
	seq->cur_pic.BottomFieldOrderCnt = 0;
      return;
    }
    if (sl->nal_unit_type == NAL_SLICE_IDR)
      prevPicOrderCntMsb = prevPicOrderCntLsb = 0;
    else if (prev_pic->mmc5)
    {
      if (!sl->bottom_field_flag)
      {
	prevPicOrderCntMsb = 0;
	prevPicOrderCntLsb = prev_pic->TopFieldOrderCnt;
      }
      else
	prevPicOrderCntMsb = prevPicOrderCntLsb = 0;
    }
    else
    {
      prevPicOrderCntMsb = prev_pic->PicOrderCntMsb;
      prevPicOrderCntLsb = prev_pic->pic_order_cnt_lsb;
    }

    if ((sl->pic_order_cnt_lsb < prevPicOrderCntLsb)
	&& ((prevPicOrderCntLsb - sl->pic_order_cnt_lsb) >=
	    (MaxPicOrderCntLsb / 2)))
      seq->cur_pic.PicOrderCntMsb = prevPicOrderCntMsb + MaxPicOrderCntLsb;
    else if ((sl->pic_order_cnt_lsb > prevPicOrderCntLsb)
	     && ((sl->pic_order_cnt_lsb - prevPicOrderCntLsb) >
		 (MaxPicOrderCntLsb / 2)))
      seq->cur_pic.PicOrderCntMsb = prevPicOrderCntMsb - MaxPicOrderCntLsb;
    else
      seq->cur_pic.PicOrderCntMsb = prevPicOrderCntMsb;

    if (!sl->field_pic_flag)
    {
      seq->cur_pic.TopFieldOrderCnt =
	seq->cur_pic.PicOrderCntMsb + sl->pic_order_cnt_lsb;
      seq->cur_pic.BottomFieldOrderCnt =
	seq->cur_pic.TopFieldOrderCnt + sl->delta_pic_order_cnt_bottom;
    }
    else
    {
      if (sl->bottom_field_flag)
	seq->cur_pic.BottomFieldOrderCnt =
	  seq->cur_pic.PicOrderCntMsb + sl->pic_order_cnt_lsb;
      else
	seq->cur_pic.TopFieldOrderCnt =
	  seq->cur_pic.PicOrderCntMsb + sl->pic_order_cnt_lsb;
    }
  }
  else
  {
    int16_t FrameNumOffset, prevFrameNumOffset;
    uint16_t MaxFrameNum = 1 << (sp->log2_max_frame_num_minus4 + 4);

    if (sl->nal_unit_type == NAL_SLICE_IDR)
    {
      FrameNumOffset = 0;
    }
    else
    {
      if (seq->prevMMC5)
	prevFrameNumOffset = 0;
      else
	prevFrameNumOffset = seq->prevFrameNumOffset;

      if (seq->prevFrameNum > sl->frame_num)
	FrameNumOffset = prevFrameNumOffset + MaxFrameNum;
      else
	FrameNumOffset = prevFrameNumOffset;
    }

    if (sp->pic_order_cnt_type == 1)
    {
      int16_t absFrameNum = 0, picOrderCntCycleCnt =
	0, frameNumInPicOrderCntCycle = 0, expectedDeltaPerPicOrderCntCycle =
	0, expectedPicOrderCnt = 0;
      int i;
      if (sp->num_ref_frames_in_pic_order_cnt_cycle)
	absFrameNum = FrameNumOffset + sl->frame_num;
      if (!sl->nal_ref_idc && (absFrameNum > 0))
	--absFrameNum;

      for (i = 0; i < sp->num_ref_frames_in_pic_order_cnt_cycle; i++)
	expectedDeltaPerPicOrderCntCycle += sp->offset_for_ref_frame[i];

      if (absFrameNum > 0)
      {
	picOrderCntCycleCnt =
	  (absFrameNum - 1) / sp->num_ref_frames_in_pic_order_cnt_cycle;
	frameNumInPicOrderCntCycle =
	  (absFrameNum - 1) % sp->num_ref_frames_in_pic_order_cnt_cycle;
	expectedPicOrderCnt =
	  picOrderCntCycleCnt * expectedDeltaPerPicOrderCntCycle;
	for (i = 0; i < frameNumInPicOrderCntCycle; i++)
	  expectedPicOrderCnt += sp->offset_for_ref_frame[i];
      }
      if (!sl->nal_ref_idc)
	expectedPicOrderCnt += sp->offset_for_non_ref_pic;

      if (!sl->field_pic_flag)
      {
	seq->cur_pic.TopFieldOrderCnt =
	  expectedPicOrderCnt + sl->delta_pic_order_cnt[0];
	seq->cur_pic.BottomFieldOrderCnt =
	  seq->cur_pic.TopFieldOrderCnt + sp->offset_for_top_to_bottom_field +
	  sl->delta_pic_order_cnt[1];
      }
      else if (!sl->bottom_field_flag)
	seq->cur_pic.TopFieldOrderCnt =
	  expectedPicOrderCnt + sl->delta_pic_order_cnt[0];
      else
	seq->cur_pic.BottomFieldOrderCnt =
	  expectedPicOrderCnt + sp->offset_for_top_to_bottom_field +
	  sl->delta_pic_order_cnt[1];
    }
    else
    {
      int32_t tmpPicOrderCnt;
      if (sl->nal_unit_type == NAL_SLICE_IDR)
	tmpPicOrderCnt = 0;
      else if (!sl->nal_ref_idc)
	tmpPicOrderCnt = 2 * (FrameNumOffset + sl->frame_num) - 1;
      else
	tmpPicOrderCnt = 2 * (FrameNumOffset + sl->frame_num);

      if (!sl->field_pic_flag)
	seq->cur_pic.TopFieldOrderCnt = seq->cur_pic.BottomFieldOrderCnt =
	  tmpPicOrderCnt;
      else if (sl->bottom_field_flag)
	seq->cur_pic.BottomFieldOrderCnt = tmpPicOrderCnt;
      else
	seq->cur_pic.TopFieldOrderCnt = tmpPicOrderCnt;
    }
    seq->prevFrameNum = seq->cur_pic.FrameNum;
    seq->prevFrameNumOffset = FrameNumOffset;
  }

  if (seq->cur_pic.completed < PICTURE_DONE)
  {
    if (sl->bottom_field_flag)
      seq->cur_pic.TopFieldOrderCnt = seq->cur_pic.BottomFieldOrderCnt;
    else
      seq->cur_pic.BottomFieldOrderCnt = seq->cur_pic.TopFieldOrderCnt;
  }
}



static void
decode_picnum (vdpau_h264_alter_decoder_t * this_gen)
{
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;
  dpb_frame_t *frame;
  int i = 0;

  int parity = sl->bottom_field_flag ? 1 : 0;

  if (!seq->cur_pic.field_pic_flag)
    seq->cur_pic.PicNum[0] = seq->cur_pic.FrameNum;
  else
    seq->cur_pic.PicNum[parity] = 2 * seq->cur_pic.FrameNum + 1;

  while (i < MAX_DPB_SIZE)
  {
    frame = seq->dpb[i];
    if (!frame->used)
      break;
    if (frame->FrameNum > seq->cur_pic.FrameNum)
      frame->FrameNumWrap = frame->FrameNum - sl->MaxFrameNum;
    else
      frame->FrameNumWrap = frame->FrameNum;

    if (!sl->field_pic_flag)
    {
      frame->PicNum[0] = frame->PicNum[1] = frame->FrameNumWrap;
    }
    else
    {
      frame->PicNum[0] = 2 * frame->FrameNumWrap + (parity ? 0 : 1);
      frame->PicNum[1] = 2 * frame->FrameNumWrap + (parity ? 1 : 0);
    }
    ++i;
  }
}



static int
check_ref_list (vdpau_h264_alter_decoder_t * this_gen)
{
  int i, j, bad_frame = 0;
  dpb_frame_t *frame;
  sequence_t *seq = (sequence_t *) & this_gen->sequence;
  slice_param_t *sl = &seq->slice_param;
  pic_param_t *pic = seq->pic_param[sl->pic_parameter_set_id];
  seq_param_t *sp = seq->seq_param[pic->seq_parameter_set_id];
  int prefs = 0;
  int brefs = 0;
  int poc, curpoc;

  //int fps = (double)sp->vui.time_scale / (double)sp->vui.num_units_in_tick / ( 2 - sl->field_pic_flag );
  int fps = (1 + sl->field_pic_flag) * 2 * sp->num_ref_frames;

  if (seq->startup_frame >= fps)
    return 0;

  curpoc =
    (seq->cur_pic.TopFieldOrderCnt >
     seq->cur_pic.BottomFieldOrderCnt) ? seq->cur_pic.TopFieldOrderCnt : seq->
    cur_pic.BottomFieldOrderCnt;

  for (i = 15; i > -1; i--)
  {
    frame = seq->dpb[i];
    if (!frame->used)
      continue;
    poc =
      (frame->TopFieldOrderCnt >
       frame->BottomFieldOrderCnt) ? frame->TopFieldOrderCnt : frame->
      BottomFieldOrderCnt;
    if (seq->cur_pic.field_pic_flag)
    {
      if (!frame->videoSurface->bad_frame)
      {
	for (j = 0; j < 2; j++)
	{
	  if (frame->is_reference[j])
	  {
	    if (poc <= curpoc)
	      ++prefs;
	    else
	      ++brefs;
	  }
	}
      }
    }
    else
    {
      if (!frame->videoSurface->bad_frame)
      {
	if (poc <= curpoc)
	  ++prefs;
	else
	  ++brefs;
      }
    }
  }

  if (sl->slice_type != SLICE_TYPE_I)
  {
    if (prefs < (sl->num_ref_idx_l0_active_minus1 + 1))
      bad_frame = 1;
    if (sl->slice_type == SLICE_TYPE_B)
    {
      if (brefs < (sl->num_ref_idx_l1_active_minus1 + 1))
	bad_frame = 1;
    }
  }

  if (bad_frame)
    fprintf (stderr,
	     "******** Missing refframes, dropping. nrf=%d lo=%d prefs=%d l1=%d brefs=%d type=%d (%d fps)\n",
	     sp->num_ref_frames, sl->num_ref_idx_l0_active_minus1 + 1, prefs,
	     sl->num_ref_idx_l1_active_minus1 + 1, brefs, sl->slice_type,
	     fps);
  //else
  //fprintf(stderr,"******** GOOD ! nrf=%d lo=%d prefs=%d l1=%d brefs=%d type=%d (%d fps)\n", sp->num_ref_frames, sl->num_ref_idx_l0_active_minus1 + 1, prefs, sl->num_ref_idx_l1_active_minus1 + 1, brefs, sl->slice_type, fps );

  if (seq->cur_pic.is_reference[0] || seq->cur_pic.is_reference[1])
    ++seq->startup_frame;

  return bad_frame;
}



static void
decode_render (vdpau_h264_alter_decoder_t * vd, int bad_frame)
{
  int i, j;
  VdpPictureInfoH264 info;
  seq_param_t *sp;
  pic_param_t *pic;
  slice_param_t *sl;
  sequence_t *seq = (sequence_t *) & vd->sequence;
  vo_frame_t *img;

  if (!seq->cur_pic.field_pic_flag || (seq->cur_pic.completed < PICTURE_DONE))
  {
    img =
      vd->stream->video_out->get_frame (vd->stream->video_out,
					seq->coded_width, seq->coded_height,
					seq->ratio, XINE_IMGFMT_VDPAU,
					VO_BOTH_FIELDS | seq->chroma | seq->
					reset | seq->color_matrix);
    seq->reset = 0;
    img->drawn = 0;
  }
  else
    img = seq->cur_pic.videoSurface;

  if (!img)
  {				/* should not happen */
    fprintf (stderr,
	     "vdpau_h264_alter : !!!!!!!!!!!!!!!!!!!!!! No vo_frame_t !!!!!!!!!!!!!!!!!!!!!!!\n");
    return;
  }

  vdpau_accel_t *accel = (vdpau_accel_t *) img->accel_data;
  if (!seq->accel_vdpau)
    seq->accel_vdpau = accel;

  if (seq->vdp_runtime_nr != *(seq->accel_vdpau->current_vdp_runtime_nr))
    vd->decoder = VDP_INVALID_HANDLE;

  sl = &vd->sequence.slice_param;
  pic = vd->sequence.pic_param[sl->pic_parameter_set_id];
  sp = vd->sequence.seq_param[pic->seq_parameter_set_id];

  VdpStatus st;
  if (vd->decoder == VDP_INVALID_HANDLE || vd->decoder_profile != seq->profile
      || vd->decoder_width != seq->coded_width
      || vd->decoder_height != seq->coded_height)
  {
    if (vd->decoder != VDP_INVALID_HANDLE)
    {
      accel->vdp_decoder_destroy (vd->decoder);
      vd->decoder = VDP_INVALID_HANDLE;
    }
    st =
      accel->vdp_decoder_create (accel->vdp_device, seq->profile,
				 seq->coded_width, seq->coded_height,
				 sp->num_ref_frames, &vd->decoder);
    if (st != VDP_STATUS_OK)
      fprintf (stderr, "vdpau_h264_alter : failed to create decoder !! %s\n",
	       accel->vdp_get_error_string (st));
    else
    {
      vd->decoder_profile = seq->profile;
      vd->decoder_width = seq->coded_width;
      vd->decoder_height = seq->coded_height;
      seq->vdp_runtime_nr = seq->accel_vdpau->vdp_runtime_nr;
    }
  }

  info.slice_count = seq->slices_count;
  info.field_order_cnt[0] = seq->cur_pic.TopFieldOrderCnt;
  info.field_order_cnt[1] = seq->cur_pic.BottomFieldOrderCnt;
  info.is_reference = sl->nal_ref_idc ? VDP_TRUE : VDP_FALSE;
  info.frame_num = sl->frame_num;
  info.field_pic_flag = sl->field_pic_flag;
  info.bottom_field_flag = sl->bottom_field_flag;
  info.num_ref_frames = sp->num_ref_frames;
  info.mb_adaptive_frame_field_flag = sp->mb_adaptive_frame_field_flag
    && !sl->field_pic_flag;
  info.constrained_intra_pred_flag = pic->constrained_intra_pred_flag;
  info.weighted_pred_flag = pic->weighted_pred_flag;
  info.weighted_bipred_idc = pic->weighted_bipred_idc;
  info.frame_mbs_only_flag = sp->frame_mbs_only_flag;
  info.transform_8x8_mode_flag = pic->transform_8x8_mode_flag;
  info.chroma_qp_index_offset = pic->chroma_qp_index_offset;
  info.second_chroma_qp_index_offset = pic->second_chroma_qp_index_offset;
  info.pic_init_qp_minus26 = pic->pic_init_qp_minus26;
  info.num_ref_idx_l0_active_minus1 = pic->num_ref_idx_l0_active_minus1;
  info.num_ref_idx_l1_active_minus1 = pic->num_ref_idx_l1_active_minus1;
  info.log2_max_frame_num_minus4 = sp->log2_max_frame_num_minus4;
  info.pic_order_cnt_type = sp->pic_order_cnt_type;
  info.log2_max_pic_order_cnt_lsb_minus4 =
    sp->log2_max_pic_order_cnt_lsb_minus4;
  info.delta_pic_order_always_zero_flag =
    sp->delta_pic_order_always_zero_flag;
  info.direct_8x8_inference_flag = sp->direct_8x8_inference_flag;
  info.entropy_coding_mode_flag = pic->entropy_coding_mode_flag;
  info.pic_order_present_flag = pic->pic_order_present_flag;
  info.deblocking_filter_control_present_flag =
    pic->deblocking_filter_control_present_flag;
  info.redundant_pic_cnt_present_flag = pic->redundant_pic_cnt_present_flag;

  if (!pic->pic_scaling_matrix_present_flag)
  {
    xine_fast_memcpy (info.scaling_lists_4x4, sp->scaling_lists_4x4,
		      sizeof (info.scaling_lists_4x4));
    xine_fast_memcpy (info.scaling_lists_8x8, sp->scaling_lists_8x8,
		      sizeof (info.scaling_lists_8x8));
  }
  else
  {
    xine_fast_memcpy (info.scaling_lists_4x4, pic->scaling_lists_4x4,
		      sizeof (info.scaling_lists_4x4));
    xine_fast_memcpy (info.scaling_lists_8x8, pic->scaling_lists_8x8,
		      sizeof (info.scaling_lists_8x8));
  }

  j = 0;
  for (i = (MAX_DPB_SIZE - 1); i > -1; i--)
  {
    if (!seq->dpb[i]->used)
      continue;
    vdpau_accel_t *accel =
      (vdpau_accel_t *) seq->dpb[i]->videoSurface->accel_data;
    info.referenceFrames[j].surface = accel->surface;
    info.referenceFrames[j].is_long_term = 0;
    info.referenceFrames[j].frame_idx = seq->dpb[i]->FrameNum;
    info.referenceFrames[j].top_is_reference =
      seq->dpb[i]->is_reference[0] ? VDP_TRUE : VDP_FALSE;
    info.referenceFrames[j].bottom_is_reference =
      seq->dpb[i]->is_reference[1] ? VDP_TRUE : VDP_FALSE;
    info.referenceFrames[j].field_order_cnt[0] =
      seq->dpb[i]->TopFieldOrderCnt;
    info.referenceFrames[j].field_order_cnt[1] =
      seq->dpb[i]->BottomFieldOrderCnt;
    ++j;
  }
  for (; j < MAX_DPB_SIZE; j++)
  {
    info.referenceFrames[j].surface = VDP_INVALID_HANDLE;
    info.referenceFrames[j].is_long_term = 0;
    info.referenceFrames[j].frame_idx = 0;
    info.referenceFrames[j].top_is_reference = 0;
    info.referenceFrames[j].bottom_is_reference = 0;
    info.referenceFrames[j].field_order_cnt[0] = 0;
    info.referenceFrames[j].field_order_cnt[1] = 0;
  }

  uint8_t sc[3] = { 0, 0, 1 };
  VdpBitstreamBuffer vbits[seq->slices_count * 2];
  for (i = 0; i < seq->slices_count; i++)
  {
    vbits[i * 2].struct_version = VDP_BITSTREAM_BUFFER_VERSION;
    vbits[i * 2].bitstream = sc;
    vbits[i * 2].bitstream_bytes = 3;
    vbits[(i * 2) + 1].struct_version = VDP_BITSTREAM_BUFFER_VERSION;
    vbits[(i * 2) + 1].bitstream = seq->buf + seq->slices[i].buf_offset;
    vbits[(i * 2) + 1].bitstream_bytes = seq->slices[i].len;
  }
  st =
    accel->vdp_decoder_render (vd->decoder, accel->surface,
			       (VdpPictureInfo *) & info,
			       seq->slices_count * 2, vbits);
  if (st != VDP_STATUS_OK)
    lprintf ("**********************DECODING failed! - surface = %d - %s\n",
	     accel->surface, accel->vdp_get_error_string (st));
  else
    lprintf ("**********************DECODING success! - surface = %d\n",
	     accel->surface);

  if ((seq->ratio != seq->reported_ratio)
      || (seq->coded_width != seq->reported_coded_width)
      || (seq->coded_height != seq->reported_coded_height)
      || (seq->video_step != seq->reported_video_step))
  {
    seq->reported_ratio = seq->ratio;
    seq->reported_coded_width = seq->coded_width;
    seq->reported_coded_height = seq->coded_height;
    seq->reported_video_step = seq->video_step;
    _x_stream_info_set (vd->stream, XINE_STREAM_INFO_VIDEO_WIDTH,
			seq->coded_width);
    _x_stream_info_set (vd->stream, XINE_STREAM_INFO_VIDEO_HEIGHT,
			seq->coded_height);
    _x_stream_info_set (vd->stream, XINE_STREAM_INFO_VIDEO_RATIO,
			((double) 10000 * seq->ratio));
    _x_stream_info_set (vd->stream, XINE_STREAM_INFO_FRAME_DURATION,
			seq->video_step);
    _x_meta_info_set_utf8 (vd->stream, XINE_META_INFO_VIDEOCODEC,
			   "H264/AVC (vdpau_alter)");
    xine_event_t event;
    xine_format_change_data_t data;
    event.type = XINE_EVENT_FRAME_FORMAT_CHANGE;
    event.stream = vd->stream;
    event.data = &data;
    event.data_length = sizeof (data);
    data.width = seq->coded_width;
    data.height = seq->coded_height;
    data.aspect = seq->ratio;
    xine_event_send (vd->stream, &event);
  }

  if (seq->cur_pic.completed == PICTURE_DONE)
  {
    seq->cur_pic.pts = seq->pic_pts;
    seq->pic_pts = 0;
  }
  if (seq->cur_pic.drop_pts)
    seq->cur_pic.pts = 0;
  if (sp->frame_mbs_only_flag)
    img->progressive_frame = -1;
  img->bad_frame = bad_frame;
  img->duration = seq->video_step;
  seq->cur_pic.videoSurface = img;
}



static void
decode_picture (vdpau_h264_alter_decoder_t * decoder)
{
  if (decoder->sequence.cur_pic.missing_header
      || !decoder->sequence.startup_frame)
  {
    memset (&decoder->sequence.cur_pic, 0, sizeof (dpb_frame_t));
    lprintf ("MISSING_HEADER or !startup_frame\n\n");
    return;
  }

  slice_param_t *sl = &decoder->sequence.slice_param;
  dpb_frame_t *cur_pic = &decoder->sequence.cur_pic;

  if (cur_pic->completed && cur_pic->field_pic_flag)
  {
    int wrong_field = 0;
    if ((sl->frame_num != cur_pic->FrameNum)
	|| (sl->bottom_field_flag
	    && (cur_pic->completed == PICTURE_BOTTOM_DONE))
	|| (!sl->bottom_field_flag
	    && (cur_pic->completed == PICTURE_TOP_DONE))
	|| !sl->field_pic_flag)
    {
      wrong_field = 1;
    }
    if (wrong_field)
    {
      fprintf (stderr, "vdpau_h264_alter : Wrong field, skipping.\n");
      memset (cur_pic, 0, sizeof (dpb_frame_t));
      dpb_reset (&decoder->sequence);
      cur_pic->missing_header = 1;
      decoder->sequence.startup_frame = 0;
      return;
    }
  }

  /* picture decoding */
  decode_poc (decoder);
  lprintf ("TopFieldOrderCnt = %d - BottomFieldOrderCnt = %d\n",
	   cur_pic->TopFieldOrderCnt, cur_pic->BottomFieldOrderCnt);
  if (sl->nal_unit_type == 5)
  {
    dpb_draw_frames (decoder, MAX_POC, DPB_DRAW_CLEAR);
    decoder->sequence.startup_frame = START_IDR_FLAG;
  }
  decode_picnum (decoder);
  ref_pic_list_reordering (decoder);
  lprintf ("............................. slices_count = %d\n",
	   decoder->sequence.slices_count);

  decode_render (decoder, check_ref_list (decoder));

  /* dec_ref_pic_marking */
  slice_header_post (decoder);

  if (!cur_pic->is_reference[0] && !cur_pic->is_reference[1])
  {
    if (cur_pic->completed == PICTURE_DONE)
    {
      dpb_draw_frames (decoder,
		       (cur_pic->TopFieldOrderCnt >
			cur_pic->BottomFieldOrderCnt) ? cur_pic->
		       TopFieldOrderCnt : cur_pic->BottomFieldOrderCnt,
		       DPB_DRAW_CURRENT);
    }
  }
  else
  {
    if (decoder->sequence.
	seq_param[decoder->sequence.pic_param[sl->pic_parameter_set_id]->
		  seq_parameter_set_id]->pic_order_cnt_type == 2)
      dpb_draw_frames (decoder,
		       (cur_pic->TopFieldOrderCnt >
			cur_pic->BottomFieldOrderCnt) ? cur_pic->
		       TopFieldOrderCnt : cur_pic->BottomFieldOrderCnt,
		       DPB_DRAW_REFS);

    if (!sl->field_pic_flag || cur_pic->completed < PICTURE_DONE)
      dbp_append (decoder, 0);
    else
      dbp_append (decoder, 1);
  }

  if (cur_pic->completed == PICTURE_DONE)
    memset (cur_pic, 0, sizeof (dpb_frame_t));


  lprintf
    ("\n___________________________________________________________________________________________\n\n");
}



static int
parse_startcodes (vdpau_h264_alter_decoder_t * this_gen, uint8_t * buf,
		  uint32_t len)
{
  sequence_t *sequence = (sequence_t *) & this_gen->sequence;
  bits_reader_set (&sequence->br, buf, len);
  int ret = 0;

  skip_bits (&sequence->br, 1);	/* forbidden_zero_bit */
  uint8_t nal_ref_idc = read_bits (&sequence->br, 2);
  uint8_t nal_unit_type = read_bits (&sequence->br, 5);
  lprintf ("NAL size = %d, nal_ref_idc = %d, nal_unit_type = %d\n", len,
	   nal_ref_idc, nal_unit_type);

  switch (nal_unit_type)
  {
  case NAL_END_SEQUENCE:
    break;
  case NAL_SEQUENCE:
    seq_parameter_set_data (this_gen);
    break;
  case NAL_PICTURE:
    pic_parameter_set (this_gen);
    break;
  case NAL_SLICE_IDR:
    slice_header (this_gen, nal_ref_idc, nal_unit_type);
    sequence->slices[sequence->slices_count].buf_offset = buf - sequence->buf;
    sequence->slices[sequence->slices_count].len = len;
    ++sequence->slices_count;
    sequence->slice_mode = NAL_SLICE_IDR;
    break;
  case NAL_SLICE_NO_IDR:
    slice_header (this_gen, nal_ref_idc, nal_unit_type);
    sequence->slices[sequence->slices_count].buf_offset = buf - sequence->buf;
    sequence->slices[sequence->slices_count].len = len;
    ++sequence->slices_count;
    sequence->slice_mode = NAL_SLICE_NO_IDR;
    break;
  }

  return ret;
}



static void
parse_codec_private (vdpau_h264_alter_decoder_t * this_gen, const uint8_t * buf,
		     uint32_t len)
{
  sequence_t *sequence = (sequence_t *) & this_gen->sequence;
  bits_reader_set (&sequence->br, buf, len);
  const uint8_t *buffer = buf;
  int i;

  lprintf ("parse_codec_private\n");

  sequence->mode_frame = 1;

  // reserved 
  skip_bits (&sequence->br, 8);
  skip_bits (&sequence->br, 8);	/* profile_idc */
  skip_bits (&sequence->br, 8);
  skip_bits (&sequence->br, 8);	/* level_idc */
  skip_bits (&sequence->br, 6);

  sequence->frame_header_size = read_bits (&sequence->br, 2) + 1;
  //sequence->frame_header_size = 3;
  skip_bits (&sequence->br, 3);
  uint8_t count = read_bits (&sequence->br, 5);
  buffer += 6;
  for (i = 0; i < count; i++)
  {
    bits_reader_set (&sequence->br, buffer, len - (buffer - buf));
    uint16_t sps_size = read_bits (&sequence->br, 16);
    skip_bits (&sequence->br, 8);
    seq_parameter_set_data (this_gen);
    buffer += sps_size + 2;
  }
  count = buffer[0];
  ++buffer;
  for (i = 0; i < count; i++)
  {
    bits_reader_set (&sequence->br, buffer, len - (buffer - buf));
    uint16_t pps_size = read_bits (&sequence->br, 16);
    skip_bits (&sequence->br, 8);
    pic_parameter_set (this_gen);
    buffer += pps_size + 2;
  }
}



static void
flush_buffer (sequence_t * seq)
{
  if ((seq->bufpos - seq->bufseek) >= seq->bufseek)
  {
    seq->bufsize = (seq->bufpos - seq->bufseek) + MIN_BUFFER_SIZE;
    lprintf ("buffer too short, have to allocate a new one.\n");
    uint8_t *tmp = (uint8_t *) malloc (seq->bufsize);
    xine_fast_memcpy (tmp, seq->buf + seq->bufseek,
		      seq->bufpos - seq->bufseek);
    free (seq->buf);
    seq->buf = tmp;
  }
  else
    xine_fast_memcpy (seq->buf, seq->buf + seq->bufseek,
		      seq->bufpos - seq->bufseek);

  seq->bufpos -= seq->bufseek;
  seq->start = -1;
  seq->bufseek = 0;
  reset_slices (seq);
}



/*
 * This function receives a buffer of data from the demuxer layer and
 * figures out how to handle it based on its header flags.
 */
static void
vdpau_h264_alter_decode_data (video_decoder_t * this_gen, buf_element_t * buf)
{
  vdpau_h264_alter_decoder_t *this = (vdpau_h264_alter_decoder_t *) this_gen;
  sequence_t *seq = (sequence_t *) & this->sequence;

#ifdef MAKE_DAT
  fwrite (buf->content, 1, buf->size, outfile);
#endif

  if (buf->decoder_flags & BUF_FLAG_FRAMERATE)
  {
    lprintf ("BUF_FLAG_FRAMERATE\n");
    seq->video_step = buf->decoder_info[0];
  }

  if (buf->decoder_flags & BUF_FLAG_ASPECT)
  {
    lprintf ("BUF_FLAG_ASPECT\n");
    seq->ratio =
      (double) buf->decoder_info[1] / (double) buf->decoder_info[2];
  }

  if (buf->decoder_flags & BUF_FLAG_STDHEADER)
  {
    lprintf ("BUF_FLAG_STDHEADER\n");
    seq->flag_header = 1;
    xine_bmiheader *bih = (xine_bmiheader *) buf->content;
    seq->coded_width = bih->biWidth;
    seq->coded_height = bih->biHeight;
    uint8_t *codec_private = buf->content + sizeof (xine_bmiheader);
    uint32_t codec_private_len = bih->biSize - sizeof (xine_bmiheader);
    if (codec_private_len > 0)
      parse_codec_private (this, codec_private, codec_private_len);
    return;
  }

  if (buf->decoder_flags & BUF_FLAG_SPECIAL)
  {
    if (buf->decoder_info[1] == BUF_SPECIAL_DECODER_CONFIG)
    {
      lprintf ("BUF_SPECIAL_DECODER_CONFIG\n");
      seq->flag_header = 1;
      const uint8_t *codec_private = buf->decoder_info_ptr[2];
      uint32_t codec_private_len = buf->decoder_info[2];
      if (codec_private_len > 0)
	parse_codec_private (this, codec_private, codec_private_len);
    }
    return;
  }

  if (!buf->size)
    return;

  int size = seq->bufpos + buf->size;
  if (seq->bufsize < size)
  {
    if (seq->bufsize > MAX_BUFFER_SIZE)
    {
      fprintf (stderr,
	       "vdpau_h264_alter : sorry, can't accumulate so much data, broken stream?\n");
      reset_sequence (seq);
      return;
    }
    seq->bufsize = size + MIN_BUFFER_SIZE;
    seq->buf = (uint8_t *) realloc (seq->buf, seq->bufsize);
    lprintf ("realloc new size = %d\n", seq->bufsize);
  }
  xine_fast_memcpy (seq->buf + seq->bufpos, buf->content, buf->size);
  seq->bufpos += buf->size;

  if (buf->decoder_flags & BUF_FLAG_FRAME_START)
    seq->pic_pts = buf->pts;

  int frame_end = buf->decoder_flags & BUF_FLAG_FRAME_END;

  if (seq->mode_frame)
  {
    if (!seq->pic_pts)
      seq->pic_pts = buf->pts;
    if (frame_end)
    {
      if (buf->pts)
	seq->pic_pts = buf->pts;
      lprintf ("frame_end && seq->mode_frame\n");
      int fhs;
      uint8_t tb;
      uint32_t j = 0;
      while (j < seq->bufpos)
      {
	uint32_t s = 0;
	for (fhs = 0; fhs < seq->frame_header_size; fhs++)
	  s |= seq->buf[j + fhs] << (8 * (seq->frame_header_size - 1 - fhs));
	tb = *(seq->buf + j + seq->frame_header_size) & 0x1F;
	if (seq->slice_mode && (tb != seq->slice_mode))
	{
	  decode_picture (this);
	  reset_slices (seq);
	}
	parse_startcodes (this, seq->buf + j + seq->frame_header_size, s);
	j += seq->frame_header_size + s;
      }
      if (seq->slice_mode)
      {
	decode_picture (this);
	reset_slices (seq);
      }
      seq->bufpos = 0;
    }
    return;
  }

  while (seq->bufseek <= seq->bufpos - 4)
  {
    uint8_t *buffer = seq->buf + seq->bufseek;
    if (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 1)
    {
      if (seq->start < 0)
      {
	seq->start = seq->bufseek;
	uint8_t tb = buffer[3] & 0x1F;
	if (((tb == NAL_SLICE_NO_IDR) || (tb == NAL_SLICE_IDR))
	    && !seq->pic_pts)
	  seq->pic_pts = buf->pts;
	if (seq->slice_mode && (tb != seq->slice_mode))
	{
	  decode_picture (this);
	  flush_buffer (seq);
	}
	if ((tb & 0x1F) == NAL_END_SEQUENCE)
	{
	  dpb_print (seq);
	  dpb_draw_frames (this, MAX_POC, DPB_DRAW_CLEAR);
	  lprintf ("NAL_END_SEQUENCE\n");
	  dpb_print (seq);
	}
      }
      else
      {
	parse_startcodes (this, seq->buf + seq->start + 3,
			  seq->bufseek - seq->start - 3);
	seq->start = -1;
	--seq->bufseek;
      }
    }
    ++seq->bufseek;
  }

  if (frame_end && seq->flag_header && (seq->start > -1)
      && (seq->bufseek > seq->start))
  {
    lprintf ("frame_end && seq->start\n");
    seq->bufseek = seq->bufpos;
    parse_startcodes (this, seq->buf + seq->start + 3,
		      seq->bufseek - seq->start - 3);
    if (seq->slice_mode)
      decode_picture (this);
    flush_buffer (seq);
  }
}


/*
 * This function is called when xine needs to flush the system.
 */
static void
vdpau_h264_alter_flush (video_decoder_t * this_gen)
{
  vdpau_h264_alter_decoder_t *this = (vdpau_h264_alter_decoder_t *) this_gen;

  printf ("vdpau_h264_alter_flush\n");
  dpb_draw_frames (this, MAX_POC, DPB_DRAW_REFS);
}


/*
 * This function resets the video decoder.
 */
static void
vdpau_h264_alter_reset (video_decoder_t * this_gen)
{
  vdpau_h264_alter_decoder_t *this = (vdpau_h264_alter_decoder_t *) this_gen;

  lprintf ("vdpau_h264_alter_reset\n");
  reset_sequence (&this->sequence);
}


/*
 * The decoder should forget any stored pts values here.
 */
static void
vdpau_h264_alter_discontinuity (video_decoder_t * this_gen)
{
  vdpau_h264_alter_decoder_t *this = (vdpau_h264_alter_decoder_t *) this_gen;
  printf ("vdpau_h264_alter_discontinuity\n");

  dpb_clear_all_pts (&this->sequence);
  this->sequence.reset = VO_NEW_SEQUENCE_FLAG;
}


/*
 * This function frees the video decoder instance allocated to the decoder.
 */
static void
vdpau_h264_alter_dispose (video_decoder_t * this_gen)
{

  vdpau_h264_alter_decoder_t *this = (vdpau_h264_alter_decoder_t *) this_gen;

  lprintf ("vdpau_h264_alter_dispose\n");

  if ((this->decoder != VDP_INVALID_HANDLE) && this->sequence.accel_vdpau)
  {
    this->sequence.accel_vdpau->vdp_decoder_destroy (this->decoder);
    this->decoder = VDP_INVALID_HANDLE;
  }
  reset_sequence (&this->sequence);

  int i;
  for (i = 0; i < MAX_DPB_SIZE; i++)
    free (this->sequence.dpb[i]);
  for (i = 0; i < 32; i++)
    if (this->sequence.seq_param[i])
      free (this->sequence.seq_param[i]);
  for (i = 0; i < 255; i++)
    if (this->sequence.pic_param[i])
      free (this->sequence.pic_param[i]);

  this->stream->video_out->close (this->stream->video_out, this->stream);

  free (this->sequence.buf);
  free (this_gen);
}


/*
 * This function allocates, initializes, and returns a private video
 * decoder structure.
 */
static video_decoder_t *
open_plugin (video_decoder_class_t * class_gen, xine_stream_t * stream)
{

  vdpau_h264_alter_decoder_t *this;

  /* the videoout must be vdpau-capable to support this decoder */
  if (!
      (stream->video_driver->
       get_capabilities (stream->video_driver) & VO_CAP_VDPAU_H264))
    return NULL;

  /* now check if vdpau has free decoder resource */
  vo_frame_t *img =
    stream->video_out->get_frame (stream->video_out, 1920, 1080, 1,
				  XINE_IMGFMT_VDPAU, VO_BOTH_FIELDS);
  vdpau_accel_t *accel = (vdpau_accel_t *) img->accel_data;
  int runtime_nr = accel->vdp_runtime_nr;
  img->free (img);
  VdpDecoder decoder;
  VdpStatus st =
    accel->vdp_decoder_create (accel->vdp_device,
			       VDP_DECODER_PROFILE_H264_MAIN, 1920, 1080, 16,
			       &decoder);
  if (st != VDP_STATUS_OK)
  {
    fprintf (stderr, "can't create vdpau decoder!\n");
    return NULL;
  }

  accel->vdp_decoder_destroy (decoder);

  this =
    (vdpau_h264_alter_decoder_t *) calloc (1,
					   sizeof
					   (vdpau_h264_alter_decoder_t));

  this->video_decoder.decode_data = vdpau_h264_alter_decode_data;
  this->video_decoder.flush = vdpau_h264_alter_flush;
  this->video_decoder.reset = vdpau_h264_alter_reset;
  this->video_decoder.discontinuity = vdpau_h264_alter_discontinuity;
  this->video_decoder.dispose = vdpau_h264_alter_dispose;

  this->stream = stream;
  this->class = (vdpau_h264_alter_class_t *) class_gen;

  int i;
  for (i = 0; i < 16; i++)
    this->sequence.dpb[i] = (dpb_frame_t *) calloc (1, sizeof (dpb_frame_t));
  this->sequence.bufsize = MIN_BUFFER_SIZE;
  this->sequence.buf = (uint8_t *) malloc (this->sequence.bufsize);
  this->sequence.vdp_runtime_nr = runtime_nr;
  this->sequence.reset = VO_NEW_SEQUENCE_FLAG;
  this->sequence.reset = VO_NEW_SEQUENCE_FLAG;
  this->sequence.ratio = 0.0;
  this->sequence.video_step = 3600;
  this->sequence.coded_width = 1280;
  this->sequence.coded_height = 720;
  this->sequence.reported_ratio = 0.0;
  this->sequence.reported_video_step = 0;
  this->sequence.reported_coded_width = 0;
  this->sequence.reported_coded_height = 0;
  this->sequence.frame_header_size = 4;
  this->sequence.flag_header = 0;
  this->sequence.mode_frame = 0;
  reset_sequence (&this->sequence);

  this->decoder = VDP_INVALID_HANDLE;
  this->sequence.accel_vdpau = NULL;

  (stream->video_out->open) (stream->video_out, stream);

#ifdef MAKE_DAT
  outfile = fopen ("/tmp/h264.dat", "w");
  nframes = 0;
#endif

  return &this->video_decoder;
}


/*
 * This function allocates a private video decoder class and initializes
 * the class's member functions.
 */
static void *
init_plugin (xine_t * xine, void *data)
{

  vdpau_h264_alter_class_t *this;

  this =
    (vdpau_h264_alter_class_t *) calloc (1,
					 sizeof (vdpau_h264_alter_class_t));

  this->decoder_class.open_plugin = open_plugin;
  this->decoder_class.identifier = "vdpau_h264_alter";
  this->decoder_class.description =
    N_
    ("vdpau_h264_alter: H264 decoder plugin using VDPAU hardware decoding.\n"
     "Must be used along with video_out_vdpau.");
  this->decoder_class.dispose = default_video_decoder_class_dispose;

  return this;
}


/*
 * This is a list of all of the internal xine video buffer types that
 * this decoder is able to handle. Check src/xine-engine/buffer.h for a
 * list of valid buffer types (and add a new one if the one you need does
 * not exist). Terminate the list with a 0.
 */
static const uint32_t video_types[] = {
  BUF_VIDEO_H264,
  0
};


/*
 * This data structure combines the list of supported xine buffer types and
 * the priority that the plugin should be given with respect to other
 * plugins that handle the same buffer type. A plugin with priority (n+1)
 * will be used instead of a plugin with priority (n).
 */
static const decoder_info_t dec_info_video = {
  video_types,			/* supported types */
  9				/* priority        */
};


/*
 * The plugin catalog entry. This is the only information that this plugin
 * will export to the public.
 */
const plugin_info_t xine_plugin_info[] EXPORTED = {
  /* { type, API, "name", version, special_info, init_function } */
  {PLUGIN_VIDEO_DECODER, 19, "vdpau_h264_alter", XINE_VERSION_CODE,
   &dec_info_video, init_plugin},
  {PLUGIN_NONE, 0, "", 0, NULL, NULL}
};
