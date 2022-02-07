#include "hilbert_pdn.h"

void HilbertPDN::Process(const float *in) 
{
  float *in1 = const_cast <float *>(in);
  arm_biquad_cascade_df1_f32(&R, in1, r_out, BLOCK_SIZE);
  arm_biquad_cascade_df1_f32(&I, in1, i_out, BLOCK_SIZE);
}

void HilbertPDN::Process(float *in) 
{
  arm_biquad_cascade_df1_f32(&R, in, r_out, BLOCK_SIZE);
  arm_biquad_cascade_df1_f32(&I, in, i_out, BLOCK_SIZE);
}
