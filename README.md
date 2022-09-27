# ParametricChorus

## Author

<!-- Insert Your Name Here -->
Jared ANDERSON

20220207

## Description

<!-- Describe your example here -->
A way of easing myself back into embedded DSP.  
This is meant to be a Parametric Chorus based on that presented by Hal Chamberlin in his Musical Applications of Microprocessors (Second Edition).
He cites the block diagram (Fig. 14-25.) as being taken from notes gathered from a techincal paper delivered by Robert Orban at the 55th convention of the Audio Engineering Society.
I searched low and high for that technical paper but couldn't find it - maybe you can?

This implementation targets the Electro-Smith Daisy Platform on a Pod.
Clone this under the DaisyExamples/pod directory and run make to build

The left input of the line-in is used.
Stereo output is provided.

There are three pages of menu accessible via the encoder. LED1 shows R, G and B respectively.

R = MIX page  
&nbsp;&nbsp;POT1 = Dry/Wet mix  
&nbsp;&nbsp;POT2 = Stereo Spread (Odd vs Even bands panned ѕeparately).  

G = PHASE page  
&nbsp;&nbsp;Button1 - Phase Noise or Phase Shift mode - LED2 toggles Red when in Phase Noise mode, otherwise it's in Phase Shift mode.  
&nbsp;&nbsp;In Phase Noise mode - (LED2 Red/Purple)  
&nbsp;&nbsp;&nbsp;&nbsp;POT1 = Amount of Noise modulation applied to each band  
&nbsp;&nbsp;&nbsp;&nbsp;POT2 = Per Band BW multiplier  
&nbsp;&nbsp;In Phase Shift mode - (LED2 Off/Blue)  
&nbsp;&nbsp;&nbsp;&nbsp;POT1 = Shift amount (CCW is -ve, 12 o'clock is none, CW is +ve)  
&nbsp;&nbsp;&nbsp;&nbsp;POT2 = Per Band BW multiplier  

B = DELAY page  
&nbsp;&nbsp;Button2 - Delay Noise or Delay OSC mode - LED2 toggles Blue when in Delay Noise mode, otherwise it's in Delay Osc mode.  
&nbsp;&nbsp;In Delay Noise mode - (LED2 Blue/Purple)  
&nbsp;&nbsp;&nbsp;&nbsp;POT1 = Amount of Noise modulation applied to each band  
&nbsp;&nbsp;&nbsp;&nbsp;POT2 = Unused  
&nbsp;&nbsp;&nbsp;&nbsp;NOTE: Maybe this needs more work?  Sounds gritty.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Updated on 20220529 with reworked random delay noise modulation - no more grit  
&nbsp;&nbsp;In Delay Osc mode - (LED2 Red/Off)  
&nbsp;&nbsp;&nbsp;&nbsp;POT1 = Delay Mod Rate  
&nbsp;&nbsp;&nbsp;&nbsp;POT2 = Delay Mod Depth  

A kind of demo available if you click here
[![Parametric Chorus Demo](https://i.imgur.com/HxNEgoC.png)](https://imgur.com/HxNEgoC "Parametric Chorus Demo")

