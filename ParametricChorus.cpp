#include "daisy_pod.h"
#include "daisysp.h"
#include "hilbert_pdn.h"
#include "iir_bp_fb.h"
#include "iir_lp_nf.h"
#include "table_osc.h"
#include "lfsr.h"

using namespace daisy;
using namespace daisysp;

//Hard coded sample rate
#define SAMPLE_RATE	  48000

//ms of delay
#define MAX_DELAY	  (48 * 80)
#define DOSC_MAX	  160
#define TABLE_FUNDAMENTAL 220
#define FIRST_BW	  40 
#define MAX_PS	      	  24
#define SEMITONE	  0.08334f
#define CATCH_THRESH	  0.05f

DaisyPod hw;

// Filter bank of band pass filters
iir_bp_fb fb;
// Phase difference network per band
HilbertPDN pdn[BANDS];

// LUT oscs
TableOsc osc[BANDS];
TableOsc dosc[BANDS];

// LFSR noise sources
LFSR plfsr[BANDS];
LFSR dlfsr[BANDS];

// noise filtering
iir_lp_nf pnoise_filt[BANDS];
iir_lp_nf dnoise_filt[BANDS];

static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS del[BANDS];

static Parameter delnoise, delfreq, deldepth, pnshift, pshift, bwmult, mix, spread;
float cur_delnoise, cur_delfreq, cur_deldepth, cur_pn, cur_ps, cur_bwm, cur_mix, cur_spread;

bool ps_noise_mode = true;
bool del_noise_mode = true;

typedef enum { MIX = 0, PHASE, DEL, LAST } page;
page cur_page;


#define BLINK_NUM 500
int blink_cnt = 0;
bool led_state = true;

void UpdateEncoder();
void UpdateButtons();
float CatchParam(float old, float cur, float thresh);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float bw;
	float dnoise[BANDS];
	float oscf, band_freq;

	// single channel in - break it down to bands
	fb.Process(&in[0][0]);	

	// Pass it through the Hilbert transform and calculate per band modulation
	bw = FIRST_BW;
	for (size_t i = 0; i < BANDS; i++)
	{
	  pdn[i].Process(&fb.out[i][0]);	

	  if (ps_noise_mode) {
	    float pnoise = pnoise_filt[i].Process(plfsr[i].Process());
	    pnoise = (pnoise > 1.0) ?  1.0 : (pnoise < -1.0) ? -1.0 : pnoise;
	    oscf = (cur_pn * MAX_PS * SEMITONE) * pnoise * bw;
	  } else {
	    oscf = (cur_ps * MAX_PS * SEMITONE) * bw;
	  }
	  band_freq = (FIRST_BW << i);
	  oscf = ((band_freq + oscf) > (SAMPLE_RATE / 2)) ? ((SAMPLE_RATE / 2) - band_freq) : ((band_freq - oscf) < FIRST_BW) ? FIRST_BW : oscf;
	  osc[i].SetFreq(oscf);

	  if (del_noise_mode) {
	    float tmp_dnoise = dnoise_filt[i].Process(dlfsr[i].Process());
	    tmp_dnoise = dnoise[i] = (tmp_dnoise > 1.0) ?  1.0 : (tmp_dnoise < -1.0) ? -1.0 : tmp_dnoise;
	    dnoise[i] = MAX_DELAY / 2 + tmp_dnoise * (cur_delnoise * MAX_DELAY / 2);
	  } else {
	    dosc[i].SetFreq(cur_delfreq * DOSC_MAX);
	  }

	  bw *= cur_bwm;
	}

	// apply modulation to PDN output per sample per band
	for (size_t i = 0; i < size; i++)
	{
	  out[0][i] = 0;
	  out[1][i] = 0;

	  float cur_sample_l = 0;
	  float cur_sample_r = 0;
	  float tmp_sample_l = 0;
	  float tmp_sample_r = 0;
	  bool even = true;
	  for (size_t j = 0; j < BANDS; j++)
	  {
	    osc[j].Process();
	    dosc[j].Process();
	    if (!del_noise_mode) {
	      dnoise[j] = (float)(((dosc[j].InPhase(0) + 1) / 2.0) * cur_deldepth);
	    }
	    float fs = del[j].ReadHermite(dnoise[j]);
	    del[j].Write(((pdn[j].r_out[i] * osc[j].InPhase(0)) \
		        + (pdn[j].i_out[i] * osc[j].QuadPhase(0))));
	    if (even) {
	      cur_sample_l += fs;
	    } else {
	      cur_sample_r += fs;
	    }
	    even = !even;
	  }
	  // spread and mix
	  tmp_sample_l = (1 + cur_spread) * cur_sample_l + (1 - cur_spread) * cur_sample_r;
	  tmp_sample_r = (1 + cur_spread) * cur_sample_r + (1 - cur_spread) * cur_sample_l;
	  out[0][i] = (tmp_sample_l * cur_mix) + ((1 - cur_mix) * in[0][i]);
	  out[1][i] = (tmp_sample_r * cur_mix) + ((1 - cur_mix) * in[0][i]);
	}
	blink_cnt++;
}

void UpdateEncoder()
{

  cur_page = (page)(cur_page + hw.encoder.Increment());
  if (cur_page >= LAST) { cur_page = MIX; }
  switch(cur_page)
  {
    case MIX:
      cur_mix = CatchParam(cur_mix, mix.Process(), CATCH_THRESH);
      cur_spread = CatchParam(cur_spread, spread.Process(), CATCH_THRESH);
      hw.led1.Set(1.0f,0,0);
      break;
    case PHASE:
      if (ps_noise_mode) {
        cur_pn  = CatchParam(cur_pn, pnshift.Process(), CATCH_THRESH);
      } else {
        cur_ps  = CatchParam(cur_ps, pshift.Process(), CATCH_THRESH);
      }
      cur_bwm  = CatchParam(cur_bwm, bwmult.Process(), CATCH_THRESH);
      hw.led1.Set(0,1.0f,0);
      break;
    case DEL:
      if (del_noise_mode) {
        cur_delnoise = CatchParam(cur_delnoise, delnoise.Process(), CATCH_THRESH);
      } else {
        cur_delfreq = CatchParam(cur_delfreq, delfreq.Process(), CATCH_THRESH);
        cur_deldepth = CatchParam(cur_deldepth, deldepth.Process(), CATCH_THRESH);
      }
      hw.led1.Set(0,0,1.0f);
      break;
    case LAST:
    default:
      break;
  }
}

void UpdateButtons()
{
  if(hw.button1.RisingEdge())
      ps_noise_mode = !ps_noise_mode;

  if(hw.button2.RisingEdge())
      del_noise_mode = !del_noise_mode;

  hw.led2.Set(ps_noise_mode, 0, del_noise_mode);
}

float CatchParam(float old, float cur, float thresh) 
{
  return (abs(old - cur) < thresh) ? cur : old;
}

int main(void)
{
	float sr;
	float bw;
	// make sure each lfsr starts on something different
	uint32_t plfsr_init[BANDS] = {0xA5A5A5A5, 0x5A5A5A5A, 0xA050A050, 0x0A050A05, 0x50A050A0, 0x050A050A, 0x5555AAAA, 0xAAAA5555 };
	uint32_t dlfsr_init[BANDS] = {0xC7C7C7C7, 0x7C7C7C7C, 0xC070C070, 0x0C070C07, 0x70C070C0, 0x070C070C, 0x7777CCCC, 0xCCCC7777 };
	hw.Init();
	sr = hw.AudioSampleRate();

	// set up sine LUT
	table_t t;
	t.l = floor(sr / TABLE_FUNDAMENTAL);
	t.t = (float *)malloc(t.l * sizeof(float)); 

	for (size_t i = 0; i < t.l ; i++) {
  	  float r = (2 * M_PI) * (i / (1.0 * t.l));
  	  t.t[i] = sin(r);
  	}

	// Init
	bw = FIRST_BW;
	for (size_t i = 0; i < BANDS; i++)
	{
	  osc[i].Init(sr, &t, 0.0f);
	  osc[i].SetFreq(bw / 6);
	  dosc[i].Init(sr, &t, (float)dlfsr_init[i]);
	  dosc[i].SetFreq(5);
	  bw = bw * 2;
	  plfsr[i].Init(plfsr_init[i]);
	  dlfsr[i].Init(dlfsr_init[i]);
	  /*
	   * Coefs for a single pole lp filter generated by octave iir_lp.m
 	   * fs = 48e3
 	   * fc = 10
 	   * some gain added to have an output in the range [-1:1]
 	   * (output may actually be higher than that...)
	   */
	  pnoise_filt[i].Init(0.054699, 0.054699, -0.99869); 
	  dnoise_filt[i].Init(0.054699, 0.054699, -0.99869);

	  del[i].Init();
	}

	//set parameters
    	delnoise.Init(hw.knob1, 0, 1, delnoise.EXPONENTIAL);
    	delfreq.Init( hw.knob1, 0, 1, delfreq.EXPONENTIAL);
    	deldepth.Init(hw.knob2, 0, 1, deldepth.LINEAR);
    	pshift.Init( hw.knob1, -1, 1, pshift.LINEAR); 
    	pnshift.Init( hw.knob1, 0, 1, pnshift.LINEAR); 
    	bwmult.Init(  hw.knob2, 0, 2.1, bwmult.LINEAR); 
    	mix.Init(     hw.knob1, 0, 1, mix.LINEAR); 
    	spread.Init(  hw.knob2, 0, 1, mix.LINEAR); 

	// defaults 
	cur_page = MIX;
	cur_delnoise = cur_delfreq = cur_deldepth = cur_pn = 0;
	cur_mix = cur_spread = 0.5;
	cur_ps = 0.3;
	cur_bwm = 2.0;

	// GO!
	hw.StartAdc();
	hw.StartAudio(AudioCallback);

	while(1) {
	  hw.ProcessDigitalControls();
	  UpdateEncoder();
	  UpdateButtons();
	  hw.UpdateLeds();
	  if (blink_cnt >= BLINK_NUM) {
	    hw.seed.SetLed(led_state);
	    led_state = !led_state;
	    blink_cnt = 0;
	  }
	  hw.DelayMs(1);
	}
}
