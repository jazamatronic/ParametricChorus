#ifndef IIR_BP_FB_H
#define IIR_BP_FB_H

#include "arm_math.h"

#define BANDS	   8
#define BLOCK_SIZE 48

/*
 * Simple 2nd order IIR bandpass filter bank
 *
 * Based on: Handbook for Digital Signal Processing Ch 9.
 *
 * And: Sanjit K. Mitra,
 *     Digital Signal Processing a Computer Based Approach
 *
 * See iir_bp.m for coefficient generation
 * 
 * Below coeffs assume 48kHz sample rate
 * 8 bands with an initial bw of 40Hz
 * Block size of 48
 *
 * Band 1, fc = 60, bw = 40
 * B =
 *    0.0026112   0.0000000  -0.0026112
 * A =
 *    1.00000  -1.99472   0.99478
 * 
 * Band 2, fc = 120, bw = 80
 * B =
 *    0.0052088   0.0000000  -0.0052088
 * A =
 *    1.00000  -1.98934   0.98958
 * 
 * Band 3, fc = 240, bw = 160
 * B =
 *    0.010364   0.000000  -0.010364
 * A =
 *    1.00000  -1.97830   0.97927
 * 
 * Band 4, fc = 480, bw = 320
 * B =
 *    0.020517   0.000000  -0.020517
 * A =
 *    1.00000  -1.95510   0.95897
 * 
 * Band 5, fc = 960, bw = 640
 * B =
 *    0.040226   0.000000  -0.040226
 * A =
 *    1.00000  -1.90441   0.91955
 * 
 * Band 6, fc = 1920, bw = 1280
 * B =
 *    0.077467   0.000000  -0.077467
 * A =
 *    1.00000  -1.78710   0.84507
 * 
 * Band 7, fc = 3840, bw = 2560
 * B =
 *    0.14467   0.00000  -0.14467
 * A =
 *    1.00000  -1.49907   0.71066
 * 
 * Band 8, fc = 7680, bw = 5120
 * B =
 *    0.25829   0.00000  -0.25829
 * A =
 *    1.00000  -0.79486   0.48342
 * 
 * For use in the ARM DSP biquad the a coeffs have opposite signs and the a0 column of 1.0s are removed
 *   
 */
class iir_bp_fb  
{
  public:
    iir_bp_fb() {}
    ~iir_bp_fb() {}

    void Process(const float *in);
    void Process(float *in);

    float out[BANDS][BLOCK_SIZE];

  private:
    float state[BANDS][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float coeffs[BANDS][5] = {
       0.0026112, 0.0,	-0.0026112, 1.99472, -0.99478, 
       0.0052088, 0.0,  -0.0052088, 1.98934, -0.98958,
       0.010364,  0.0,  -0.010364,  1.97830, -0.97927,
       0.020517,  0.0,  -0.020517,  1.95510, -0.95897,
       0.040226,  0.0,  -0.040226,  1.90441, -0.91955,
       0.077467,  0.0,  -0.077467,  1.78710, -0.84507,
       0.14467,	  0.0,  -0.14467,   1.49907, -0.71066,
       0.25829,   0.0,  -0.25829,   0.79486, -0.48342
    };
    const arm_biquad_casd_df1_inst_f32 bank[BANDS] = {
      1, state[0], coeffs[0],
      1, state[1], coeffs[1],
      1, state[2], coeffs[2],
      1, state[3], coeffs[3],
      1, state[4], coeffs[4],
      1, state[5], coeffs[5],
      1, state[6], coeffs[6],
      1, state[7], coeffs[7]
    };
};
#endif
