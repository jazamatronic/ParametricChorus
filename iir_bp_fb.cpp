#include "iir_bp_fb.h"

void iir_bp_fb::Process(const float *in) 
{
  float *in1 = const_cast <float *>(in);
  for (size_t i = 0; i < BANDS; i++) {
    arm_biquad_cascade_df1_f32(&bank[i], in1, out[i], BLOCK_SIZE);
  }
}

void iir_bp_fb::Process(float *in) 
{
  for (size_t i = 0; i < BANDS; i++) {
    arm_biquad_cascade_df1_f32(&bank[i], in, out[i], BLOCK_SIZE);
  }
}
