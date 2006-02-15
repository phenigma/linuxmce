/* aec.cpp
 *
 * Copyright (C) DFS Deutsche Flugsicherung (2004, 2005). 
 * All Rights Reserved.
 *
 * Acoustic Echo Cancellation NLMS-pw algorithm
 *
 * Version 0.3.4 H-infinity LMS-pw algorithm
 * Version 0.3.1 Allow change of stability parameter delta
 * Version 0.3 filter created with www.dsptutor.freeuk.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

	#include <math.h>
	#include "aec_cwrap.h"
	#include <iaxclient.h>
}
//#include "oss.h"

#include "aec.h"
//#include "intercomd.h"

#if 1
/* Vector Dot Product */
REAL dotp(REAL a[], REAL b[])
{
  REAL sum0 = 0.0, sum1 = 0.0;
  int j;

  for (j = 0; j < NLMS_LEN; j += 2) {
    // optimize: partial loop unrolling
    sum0 += a[j] * b[j];
    sum1 += a[j + 1] * b[j + 1];
  }
  return sum0 + sum1;
}

#else

/* this is unfinished work in progress. */
#include <xmmintrin.h>

inline float dotp(float a[], float b[])
{
  int i;
  __m128 xmm0, xmm1, xmm2;
  __m128 xmm3, xmm4, xmm5;
  float sum[8];

  /* XMM convolution - Intel Pentium 3 and above */
  
  sum[0] = sum[1] = sum[2] = sum[3] = 0.0f;
  xmm2 = _mm_load_ps(sum);
    xmm5 = _mm_load_ps(sum);
  for (i = 0; i < NLMS_LEN; i += 8) {
    /* load not aligned data */
    xmm0 = _mm_loadu_ps(&a[i]);    
      xmm3 = _mm_loadu_ps(&a[i+4]);
       
    xmm1 = _mm_loadu_ps(&b[i]);
      xmm4 = _mm_loadu_ps(&b[i+4]);
    
    /* Intel notation: first operand is destination */
    /* GNU as notation: first operand is source */
    xmm0 = _mm_mul_ps(xmm0, xmm1);
      xmm3 = _mm_mul_ps(xmm3, xmm4);
    
    xmm2 = _mm_add_ps(xmm2, xmm0);      
      xmm5 = _mm_add_ps(xmm5, xmm3);
  }
  _mm_store_ps(sum, xmm2);
  _mm_store_ps(&sum[4], xmm5);
 
  return sum[0] + sum[1] + sum[2] + sum[3]
    + sum[4] + sum[5] + sum[6] + sum[7];
}
#endif


AEC::AEC()
{
  max_max_x = 0.0f;
  hangover = 0;
  memset(max_x, 0, sizeof(max_x));
  dtdCnt = dtdNdx = 0;

  max_max_u = 0.0f;
  memset(max_u, 0, sizeof(max_u));
  c_max_u = i_max_u = 0;

  memset(x, 0, sizeof(x));
  memset(xf, 0, sizeof(xf));
  memset(w, 0, sizeof(w));
  j = NLMS_EXT;
  delta = 0.0f;
  setambient(NoiseFloor);
  dfast = dslow = M50dB_PCM;
  xfast = xslow = M40dB_PCM;
  gain = 1.0f;
  Fx.init(2000.0f/RATE);
  Fe.init(2000.0f/RATE);
  Fxx.init(2000.0f/RATE);
  Fdd.init(2000.0f/RATE);
}

inline double AEC::max_dotp_xf_xf(double u)
{
  // optimized implementation of max(u[0], u[1], .., u[L-1]):
  // calculate max of block (DTD_LEN values)
  if (u > max_u[i_max_u]) {
    max_u[i_max_u] = u;
    if (u > max_max_u) {
      max_max_u = u;
      // printf("max_dotp_xf_xf %f\n", sqrt(max_max_u/NLMS_LEN)); 
    }
  }
  if (++c_max_u >= DTD_LEN) {
    c_max_u = 0;
    // calculate max of max
    max_max_u = 0.0f;
    for (int i = 0; i < NLMS_LEN / DTD_LEN; ++i) {
      if (max_u[i] > max_max_u) {
        max_max_u = max_u[i];
      }
    }
    // rotate Ndx
    if (++i_max_u >= NLMS_LEN / DTD_LEN)
      i_max_u = 0;
    max_u[i_max_u] = 0.0f;
  }
  return max_max_u;
}

float ratio;

#if 1
// Adrian soft decision DTD
// (Dual Average Near-End to Far-End signal Ratio DTD)
// This algorithm uses exponential smoothing with differnt
// ageing parameters to get fast and slow near-end and far-end
// signal averages. The ratio of NFRs term
// (dfast / xfast) / (dslow / xslow) is used to compute the stepsize
// A ratio value of 2.5 is mapped to stepsize 0, a ratio of 0 is
// mapped to 1.0 with a limited linear function.
inline float AEC::dtd(REAL d, REAL x)
{
  float stepsize;
  
  d = Fdd.highpass(d);
  x = Fxx.highpass(x);

  // fast near-end and far-end average
  dfast += 2e-3f * (fabsf(d) - dfast);
  xfast += 2e-3f * (fabsf(x) - xfast);
  
  // slow near-end and far-end average
  dslow += 1e-6f * (fabsf(d) - dslow);
  xslow += 1e-6f * (fabsf(x) - xslow);
  
  if (xfast < M70dB_PCM) {
    return 0.0;   // no Spk signal
  }
  
  if (dfast < M70dB_PCM) {
    return 0.0;   // no Mic signal
  }
  
  // ratio of NFRs
  ratio = (dfast * xslow) / (dslow * xfast);
  
  // begrenzte lineare Kennlinie
  const float M = (STEPY2 - STEPY1) / (STEPX2 - STEPX1);
  if (ratio < STEPX1) {
    stepsize = STEPY1;
  } else if (ratio > STEPX2) {
    stepsize = STEPY2;
  } else {
    // Punktrichtungsform einer Geraden
    stepsize = M * (ratio - STEPX1) + STEPY1;
  }

  return stepsize;
}

#else

// Geigel DTD. return is 0.0 or 1.0
inline float AEC::dtd(REAL d, REAL x)
{
  // optimized implementation of max(|x[0]|, |x[1]|, .., |x[L-1]|):
  // calculate max of block (DTD_LEN values)
  x = fabsf(x);
  if (x > max_x[dtdNdx]) {
    max_x[dtdNdx] = x;
    if (x > max_max_x) {
      max_max_x = x;
    }
  }
  if (++dtdCnt >= DTD_LEN) {
    dtdCnt = 0;
    // calculate max of max
    max_max_x = 0.0f;
    for (int i = 0; i < NLMS_LEN / DTD_LEN; ++i) {
      if (max_x[i] > max_max_x) {
        max_max_x = max_x[i];
      }
    }
    // rotate Ndx
    if (++dtdNdx >= NLMS_LEN / DTD_LEN)
      dtdNdx = 0;
    max_x[dtdNdx] = 0.0f;
  }
  // The Geigel DTD algorithm with Hangover timer Thold
  if (fabsf(d) >= GeigelThreshold * max_max_x) {
    hangover = Thold;
  }

  if (hangover)
    --hangover;
   
  // Silence is the same as Double Talk 
  if (max_max_x < M40dB_PCM) {
    // return 0;
  }

  if (hangover > 0) {
    return STEPY2;
  } else {
    return STEPY1;
  }
}

#endif


inline REAL AEC::nlms_pw(REAL d, REAL x_, float stepsize)
{
  x[j] = x_;
  xf[j] = Fx.highpass(x_);     // pre-whitening of x

  // calculate error value 
  // (mic signal - estimated mic signal from spk signal)
  REAL e = d - dotp(w, x + j);
  REAL ef = Fe.highpass(e);     // pre-whitening of e
  
  // optimize: iterative dotp(xf, xf)
  dotp_xf_xf += (xf[j] * xf[j] - xf[j + NLMS_LEN - 1] * xf[j + NLMS_LEN - 1]);
    
  if (stepsize > 0.0) {
    // calculate variable step size
    REAL mikro_ef = stepsize * ef / dotp_xf_xf;
    
    // inspired by H-infinity theory: use max_dotp_xf_xf(dotp_xf_xf)
    // REAL mikro_ef = 1.0f * ef / max_dotp_xf_xf(dotp_xf_xf);

    // update tap weights (filter learning)
    int i;
    for (i = 0; i < NLMS_LEN; i += 2) {
      // optimize: partial loop unrolling
      w[i] += mikro_ef * xf[i + j];
      w[i + 1] += mikro_ef * xf[i + j + 1];
    }
  }

  if (--j < 0) {
    // optimize: decrease number of memory copies
    j = NLMS_EXT;
    memmove(x + j + 1, x, (NLMS_LEN - 1) * sizeof(REAL));
    memmove(xf + j + 1, xf, (NLMS_LEN - 1) * sizeof(REAL));
  }
  
  // Saturation
  if (e > MAXPCM) {
    return MAXPCM;
  } else if (e < -MAXPCM) {
    return -MAXPCM;
  } else {
    return e;
  }
}

// soft decision Acoustic Echo Suppression (AES) or
// Non Linear Processor (NLP): 
// attenuate d for large x signals (like AGC)
inline float AEC::aes(REAL d)
{
  const float X1 = M70dB_PCM, Y1 = M0dB;
  const float X2 = M40dB_PCM, Y2 = M24dB;
  const float M = (Y2 - Y1) / (X2 - X1);

  // begrenzte lineare Kennlinie
  float atten;
  if (xfast < X1) {
    atten = Y1;
  } else if (xfast > X2) {
    atten = Y2;
  } else {
    // Punktrichtungsform einer Geraden
    atten = M * (xfast - X1) + Y1;
  }
  return d * atten;
}


int AEC::doAEC(int d_, int x_, int enable)
{
  extern int noaes;

  REAL d = (REAL) d_;
  REAL x = (REAL) x_;

  // Mic Highpass Filter - to remove DC
  d = acMic.highpass(d);

  // Mic Highpass Filter - cut-off below 300Hz
  d = cutoff.highpass(d);
  
  // Amplify, for e.g. Soundcards with -6dB max. volume
  d *= gain;

  // Spk Highpass Filter - to remove DC
  if (enable) 
    x = acSpk.highpass(x);

  // Double Talk Detector
  float stepsize = dtd(d, x);
  
  // Acoustic Echo Cancellation
  d = nlms_pw(d, x, stepsize);
  
  // Acoustic Echo Suppression
  //if (NO == noaes) {
    d = aes(d);
  //}

  return (int) d;
}
