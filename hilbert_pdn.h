#pragma once
#ifndef _HILBERT_PDN_H
#define _HILBERT_PDN_H
#ifdef __cplusplus

#include "arm_math.h"

#define NUM_STAGES 4
#define BLOCK_SIZE 48


/*
 * Phase difference network based on IIR hilbert transformer
 *
 * Input a mono signal
 * Two outputs - the original signal after passing through the real branch of a hilbert transformer, and a signal with 90Deg phase shift from the imaginary branch
 *
 * Magic numbers abound.
 *
 * See https://github.com/jazamatronic/iir_hilbert for details
 *
 * Coeffs generated in octave by:
 * 
 *  wc = 0.495;
 *  d_phi = 0.012235;
 *  [B, A, Bi, Ai] = iir_hilbert_even(wc, d_phi);
 *  sosr = tf2sos(B, A)
 *  
 *  sosr =
 *  
 *    -0.07753  -0.47072  -0.47387   1.00000   1.95251   0.95271
 *     1.00000   2.04942   1.04963   1.00000   0.99335   0.16362
 *     1.00000  -2.12652   1.12844   1.00000  -1.46314   0.50620
 *     1.00000  -1.78166   0.00000   1.00000  -0.98260   0.00000
 *  
 *  sosi = tf2sos(Bi, Ai)
 *
 *  sosi =
 *  
 *     0.07753   0.22410   0.15317   1.00000   1.88447   0.88618
 *     1.00000   0.01302  -1.02248   1.00000   0.35282  -0.11700
 *     1.00000  -2.31880   1.33106   1.00000  -1.74207   0.75128
 *     1.00000  -4.79717   0.00000   1.00000  -0.99533   0.00000
 *
 *	Note that the transfer function used in octave's filter is:
 *
 *		          N                   M
 *		y(n) = - SUM c(k+1) y(n-k) + SUM d(k+1) x(n-k)  for 1<=n<=length(x)
 *		         k=1                 k=0
 *
 *     where c = a/a(1) and d = b/a(1).
 *
 *	Whereas the ARM DSP uses:
 *
 *		    y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
 *
 *  i.e. the a coeffs have opposite signs and the a0 column of 1.0s are removed
 *
 *  Assumes a block size of 48 - this is currently hard coded in the daisy_seed config routines
 *
 * Jared ANDERSON
 * 20210813
 */
class HilbertPDN
{
  public:
    HilbertPDN() {}
    ~HilbertPDN() {}

    void Process(const float *in);
    void Process(float *in);

    float r_out[BLOCK_SIZE];
    float i_out[BLOCK_SIZE];

  private:

    float r_state[NUM_STAGES * 4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float i_state[NUM_STAGES * 4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    float r_coeffs[NUM_STAGES * 5] =  {
      -0.07753,  -0.47072,  -0.47387,  -1.95251,   -0.95271,
       1.00000,   2.04942,   1.04963,  -0.99335,   -0.16362,
       1.00000,  -2.12652,   1.12844,   1.46314,   -0.50620,
       1.00000,  -1.78166,   0.00000,   0.98260,   -0.00000
    };

    float i_coeffs[NUM_STAGES * 5] = {
      0.07753,   0.22410,   0.15317,   -1.88447,  -0.88618,
      1.00000,   0.01302,  -1.02248,   -0.35282,   0.11700,
      1.00000,  -2.31880,   1.33106,    1.74207,  -0.75128,
      1.00000,  -4.79717,   0.00000,    0.99533,  -0.00000
    };
    const arm_biquad_casd_df1_inst_f32 R = {NUM_STAGES, r_state, r_coeffs};
    const arm_biquad_casd_df1_inst_f32 I = {NUM_STAGES, i_state, i_coeffs};
};

#endif
#endif
