#include "iir_lp_nf.h"

void iir_lp_nf::Init(float b0, float b1, float a1)
{
  b0_ = b0;
  b1_ = b1;
  a1_ = a1;
  xn1_ = yn1_ = 0;
}

float iir_lp_nf::Process(float in)
{
  float out;

  out = b0_ * in + b1_ * xn1_ - a1_ * yn1_;
  xn1_ = in;
  yn1_ = out;

  return out;
}
