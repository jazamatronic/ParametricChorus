#ifndef IIR_LP_NF_H
#define IIR_LP_NF_H

/*
 * first order iir lp filter implementation used for noise filtering
 *
 */
class iir_lp_nf
{
  public:
    iir_lp_nf() {}
    ~iir_lp_nf() {}

    void Init(float b0, float b1, float a1);
    float Process(float in);

  private:
    float b0_, b1_, a1_, xn1_, yn1_;
};
#endif
