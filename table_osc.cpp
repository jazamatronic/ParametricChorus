#include <math.h>
#include "table_osc.h"

using namespace daisysp;

void TableOsc::Init(float sample_rate, table_t *t, float phase)
{
    sr_     = sample_rate;
    t_	    = t;
    phase_ = fmod(phase, t_->l);

}

void TableOsc::Process()
{
  phase_ = phase_ + f_;
  if (phase_ > t_->l) {
    phase_ = fmod(phase_, t_->l);
  }
  if (phase_ < 0) {
    phase_ = phase_ + t_->l;
  }
}

void TableOsc::SetFreq(float f)
{
  f_ = (f * t_->l) / sr_;
}

float TableOsc::InPhase(float pn) {
  float p = phase_ + pn;
  if (p > t_->l) {
    p = fmod(p, t_->l);
  }
  if (p < 0) {
    p = p + t_->l;
  }
  return InterpTable(p);
}

float TableOsc::QuadPhase(float pn) {
  float p = phase_ + pn + (t_->l / 4);
  if (p > t_->l) {
    p = fmod(p, t_->l);
  }
  if (p < 0) {
    p = p + t_->l;
  }
  return InterpTable(p);
}

float TableOsc::InterpTable(float phase)
{
  int idx = phase;
  int next_idx = (idx + 1) % t_->l;
  float r = phase - idx;
  float s0, s1, sf;
  s0 = t_->t[idx];
  s1 = t_->t[next_idx];
  sf = (s0 + r * (s1 - s0));
  return(sf);
}

