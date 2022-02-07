#pragma once
#ifndef DSY_LFSR_H
#define DSY_LFSR_H
#include <stdint.h>
#ifdef __cplusplus

/** do this many shifts before generating a noise sample
 */
#define LFSR_ITR 6

namespace daisysp
{
/** Linear Feedback Shift Register

    From Hal Chamberlin's Musical Applications of Microprocessors
    Should return random numbers between -1.0 and 1.0
    
*/
class LFSR
{
  public:
    LFSR() {}
    ~LFSR() {}

    /** Initializes the LFSR object
    */
    void Init(uint32_t seed)
    {
        rand_ = seed; 
    }

    float Process()
    {
      for (int i = 0; i < LFSR_ITR; i++) {
	uint32_t overflow = rand_ & 0x80000000;
	if (overflow) {
	  rand_ = rand_ << 1;
	  rand_ = rand_ ^ mask_;
	} else {
	  rand_ = rand_ << 1;
	}
      }
      // take the lower 16 bits and convert to a float
      return (2.0 * ((float(rand_ & 0x0000FFFF) / 65535.0f) - 0.5));
    }

  private:
    static constexpr uint32_t mask_ = 0x1D872B41;
    uint32_t		      rand_;
};
} // namespace daisysp
#endif
#endif
